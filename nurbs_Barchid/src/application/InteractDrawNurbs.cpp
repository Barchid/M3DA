#include "InteractDrawNurbs.h"
#include "GLTool.h"

using namespace p3d;
using namespace std;


InteractDrawNurbs::~InteractDrawNurbs() {}

InteractDrawNurbs::InteractDrawNurbs(Nurbs *n,Camera *curve, Camera *surface,Camera *knot) {
  _nurbs=n;

  _activeDraw=D_Curve;
  _drawNbPts=100;
  _draw.resize(_drawNbPts);

  _cameraCurve=curve;
  _cameraSurface=surface;
  _cameraKnot=knot;

  _updateCurve=true;
  _updateSurface=true;

  /** RENDER **/
  for(unsigned int i=0;i<30;++i) {
    Vector3 c(double(rand())/RAND_MAX+0.5*(i%6==0 || i%6==3 || i%6==5),double(rand())/RAND_MAX+0.5*(i%6==1 || i%6==3 || i%6==4),double(rand())/RAND_MAX+0.5*(i%6==2 || i%6==4 || i%6==5));
    c.normalize();
    _basisColor.push_back(Vector4(c,1));
  }

  _transformKnot[D_U].setTranslation(-0.8,0.2,0);
  _transformKnot[D_V].setTranslation(-0.8,-0.8,0);
  _transformKnot[D_U].scale(1.5,0.7,1);
  _transformKnot[D_V].scale(1.5,0.7,1);

  _activeInteract=I_None;

  _computeNurbsRequest=false;

}


bool InteractDrawNurbs::computeSurface() {
  _draw.clear();
  if (!(_nurbs->checkNbKnot(D_U) && _nurbs->checkNbKnot(D_V))) return false;
  _draw.resize(_drawNbPts*_drawNbPts);

  /* TODO :
   * - compute each drawing point of the surface in _draw (vector of Vector3 interpreted as a grid)
   * - _nurbs->startInterval(D_U) (resp. startInterval(D_V), endInterval(D_U), endInterval(D_V)) returns the start of value u (resp. value v, end u, end v).
   * - loop on indices of _draw (size = _drawNbPts in direction U and V) and update u and v (pre compute the steps)
   *  - _nurbs->pointSurface(u,v) should give the point P(u,v)
   */
  double startU=_nurbs->startInterval(D_U);
  double endU=_nurbs->endInterval(D_U);
  double stepU=(endU-startU)/(_drawNbPts-1);
  double u=startU;

  double startV=_nurbs->startInterval(D_V);
  double endV=_nurbs->endInterval(D_V);
  double stepV=(endV-startV)/(_drawNbPts-1);
  double v=startV;


  Vector4 p;
  for(int i=0;i<_drawNbPts;++i) {
    u=startU;
    for(int j=0;j<_drawNbPts;++j) {
      _draw[i+j*_drawNbPts]=_nurbs->pointSurface(u,v);
      u+=stepU;
    }
    v+=stepV;

  }

  _computeNurbsRequest=false;

  return true;
}

bool InteractDrawNurbs::computeCurve() {
  _draw.clear();
  if (!_nurbs->checkNbKnot(D_U)) return false;
  _draw.resize(_drawNbPts);

  /* TODO :
   * - compute each drawing point of the curve in array _draw (_drawNbPts is the number of points to set; _draw is a vector of Vector3)
   * - _nurbs->startInterval(D_U) (resp. endInterval(D_U)) returns the start of value u (resp. end u).
   * - loop on indices of _draw (size = _drawNbPts) and update u
   * - _nurbs->pointCurve(u) should give the point P(u)
   */


  _computeNurbsRequest=false;
  return true;
}

bool InteractDrawNurbs::computeNurbs() {
  if (_activeDraw==D_Curve) {
    return computeCurve();
  }
  else {
    return computeSurface();
  }
}




Vector3 *InteractDrawNurbs::interactPoint(unsigned int i) {
  return &_interactPoint[i];
}


