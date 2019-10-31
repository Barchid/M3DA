#include <cmath>
#include "GLTool.h"
#include "GLRender.h"
#include "Shader.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "VertexArray.h"
#include "Camera.h"

#include <vector>

/*!
*
* @file
*
* @brief
* @author F. Aubert
*
*/

using namespace std;

namespace p3d {


void initGLTool() {
  initGLRender();
  initGLPrimitive();
}

void apply(const p3d::Camera &c) {
  p3d::projectionMatrix=c.projectionMatrix();
  p3d::modelviewMatrix=c.cameraWorld();
  glViewport(c.viewX(),c.viewY(),c.viewWidth(),c.viewHeight());
}

void drawGround(p3d::Texture &texture) {
  texture.bind(0);
  Vector4 saveLight=p3d::lightPosition[0];
  p3d::modelviewMatrix.push();
  p3d::ambientColor=Vector4(0,0,0,1);
  p3d::modelviewMatrix.rotate(-90,Vector3(1,0,0));
  p3d::lightPosition[0]=p3d::modelviewMatrix.transform(Vector4(0,0,1));
  p3d::modelviewMatrix.scale(Vector3(50,50,1));
  p3d::shaderTextureLight();
  p3d::uniformTransformation();
  p3d::uniformLight();
  p3d::uniformMaterial();
  p3d::drawSquare();

  p3d::modelviewMatrix.pop();
  p3d::lightPosition[0]=saveLight;
}

void drawTexture(p3d::Texture &texture,double x,double y,double w,double h,bool depthTexture) {
  texture.bind(0);
  p3d::modelviewMatrix.push();
  p3d::projectionMatrix.push();
  p3d::projectionMatrix=Matrix4::fromOrtho(0,1,0,1,0,1);
  // square is drawn in [-1,1]
  p3d::modelviewMatrix.setIdentity();
  p3d::modelviewMatrix.translate(x,y,-0.00001);
  p3d::modelviewMatrix.scale(w/2.0,h/2.0,1);
  p3d::modelviewMatrix.translate(1,1,0);
  if (depthTexture)
    p3d::shaderTextureDepth();
  else
    p3d::shaderTextureAmbient();

  p3d::drawSquare();
  p3d::projectionMatrix.pop();
  p3d::modelviewMatrix.pop();
}

void drawArrow(const p3d::Vector3 &a,const p3d::Vector3 &u,double radius,const std::string &s1,const std::string &s2,double arrowRatio) {
  if (fabs(u.length2())<0.000001) return;
  p3d::modelviewMatrix.push();

  Quaternion q;
  q.setRotation(Vector3(0,0,1),u);
  double aqq;Vector3 uqq;
  q.toAngleAxis(&aqq,&uqq);
  p3d::modelviewMatrix.translate(a);
  p3d::modelviewMatrix.rotate(q);

  p3d::modelviewMatrix.push();
  p3d::modelviewMatrix.scale(radius,radius,u.length()*(1-arrowRatio));
  p3d::shaderVertexAmbient();
  p3d::drawCylinder();
  p3d::modelviewMatrix.pop();



  p3d::modelviewMatrix.translate(0,0,u.length()*1);
  p3d::modelviewMatrix.rotate(180,1,0,0);
  p3d::modelviewMatrix.scale(radius*2,radius*2,u.length()*arrowRatio);
  p3d::shaderLightPhong();
  p3d::drawCone();
  p3d::modelviewMatrix.pop();

  p3d::modelviewMatrix.push();

  if (s1!="")
  GLText::draw(s1,a.x(),a.y(),a.z(),nullptr,(p3d::projectionMatrix*p3d::modelviewMatrix).fv(),p3d::ambientColor.fv());
  if (s2!="") {
    Vector3 end=a+u;
    GLText::draw(s2,end.x(),end.y(),end.z(),nullptr,(p3d::projectionMatrix*p3d::modelviewMatrix).fv(),p3d::ambientColor.fv());
  }


  p3d::modelviewMatrix.pop();


}

void draw(const std::string &s,const Vector3 &pos) {
  glDepthMask(GL_FALSE);
  GLText::draw(s,pos.x(),pos.y(),pos.z(),(p3d::projectionMatrix*p3d::modelviewMatrix).fv(),nullptr,p3d::ambientColor.fv());
  glDepthMask(GL_TRUE);
}

void draw(const std::string &s,const Vector2 &pos) {
  glDepthFunc(GL_ALWAYS);
  GLText::draw(s,pos.x(),pos.y(),0,nullptr,nullptr,p3d::ambientColor.fv());
  glDepthFunc(GL_LESS);
}


void draw(const std::string &s,const Vector2 &pos,const Vector4 &color) {
  glDepthFunc(GL_ALWAYS);
  GLText::draw(s,pos.x(),pos.y(),0,nullptr,nullptr,color.fv());
  glDepthFunc(GL_LESS);
}


void draw(int v,const Vector3 &pos) {draw(toString(v),pos);}


void drawThickLineStrip(const std::vector<Vector3> &p) {
  if (p.size()<2) return;
    Vector3 dir;
    Matrix4 t=projectionMatrix*modelviewMatrix;
    Vector3 p0,p1;
    Vector4 p0h,p1h;
    vector<Vector3> pts;

    Vector4 vv(0,0,0,1);
    Vector4 vvh=projectionMatrix*vv;

    pts.clear();
    double bias=0.99; // bias for clipping close to the near plane (debug purpose)
    for(unsigned int i=0;i<p.size()-1;++i) {
      p0h=t*Vector4(p[i],1);
      p1h=t*Vector4(p[i+1],1);

      /* own clipping with near plane (the case w close to 0 is impossible to treat for 3D operation to make the thick line */
      if (p0h.z()<-bias*p0h.w() && p1h.z()<-bias*p1h.w()) continue;
      else if (p0h.z()<-bias*p0h.w() || p1h.z()<-bias*p1h.w()) {
        double lambda=(bias*p0h.w()+p0h.z())/((bias*p0h.w()+p0h.z())-(bias*p1h.w()+p1h.z()));
        if (lambda<0 || lambda>1) continue;
        Vector4 pih=(1-lambda)*p0h+lambda*p1h;
        if (p0h.z()<-fabs(bias*p0h.w())) {
//          cout << "before p0 : " << p0h.w() << " ";
          p0h=pih;

        }
        else {
//          cout << "before p1 : " << p1h.w() << " ";
          p1h=pih;
        }
//        cout << pih.z() << " et " << pih.w() << " div =" << pih.z()/pih.w() << endl;
      }
      /* clipping done */

      p0=p0h.project();
      p1=p1h.project();

      dir=p1-p0;

      dir=Vector3(-dir.y(),dir.x(),0);
      bool ok;
      dir.normalize(&ok);
      if (!ok) dir=Vector3(0,1,0);
      dir*=0.01;
      pts.push_back(p0-dir);
      pts.push_back(p1-dir);
      pts.push_back(p0+dir);

      pts.push_back(p0+dir);
      pts.push_back(p1-dir);
      pts.push_back(p1+dir);
    }
    modelviewMatrix.push();
    projectionMatrix.push();
    modelviewMatrix.setIdentity();
    projectionMatrix.setIdentity();
    p3d::shaderVertexAmbient();
    p3d::draw(GL_TRIANGLES,pts);
    projectionMatrix.pop();
    modelviewMatrix.pop();
}

void drawFrame(const p3d::Matrix4 &frame, const std::string &name, double size,int pos) {
  p3d::ambientColor=Vector4(1,0,0,1);
  p3d::drawArrow(frame.column(3).xyz(),frame.column(0).xyz()*size,0.01*size);
  p3d::ambientColor=Vector4(0,1,0,1);
  p3d::drawArrow(frame.column(3).xyz(),frame.column(1).xyz()*size,0.01*size);
  p3d::ambientColor=Vector4(0,0,1,1);
  p3d::drawArrow(frame.column(3).xyz(),frame.column(2).xyz()*size,0.01*size);
  Vector3 posString=frame.column(3).xyz()+frame.column(0).xyz();
  if (pos==1) posString+=Vector3(0,-0.15,0);
  p3d::draw(name,posString);
}


} // namespace p3d



