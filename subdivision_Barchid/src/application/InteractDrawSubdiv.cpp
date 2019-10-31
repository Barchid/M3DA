#include "InteractDrawSubdiv.h"
#include "GLTool.h"
#include "MeshGL.h"

using namespace p3d;
using namespace std;


InteractDrawSubdiv::~InteractDrawSubdiv() {}

InteractDrawSubdiv::InteractDrawSubdiv(SubdivCurve *c, SubdivSurface *s, Camera *curve, Camera *surface) {
  _subdivCurve=c;
  _subdivSurface=s;

  _activeDraw=D_Curve;
  _activeCompute=C_Chaikin;


  _cameraCurve=curve;
  _cameraSurface=surface;


  _activeInteract=I_None;

  _computeSubdivRequest=false;

}




Vector3 *InteractDrawSubdiv::interactPoint(unsigned int i) {
    return &_interactPoint[i];
}


void InteractDrawSubdiv::interactUpdate(unsigned int i) {
  _subdivCurve->point(i,_interactPoint[i]);
}

unsigned int InteractDrawSubdiv::interactSize() {
  return _interactPoint.size();
}

void InteractDrawSubdiv::interactInsert(unsigned int , const Vector3 &p) {
  _interactPoint.push_back(p);
  _subdivCurve->addPoint(p);
}


void InteractDrawSubdiv::setupInteractPoint() {
  _interactPoint.clear();
  if (_activeDraw==D_Curve) {
    cameraInteract(_cameraCurve);
    for(auto &p:*_subdivCurve->sourcev()) {
      _interactPoint.push_back(p);
    }
  }
}

void InteractDrawSubdiv::leftClick(const Vector2 &mouse) {
  if (_activeDraw==D_Curve) {
    setupInteractPoint();
    selectNearest(mouse,5);
  }
}

void InteractDrawSubdiv::rightMove(const Vector2 &mouse) {
}

void InteractDrawSubdiv::leftMove(const Vector2 &mouse) {
  moveSelected(mouse);
  _computeSubdivRequest=true;
}

void InteractDrawSubdiv::rightClick(const Vector2 &mouse) {
  if (_activeDraw==D_Curve) {
    setupInteractPoint();
    addEnd(mouse);
    _computeSubdivRequest=true;
  }
}

void InteractDrawSubdiv::changeValue(const Vector2 &m, double incr) {
}




void InteractDrawSubdiv::drawCurveSource() {
  if (_subdivCurve->nbPoint()==0) return;
  vector<Vector3> toDraw;
  for(auto &p:*_subdivCurve->sourcev()) {
    toDraw.push_back(p);
  }
  if (_subdivCurve->isClosed()) toDraw.push_back(_subdivCurve->point(0));

  glPointSize(10);
  p3d::ambientColor=Vector4(0,0,1,0);
  p3d::shaderVertexAmbient();
  p3d::drawPoints(toDraw);
  p3d::drawLineStrip(toDraw);
}


void InteractDrawSubdiv::enableCurve() {
  _activeDraw=D_Curve;
}


void InteractDrawSubdiv::enableSurface() {
  _activeDraw=D_Surface;
}


void InteractDrawSubdiv::computeSubdiv() {
  switch(_activeCompute) {
  case C_Chaikin:_subdivCurve->chaikin();break;
  case C_Dyn:_subdivCurve->dynLevin();break;
  case C_Catmull:_subdivSurface->catmullClark();break;
  default:
    break;
  }
  _computeSubdivRequest=false;
}


void InteractDrawSubdiv::enableChaikin() {
  _activeDraw=D_Curve;
  _activeCompute=C_Chaikin;
  _computeSubdivRequest=true;
}

void InteractDrawSubdiv::enableDyn() {
  _activeDraw=D_Curve;
  _activeCompute=C_Dyn;
  _computeSubdivRequest=true;
}

void InteractDrawSubdiv::enableCatmull() {
  _activeDraw=D_Surface;
  _activeCompute=C_Catmull;
  _computeSubdivRequest=true;
}

void InteractDrawSubdiv::drawCurveResult() {
  if (_subdivCurve->nbPoint()==0) return;
  vector<Vector3> toDraw=*_subdivCurve->resultv();
  if (toDraw.size()<1) return;
  if (_subdivCurve->isClosed()) toDraw.push_back(_subdivCurve->resultv()->at(0));
  p3d::ambientColor=Vector4(1,0,0,1);
  p3d::shaderVertexAmbient();
  p3d::drawPoints(*_subdivCurve->resultv());
  p3d::drawLineStrip(toDraw);
}

void InteractDrawSubdiv::drawSurfaceSource() {
  p3d::ambientColor=Vector4(0,0,1,1);
  p3d::shaderVertexAmbient();
  _subdivSurface->source()->render()->drawLine(_scale);
}

void InteractDrawSubdiv::drawSurfaceResult() {
  p3d::ambientColor=Vector4(1,0,0,1);
  p3d::shaderVertexAmbient();
  if (_subdivSurface->result()) {
    _subdivSurface->result()->render()->drawLine(_scale);
  }
  if (_testCatmull) _subdivSurface->drawTest();
}


void InteractDrawSubdiv::drawResult() {
  if (_computeSubdivRequest) computeSubdiv();
  if (_activeDraw==D_Curve) {
    p3d::apply(*_cameraCurve);
    drawCurveResult();
  }
  else {
    p3d::apply(*_cameraSurface);
    drawSurfaceResult();
  }
}

void InteractDrawSubdiv::drawSource() {
  if (_activeDraw==D_Curve) {
    drawCurveSource();
  }
  else {
    drawSurfaceSource();
  }
}

void InteractDrawSubdiv::draw() {
  if (_activeDraw==D_Curve) {
    p3d::apply(*_cameraCurve);
  }
  else {
    p3d::apply(*_cameraSurface);
  }
  if (_activeSource) drawSource();
  drawResult();
}


