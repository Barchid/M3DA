#ifndef INTERACTDRAWNURBS_H
#define INTERACTDRAWNURBS_H

#include "InteractPosition.h"
#include "SubdivCurve.h"
#include "SubdivSurface.h"
#include "Matrix4.h"

class InteractDrawSubdiv : public p3d::InteractPosition
{
  enum EDraw {D_Curve,D_Surface};
  enum EInteract {I_Point,I_None};
  enum ECompute {C_Chaikin,C_Dyn,C_Catmull};

public:
  virtual ~InteractDrawSubdiv();
  InteractDrawSubdiv(SubdivCurve *c, SubdivSurface *s,p3d::Camera *curve, p3d::Camera *surface);


  /** ***************************************** **/
  /** INTERACTION **/
  p3d::Vector3 *interactPoint(unsigned int i);
  void interactUpdate(unsigned int i);
  unsigned int interactSize();
  void interactInsert(unsigned int, const p3d::Vector3 &p);


  void setupInteractPoint();
  void leftClick(const p3d::Vector2 &mouse);
  void leftMove(const p3d::Vector2 &mouse);
  void rightClick(const p3d::Vector2 &mouse);
  void rightMove(const p3d::Vector2 &mouse);
  void changeValue(const p3d::Vector2 &m, double incr);


  void width(int width) {_width=width;}
  void height(int height) {_height=height;}
  int width() {return _width;}
  int height() {return _height;}

  /** ******************************************* **/
  /** DRAWING **/
  void drawCurveSource();
  void drawCurveResult();

  void drawSurfaceSource();
  void drawSurfaceResult();

  void drawResult();

  void draw();
  void drawSource();



  /** ******************************************* **/
  void enableSurface();
  void enableCurve();

  bool isModeCurve() {return _activeDraw==D_Curve;}
  bool isModeSurface() {return _activeDraw==D_Surface;}

  void enableChaikin();

  void computeSubdiv();

  void incNbIteration() {_subdivCurve->incNbIteration();_subdivSurface->incNbIteration();_computeSubdivRequest=true;}
  void decNbIteration() {_subdivCurve->decNbIteration();_subdivSurface->decNbIteration();_computeSubdivRequest=true;}
  void switchClose() {_subdivCurve->switchClose();_computeSubdivRequest=true;}
  void switchTestCatmull() {_testCatmull=!_testCatmull;}
  void switchSource() {_activeSource=!_activeSource;}
  void incScale() {_scale*=1.01;if (_scale>1) _scale=1.0;}
  void decScale() {_scale*=0.99;}


  void enableDyn();
  void enableCatmull();
private:
  SubdivCurve *_subdivCurve;
  SubdivSurface *_subdivSurface;


  EDraw _activeDraw; /// = D_Curve or D_Surface (for D_Curve, all data for direction V are ignored).
  EInteract _activeInteract;
  ECompute _activeCompute;

  std::vector<p3d::Vector3>  _interactPoint;



  p3d::Camera *_cameraCurve,*_cameraSurface;

  int _width,_height;
  bool _computeSubdivRequest;

  bool _testCatmull=false;
  bool _activeSource=true;
  double _scale=0.8;
};

#endif // INTERACTDRAWNURBS_H

