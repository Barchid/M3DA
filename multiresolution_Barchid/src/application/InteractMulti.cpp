#include "InteractMulti.h"
#include "GLTool.h"

using namespace p3d;
using namespace std;


InteractMulti::~InteractMulti() {}

InteractMulti::InteractMulti(MultiCurve *c, Camera *curve) {
  _subdivCurve=c;

  _cameraCurve=curve;

  _activeInteract=I_None;
  _drawSample=true;
  _drawMulti=false;
  _levelView=_subdivCurve->nbLevel();
}




Vector3 *InteractMulti::interactPoint(unsigned int i) {
    return &_interactPoint[i];
}


void InteractMulti::interactUpdate(unsigned int i) {
  if (_activeInteract==I_Move) {
    if (_drawDetail) {
      int j=i-_subdivCurve->nbCurrent();
      if (j>=0) {
        _subdivCurve->pointDetail(j,_interactPoint[i]);
      }
      else {
        _subdivCurve->pointCurrent(i,_interactPoint[i]);
        _subdivCurve->analysis();
        _computeCurrentCurveRequest=true;
      }

    }
    else {
      _subdivCurve->pointCurrent(i,_interactPoint[i]);
      _subdivCurve->analysis();
      _computeCurrentCurveRequest=true;
    }

  }
}

unsigned int InteractMulti::interactSize() {
  return _interactPoint.size();
}

void InteractMulti::interactInsert(unsigned int , const Vector3 &p) {
  if (_activeInteract==I_Input) {
    _interactPoint.push_back(Vector3(p.x(),p.y(),0));
    _subdivCurve->addPoint(Vector3(p.x(),p.y(),0));
  }
}


void InteractMulti::setupInteractPoint() {
  _interactPoint.clear();
  cameraInteract(_cameraCurve);
  if (_activeInteract==I_Input) {
    for(auto &p:*_subdivCurve->sourcev()) {
      _interactPoint.push_back(p);
    }
  }
  else {
    for(auto &p:*_subdivCurve->currentCurvev()) {
      _interactPoint.push_back(p);
    }
    if (_drawDetail && _levelView<_subdivCurve->nbLevel()) {
      for(auto &p:*_subdivCurve->detailv(_levelView)) {
        _interactPoint.push_back(p);
      }
    }

  }
}

void InteractMulti::leftClick(const Vector2 &mouse) {
  _activeInteract=I_Move;
  setupInteractPoint();
  selectNearest(mouse,5);
}

void InteractMulti::rightMove(const Vector2 &mouse) {
  _computeResampleRequest=true;
  if ((mouse-_oldMouse).length()<1) return;
  addEnd(mouse);
  _oldMouse=mouse;
}

void InteractMulti::leftMove(const Vector2 &mouse) {
  moveSelected(mouse);
  _subdivCurve->analysis();
  _computeCurrentCurveRequest=true;
}

void InteractMulti::rightClick(const Vector2 &mouse) {
  _activeInteract=I_Input;
  _computeResampleRequest=true;
  setupInteractPoint();
  addEnd(mouse);
  _oldMouse=mouse;
//  _computeSubdivRequest=true;
}

void InteractMulti::changeValue(const Vector2 &m, double incr) {
}


void InteractMulti::computeCurrentCurve() {
  if (_drawMulti) {
    _subdivCurve->synthesis(_levelView);
  }
  _computeCurrentCurveRequest=false;
}



void InteractMulti::drawSource() {
  if (_subdivCurve->nbPoint()==0) return;
  vector<Vector3> toDraw;
  for(auto &p:*_subdivCurve->sourcev()) {
    toDraw.push_back(p);
  }
  toDraw.push_back(_subdivCurve->point(0));

  glPointSize(10);
  p3d::ambientColor=Vector4(0,0,1,0);
  p3d::shaderVertexAmbient();
  p3d::drawPoints(toDraw);
  p3d::drawLineStrip(toDraw);
}

void InteractMulti::drawCurrentCurve() {
  if (_subdivCurve->nbPoint()==0) return;
  if (_computeCurrentCurveRequest) computeCurrentCurve();
  vector<Vector3> toDraw=*_subdivCurve->currentCurvev();
  if (toDraw.size()<1) return;
  toDraw.push_back(_subdivCurve->currentCurvev()->at(0));
  p3d::ambientColor=Vector4(1,0,0,1);
  p3d::shaderVertexAmbient();
  p3d::drawPoints(*_subdivCurve->currentCurvev());
  p3d::drawLineStrip(toDraw);
}

void InteractMulti::drawHighestCurve() {
  if (_subdivCurve->nbPoint()==0) return;
  _subdivCurve->synthesisHighest();
  vector<Vector3> toDraw=*_subdivCurve->currentCurvev();
  if (toDraw.size()<1) return;
  toDraw.push_back(_subdivCurve->currentCurvev()->at(0));
  p3d::ambientColor=Vector4(1,0,1,1);
  p3d::shaderVertexAmbient();
  p3d::drawPoints(*_subdivCurve->currentCurvev());
  p3d::drawLineStrip(toDraw);
  _computeCurrentCurveRequest=true;
}


void InteractMulti::drawResample() {
  glPointSize(5);
  if (_computeResampleRequest) computeResample();
  if (_subdivCurve->nbPoint()==0) return;
  vector<Vector3> toDraw=*_subdivCurve->samplev();
  if (toDraw.size()<1) return;
  toDraw.push_back(_subdivCurve->samplev()->at(0));
  p3d::ambientColor=Vector4(0,0,1,1);
  p3d::shaderVertexAmbient();
  p3d::drawPoints(*_subdivCurve->samplev());
  p3d::drawLineStrip(toDraw);
}


void InteractMulti::drawDetail() {
  if (_levelView>=_subdivCurve->nbLevel()) return;
  glPointSize(5);
  if (_subdivCurve->nbPoint()==0) return;
  vector<Vector3> toDraw=*_subdivCurve->detailv(_levelView);
  if (toDraw.size()<1) return;
  toDraw.push_back(_subdivCurve->detailv(_levelView)->at(0));
  p3d::ambientColor=Vector4(0,1,0,1);
  p3d::shaderVertexAmbient();
  p3d::drawPoints(*_subdivCurve->detailv(_levelView));
  p3d::drawLineStrip(toDraw);
}


void InteractMulti::stepAnalysis() {
  if (_subdivCurve->currentLevel()>0) {
    _subdivCurve->analysisStep();
  }
}

void InteractMulti::stepSynthesis() {
  if (_subdivCurve->currentLevel()<_subdivCurve->nbLevel()) {
    _subdivCurve->synthesisStep();
  }
}


void InteractMulti::computeResample() {
  _subdivCurve->resample();
  _computeResampleRequest=false;
}


void InteractMulti::nbSample(int nb) {
  _subdivCurve->nbSample(nb);
  _computeResampleRequest=true;
}


void InteractMulti::draw() {
  p3d::apply(*_cameraCurve);
//  if (_activeSource) drawSource();
  if (!_drawMulti) {
    if (_drawSample) drawResample();
    drawCurrentCurve();
  }
  else {
    if (_drawHighest) {
      drawHighestCurve();
    }
    drawCurrentCurve();
    if (_drawDetail) drawDetail();
  }
}


void InteractMulti::activeMultiResolution() {
   _drawMulti=true;
   _subdivCurve->analysisHighest();
}