void InteractDrawNurbs::interactUpdate(unsigned int i) {
  if (_activeInteract==I_Knot) {
    Vector3 unt;
    if (_interactKnotSelected>=_nurbs->nbKnot(D_U)) {
      unt=_transformKnot[D_U].inverse().transformPoint(_interactPoint[_interactKnotSelected]);
    }
    else {
      unt=_transformKnot[D_V].inverse().transformPoint(_interactPoint[_interactKnotSelected]);
    }
    double t=unt.x();
    if (t<0) t=0;
    if (t>1) t=1;
    if (_interactKnotSelected>=_nurbs->nbKnot(D_U)) {
      int k=_interactKnotSelected-_nurbs->nbKnot(D_U);
      if (k>0 && t<_nurbs->knot(D_V,k-1)) _nurbs->knot(D_V,k,_nurbs->knot(D_V,k-1));
      else if (k<_nurbs->nbKnot(D_V)-1 && t>_nurbs->knot(D_V,k+1)) _nurbs->knot(D_V,k,_nurbs->knot(D_V,k+1));
      else _nurbs->knot(D_V,k)=t;
    }
    else {
      int k=_interactKnotSelected;
      if (k>0 && t<_nurbs->knot(D_U,k-1)) _nurbs->knot(D_U,k,_nurbs->knot(D_U,k-1));
      else if (k<_nurbs->nbKnot(D_U)-1 && t>_nurbs->knot(D_U,k+1)) _nurbs->knot(D_U,k,_nurbs->knot(D_U,k+1));
      else _nurbs->knot(D_U,k)=t;
    }


  }
  else if (_activeInteract==I_Control) {
    _nurbs->control(i,Vector4(_interactPoint[i]*_nurbs->control(i).w(),_nurbs->control(i).w()));
  }

}

unsigned int InteractDrawNurbs::interactSize()
{
  if (_activeInteract==I_Knot) {
    return _interactPoint.size();
  }
  else if (_activeInteract==I_Control) {
    if (_activeDraw==D_Curve) {
      return _nurbs->nbControl(D_U);
    }
    else {
      return _nurbs->nbControl(D_U)*_nurbs->nbControl(D_V);
    }
  }
  else
    return 0;
}

void InteractDrawNurbs::interactInsert(unsigned int , const Vector3 &p) {
  _interactPoint.push_back(p);
  _nurbs->addControlU(Vector4(p,1));
  _computeNurbsRequest=true;
}


void InteractDrawNurbs::select(const Vector2 &mouse) {
  if (mouse.x()<_width/2) { // interaction on knot
    _activeInteract=I_Knot;
    cameraInteract(_cameraKnot);
    _interactPoint.clear();
    _interactKnotFirstMove=true;
    _interactKnotMouseInit=mouse.x();
    for(int i=0;i<_nurbs->nbKnot(D_U);++i) {
      _interactPoint.push_back(_transformKnot[D_U].transformPoint(Vector3(_nurbs->knot(D_U,i),0,0)));
    }
    for(int i=0;i<_nurbs->nbKnot(D_V);++i) {
      _interactPoint.push_back(_transformKnot[D_V].transformPoint(Vector3(_nurbs->knot(D_V,i),0,0)));
    }
    _interactKnotSelected=selectNearest(mouse,2);

  }
  else {
    _activeInteract=I_Control;
    _interactPoint.clear();
    for(auto &p:*_nurbs->controlv()) {
      _interactPoint.push_back(p.project());
    }
    if (_activeDraw==D_Curve) {
      cameraInteract(_cameraCurve);
    }
    else {
      cameraInteract(_cameraSurface);
    }

    selectNearest(mouse,5);

  }

}

void InteractDrawNurbs::rightMove(const Vector2 &mouse) {
  if (_activeInteract==I_CurrentValue) {
    EDirection direction;
    if (mouse.y()>_height/2) direction=D_U; else direction=D_V;
    Vector3 p;
    bool ok=_cameraKnot->windowToWorld(mouse,&p);
    if (ok) {
      _transformKnot[direction].inverse().transformPoint(&p);
      _currentValue[direction]=p.x();
      if (_currentValue[direction]<_nurbs->startInterval(direction)) _currentValue[direction]=_nurbs->startInterval(direction);
      else if (_currentValue[direction]>_nurbs->endInterval(direction)) _currentValue[direction]=_nurbs->endInterval(direction);
    }
  }
}

