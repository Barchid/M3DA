#ifndef INTERACTDRAWNURBS_H
#define INTERACTDRAWNURBS_H

#include "InteractPosition.h"
#include "Nurbs.h"
#include "Matrix4.h"

class InteractDrawNurbs : public p3d::InteractPosition
{
  enum EDraw {D_Curve,D_Surface};
  enum EInteract {I_Knot,I_Control,I_CurrentValue,I_None};

public:
  virtual ~InteractDrawNurbs();
  InteractDrawNurbs(Nurbs *n, p3d::Camera *curve, p3d::Camera *surface, p3d::Camera *knot);


  /** ***************************************** **/
  /** INTERACTION **/
  p3d::Vector3 *interactPoint(unsigned int i);
  void interactUpdate(unsigned int i);
  unsigned int interactSize();
  void interactInsert(unsigned int, const p3d::Vector3 &p);
  //double *interactValue(unsigned int i);


  void select(const p3d::Vector2 &mouse);
  void leftMove(const p3d::Vector2 &mouse);
  void rightClick(const p3d::Vector2 &mouse);
  void rightMove(const p3d::Vector2 &mouse);
  void changeValue(const p3d::Vector2 &m, double incr);


  void width(int width) {_width=width;}
  void height(int height) {_height=height;}
  int width() {return _width;}
  int height() {return _height;}

  void degree(EDirection direction,int d) {_nurbs->degree(direction,d);_computeNurbsRequest=true;}

  void enableCurve();
  void enableSurface();

  void setUniform();
  void setOpenUniform();
  void setBezier();

  /** ******************************************* **/
  /** DRAWING **/
  void drawKnot();
  void drawControlCurve();
  void drawControlSurface();
  void drawBasis();
  void drawCurve();
  void drawSurface();
  void drawNurbs();

  bool computeCurve();
  bool computeSurface();
  bool computeNurbs();


  /** ******************************************* **/
  /// switches to surface mode (all data are defined with 2 directions U and V)
  void modeSurface() {_activeDraw=D_Surface;}
  /// switches to curve mode (all data are defined in only 1 direction; direction V should be ignored).
  void modeCurve() {_activeDraw=D_Curve;}

  /// checks if mode is curve
  bool isModeCurve() {return _activeDraw==D_Curve;}
  /// checks if mode is surface
  bool isModeSurface() {return _activeDraw==D_Surface;}



  void drawControl();
  void setCircle();
  void setRevolution();
  std::vector<p3d::Vector2> basisRepresentation(int k, int p, const std::vector<double> &knot);
private:
  Nurbs *_nurbs;


  EDraw _activeDraw; /// = D_Curve or D_Surface (for D_Curve, all data for direction V are ignored).
  EInteract _activeInteract;

  std::vector<p3d::Vector3>  _interactPoint;
  bool _interactKnotFirstMove;
  int _interactKnotMouseInit;
  int _interactKnotSelected;


  bool _updateCurve,_updateSurface; /// to avoid computation when drawing

  std::vector<p3d::Vector4> _basisColor; // color for basis functions

  int _drawNbPts; // number of points of the drawn nurbs in each direction U and V
  std::vector<p3d::Vector3> _draw; // points of the NURBS to draw

  p3d::Camera *_cameraCurve,*_cameraSurface,*_cameraKnot;
  p3d::Matrix4 _transformKnot[2];

  int _width,_height;
  bool _computeNurbsRequest;

  double _currentValue[2];

};

#endif // INTERACTDRAWNURBS_H

