#include "InteractPosition.h"
#include <algorithm>
#include <iostream>
#include <math.h>


#include "Vector3.h"
#include "GLTool.h"

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

InteractPosition::~InteractPosition() {
}

InteractPosition::InteractPosition() {
  _selected=-1;
  _cameraInteract=NULL;
}



void InteractPosition::backup() {
  _backup.clear();
  for (unsigned int i=0;i<interactSize();++i) {
    _backup.push_back(*interactPoint(i));
  }
}


void InteractPosition::restore() {
  for(unsigned int i=0;i<interactSize();++i) {
    *interactPoint(i)=_backup[i];
  }
}


void InteractPosition::add(const Vector2 &mouse) {
  Vector3 addPoint;
  Line ray;
  _cameraInteract->windowToRayWorld(mouse,&ray);
  addPoint=ray.point(1.1);
  int i;
  if (interactSize()>1) {
    i=_cameraInteract->selectNearest(mouse,{*interactPoint(0),*interactPoint(interactSize()-1)},0.1);
    if (i!=0) i++;
  } else {
    i=0;
  }
  interactInsert(i,addPoint);
  interactUpdate(i);
}

void InteractPosition::addEnd(const Vector2 &mouse) {
  Vector3 addPoint;
  Line ray;
  _cameraInteract->windowToRayWorld(mouse,&ray);
  addPoint=ray.point(1.1);
  interactInsert(interactSize(),addPoint);
  interactUpdate(interactSize()-1);
}



unsigned int InteractPosition::nearestPoint(const Vector2 &m,float radius) {
  vector<Vector3> src;
  src.resize(interactSize());
  for(unsigned int i=0;i<interactSize();++i) {
    src[i]=*interactPoint(i);
  }
  return _cameraInteract->selectNearest(m,src,radius*2);
}

unsigned int InteractPosition::selectNearest(const Vector2 &m, double seuil) {
    _selected=nearestPoint(m,seuil);
    return _selected;
}


void InteractPosition::movePoint(unsigned int i,const Vector2 &m) {
  Vector3 current=*interactPoint(i);
  _cameraInteract->windowToWorld(m,current,interactPoint(i));
  interactUpdate(i);
}

/*
void InteractPosition::startMouse(const Vector2 &m) {
  Vector3 *nearest;
  nearest=interactPoint(nearestPoint(m,0.1));
  _camera->windowToWorld(m,*nearest,&_startingMouse);
}
*/
void InteractPosition::moveSelected(const Vector2 &m) {
    if(_selected>=0) {
        movePoint(_selected,m);
    }
}

void InteractPosition::forceSelect(int i) {
  _selected=i;
}


void InteractPosition::moveValue(const Vector2 &m,double incr) {
  int nearest=nearestPoint(m,10);
  if (nearest!=-1) {
    double *v=interactValue(nearest);
    if (v) *v+=incr;
  }
}