void InteractDrawNurbs::leftMove(const Vector2 &mouse) {
  if (_activeInteract==I_Knot) {
    if (_interactKnotFirstMove) {
      if (mouse.x()==_interactKnotMouseInit) return;
      if (mouse.x()>_interactKnotMouseInit) {
        unsigned int i=_interactKnotSelected;
        while (i<_interactPoint.size()-1 && fabs(_interactPoint[i+1].distance(_interactPoint[i]))<0.0001) {
          ++i;
        }
        _interactKnotSelected=i;
        forceSelect(i);
      }
      _interactKnotFirstMove=false;
    }
    moveSelected(mouse);
    _computeNurbsRequest=true;
  }
  else {
    moveSelected(mouse);
    _computeNurbsRequest=true;
  }

}

void InteractDrawNurbs::rightClick(const Vector2 &mouse) {
  if (mouse.x()<_width/2) {
    _activeInteract=I_CurrentValue;
  }
  else {
    if (_activeDraw==D_Curve) {
      _interactPoint.clear();
      for(auto &p:*_nurbs->controlv()) {
        _interactPoint.push_back(p.project());
      }
      _activeInteract=I_Control;
      if (_activeDraw==D_Curve) cameraInteract(_cameraCurve);
      else if (_activeDraw==D_Surface) cameraInteract(_cameraSurface);
      addEnd(mouse);
      _computeNurbsRequest=true;
    }
  }
}

void InteractDrawNurbs::changeValue(const Vector2 &m, double incr) {
  if (_activeInteract==I_Control) {
    int i=nearestPoint(m,5);
    if (i!=-1) {
      Vector4 *p;
      p=&((_nurbs->controlv())->at(i));
      (*p)*=incr;
      _computeNurbsRequest=true;
    }
  }

}

void InteractDrawNurbs::drawKnot() {
  p3d::apply(*_cameraKnot);
  vector<Vector3> toDraw;
  EDirection direction;

  for(int dd=0;dd<=(_activeDraw==D_Surface)?1:0;++dd) {
    direction=static_cast<EDirection>(dd); // direction is D_U or D_V
    toDraw.clear();
    for(int i=0;i<_nurbs->nbKnot(direction);++i) {
      Vector3 k=Vector3(_nurbs->knot(direction,i),0);
      toDraw.push_back(k);
    }

    glPointSize(5);

    p3d::modelviewMatrix.push();

    p3d::modelviewMatrix*=_transformKnot[direction];

    p3d::ambientColor=Vector4(1,0,0,0);

    p3d::shaderVertexAmbient();
    p3d::drawPoints(toDraw);

    if (_nurbs->inInterval(direction,_currentValue[direction])) {
      toDraw.clear();
      toDraw.push_back(Vector3(_currentValue[direction],-0.1,0));
      toDraw.push_back(Vector3(_currentValue[direction],1,0));
      p3d::ambientColor=Vector4(1,0,0,1);
      p3d::drawLineStrip(toDraw);
    }

    p3d::modelviewMatrix.pop();
  }

}


void InteractDrawNurbs::drawControl() {
  if (_activeDraw==D_Curve) {
    p3d::apply(*_cameraCurve);
    drawControlCurve();
  }
  else {
    p3d::apply(*_cameraSurface);
    drawControlSurface();
  }
}


void InteractDrawNurbs::drawControlCurve() {
  vector<Vector3> toDraw;
  for(auto &p:*_nurbs->controlv()) {
    toDraw.push_back(p.project());
  }

  p3d::ambientColor=Vector4(0,0,1,0);
  p3d::shaderVertexAmbient();
  p3d::drawPoints(toDraw);
  p3d::drawLineStrip(toDraw);
}

void InteractDrawNurbs::drawControlSurface() {
  vector<Vector3> toDraw;
  for(auto &p:*_nurbs->controlv()) {
    toDraw.push_back(p.project());
  }

  p3d::ambientColor=Vector4(0,0,1,0);
  p3d::shaderVertexAmbient();
  p3d::drawPoints(toDraw);
  p3d::drawGrid(toDraw,_nurbs->nbControl(D_U));
}

void InteractDrawNurbs::enableCurve() {
  _drawNbPts=100;
  _nurbs->clearControl();
  _computeNurbsRequest=true;
  _activeDraw=D_Curve;
}


void InteractDrawNurbs::enableSurface() {
  _nurbs->initControlGrid();
  _drawNbPts=50;
  _computeNurbsRequest=true;
  _activeDraw=D_Surface;
}


