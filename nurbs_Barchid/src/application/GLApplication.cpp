#include "GLApplication.h"
#include "GLTool.h"

#include "Vector3.h"
#include "Vector2.h"


#include <iostream>

/*!
*
* @file
*
* @brief
* @author F. Aubert
*
*/


using namespace std;
using namespace p3d;

GLApplication::~GLApplication() {
}

enum EMenu {M_Draw_Curve, M_Draw_Surface,M_Uniform,M_OpenUniform,M_Bezier,M_Circle,M_Revolution};
EMenu _activeMenu;

GLApplication::GLApplication() {

  //
  _leftPanelMenu << "Draw NURBS (Curve)" << "Draw NURBS (Surface)" << "Uniform" << "Open Uniform" << "Bezier" << "Circle" << "Revolution";
  _activeMenu=M_Draw_Curve;


  _cameraKnot.ortho(-1,1,-1,1,-1,1);
  _cameraCurve.ortho(-2,2,-2,2,0,2);
  _cameraSurface.frustum(-_frustum,_frustum,-_frustum,_frustum,0.03,1000);
  _cameraSurface.position(0,0,10);
  _cameraSurface.lookAt(Vector3(0,0,0));

  _interactDrawNurbs=new InteractDrawNurbs(&_nurbs,&_cameraCurve,&_cameraSurface,&_cameraKnot);

  _nurbs.knotUniform(D_U,6);
  _nurbs.knotUniform(D_V,6);

  _nurbs.degree(D_U,2);
  _nurbs.degree(D_V,2);

}


/** ********************************************************************** **/
void GLApplication::initialize() {
  // appelée 1 seule fois à l'initialisation du contexte
  // => initialisations OpenGL
  glClearColor(1,1,1,1);

  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClearDepth(1);

  p3d::initGLTool();

  // ...
}

void GLApplication::resize(int width,int height) {
  // appelée à chaque dimensionnement du widget OpenGL
  // (inclus l'ouverture de la fenêtre)
  // => réglages liés à la taille de la fenêtre
  _cameraCurve.viewport(width/2,0,width/2,height);
  _cameraSurface.viewport(width/2,0,width/2,height);
  _cameraKnot.viewport(0,0,width/2,height);
  _interactDrawNurbs->width(width);
  _interactDrawNurbs->height(height);
  // ...
}

void GLApplication::update() {
  // appelée toutes les 20ms (60Hz)
  // => mettre à jour les données de l'application
  // avant l'affichage de la prochaine image (animation)
  // ...
  Vector2 mouse(mouseX(),mouseY());

  if (mouseLeftPressed()) {
    _interactDrawNurbs->select(mouse);
  }
  if (mouseLeft()) {
    _interactDrawNurbs->leftMove(mouse);
  }
  if (mouseRightPressed()) {
    _interactDrawNurbs->rightClick(mouse);
  }
  if (mouseRight()) {
    if (_interactDrawNurbs->isModeSurface() && mouse.x()>_interactDrawNurbs->width()/2) {
      updateCameraSurface();
    }
    else {
      _interactDrawNurbs->rightMove(mouse);
    }
  }
  if (accelerateWheel()) {
    _interactDrawNurbs->changeValue(mouse,+1.1);
  }
  if (decelerateWheel()) {
    _interactDrawNurbs->changeValue(mouse,0.9);
  }




  switch (keyPressed()) {
  case Qt::Key_O:_interactDrawNurbs->degree(D_U,_nurbs.degree(D_U)-1);break;
  case Qt::Key_P:_interactDrawNurbs->degree(D_U,_nurbs.degree(D_U)+1);break;
  case Qt::Key_L:_interactDrawNurbs->degree(D_V,_nurbs.degree(D_V)-1);break;
  case Qt::Key_M:_interactDrawNurbs->degree(D_V,_nurbs.degree(D_V)+1);break;

    /*
  case Qt::Key_W:_nurbs.uniform(0);_nurbs.uniform(1);break;
  case Qt::Key_X:_nurbs.openUniform(0);_nurbs.openUniform(1);break;
  case Qt::Key_C:_nurbs.bezier(0);_nurbs.bezier(1);break;
  */
  }


}

/* ************************************************************ */

void GLApplication::updateCameraSurface() {
  if (mouseRight()) {
    Vector3 center=_cameraSurface.pointTo(Coordinate_Local,Vector3(0,0,0));
    Vector3 vertical=Vector3(0,1,0);
    _cameraSurface.translate(center,Coordinate_Local);
    _cameraSurface.rotate(-deltaMouseX()/2.0,vertical,Coordinate_Local);
    _cameraSurface.rotate(deltaMouseY()/2.0,Vector3(1,0,0),Coordinate_Local);
    _cameraSurface.translate(-center,Coordinate_Local);
  }
  if (left()) _cameraSurface.left(0.3);
  if (right()) _cameraSurface.right(0.3);
  if (forward()) _cameraSurface.forward(0.3);
  if (backward()) _cameraSurface.backward(0.3);
  if (accelerateWheel()) {
    _frustum*=1.05;
    _cameraSurface.frustum(-_frustum,_frustum,-_frustum,_frustum,0.03,1000);
  }
  if (decelerateWheel()) {
    _frustum/=1.05;
    _cameraSurface.frustum(-_frustum,_frustum,-_frustum,_frustum,0.03,1000);
  }
}


void GLApplication::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


 _interactDrawNurbs->drawKnot();
 _interactDrawNurbs->drawBasis();
 _interactDrawNurbs->drawControl();
 _interactDrawNurbs->drawNurbs();

}




/** ********************************************************************** **/
/** i = button number, s = button text
 */
void GLApplication::leftPanel(int i,const std::string &) {
  _activeMenu=static_cast<EMenu>(i);

  switch (_activeMenu) {
  case M_Draw_Curve:_interactDrawNurbs->enableCurve();break;
  case M_Draw_Surface:_interactDrawNurbs->enableSurface();break;
  case M_Uniform:_interactDrawNurbs->setUniform();break;
  case M_OpenUniform:_interactDrawNurbs->setOpenUniform();break;
  case M_Bezier:_interactDrawNurbs->setBezier();break;
  case M_Circle:_interactDrawNurbs->setCircle();break;
  case M_Revolution:_interactDrawNurbs->setRevolution();break;
  default:break;
  }

  /*
  switch (i) {
    case 0:...;break;
    case 1:...;break;
    ...
  }
  */
}