vector<Vector2> InteractDrawNurbs::basisRepresentation(int k,int p,const std::vector<double> &knot) {

  int nbPoint=40;
  vector<Vector2> representation;

  /* TODO : compute nbPoint of the representation of N(k,p) in toDraw :
   * - loop to set each point in toDraw (toDraw.push_back(Vector2(u,Nkp)),
   * - _nurbs->evalNkp(k,p,u,knot) is the value of the basis function Nkp(u) for the parameter u (done in previous question)
   * - compute the representation where the basis function is non-zero (i.e. in the interval [??,??] (you have to determine the interval))
   */


  return representation;
}


void InteractDrawNurbs::drawBasis()
{
  vector<Vector2> toDraw;
  EDirection direction;

  for(int dd=0;dd<=(_activeDraw==D_Surface)?1:0;++dd) {
    direction=static_cast<EDirection>(dd); // direction is D_U or D_V
    p3d::modelviewMatrix.push();
    p3d::modelviewMatrix*=_transformKnot[direction];

    int nbPoint=30; // number of points to draw the basis curve.
    int nbBasis=_nurbs->nbKnot(direction)-_nurbs->degree(direction)-1; // number of basis curve to draw

    for(int k=0;k<nbBasis;++k) { // draw nbBasis basis functions
      toDraw.clear();
      p3d::ambientColor=_basisColor[k%30]; // color of the drawn basis curve
      p3d::shaderVertexAmbient();

      toDraw=this->basisRepresentation(k,_nurbs->degree(direction),_nurbs->knot(direction));
      // end TODO

      // draw the resulting line strip :
      p3d::drawLineStrip(toDraw);
    }
    p3d::modelviewMatrix.pop();

  }
}

void InteractDrawNurbs::drawCurve() {
  p3d::ambientColor=Vector4(1,0,0,1);
  p3d::shaderVertexAmbient();
  p3d::drawLineStrip(_draw);
}

void InteractDrawNurbs::drawSurface() {
  p3d::ambientColor=Vector4(1,0,0,1);
  p3d::shaderVertexAmbient();
  p3d::drawGrid(_draw,_drawNbPts);
}


void InteractDrawNurbs::drawNurbs() {
  if (_computeNurbsRequest) computeNurbs();
  if (_activeDraw==D_Curve) {
    p3d::apply(*_cameraCurve);
    drawCurve();
    if (_nurbs->inInterval(D_U,_currentValue[D_U])) {
      vector<Vector3> p;
      p.push_back(_nurbs->pointCurve(_currentValue[D_U]));
      p3d::drawPoints(p);
    }
  }
  else {
    p3d::apply(*_cameraSurface);
    drawSurface();
    if (_nurbs->inInterval(D_U,_currentValue[D_U]) && _nurbs->inInterval(D_V,_currentValue[D_V])) {
      vector<Vector3> p;
      p.push_back(_nurbs->pointSurface(_currentValue[D_U],_currentValue[D_V]));
      p3d::ambientColor=Vector4(0.8,0.2,0.9,1.0);
      glPointSize(10);
      p3d::shaderVertexAmbient();
      p3d::drawPoints(p);
    }
  }
}

void InteractDrawNurbs::setUniform() {
  _nurbs->knotUniform(D_U,_nurbs->nbControl(D_U)+_nurbs->degree(D_U)+1);
  _nurbs->knotUniform(D_V,_nurbs->nbControl(D_V)+_nurbs->degree(D_V)+1);
  _computeNurbsRequest=true;
}

void InteractDrawNurbs::setOpenUniform() {
  _nurbs->knotOpenUniform(D_U);
  _nurbs->knotOpenUniform(D_V);
  _computeNurbsRequest=true;
}

void InteractDrawNurbs::setBezier() {
  _nurbs->knotBezier(D_U);
  _nurbs->knotBezier(D_V);
  _computeNurbsRequest=true;
}

void InteractDrawNurbs::setCircle() {
  _activeDraw=D_Curve;
  _nurbs->setCircle();
  _computeNurbsRequest=true;
}

void InteractDrawNurbs::setRevolution() {
  if (_activeDraw==D_Curve) {
    _activeDraw=D_Surface;
    _nurbs->setRevolution(10);
    _computeNurbsRequest=true;
  }
}




