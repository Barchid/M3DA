#ifndef INTERACTMULTI_H
#define INTERACTMULTI_H

#include "InteractPosition.h"
#include "MultiCurve.h"
#include "Matrix4.h"

class InteractMulti : public p3d::InteractPosition
{
  enum EInteract {I_Input,I_Move,I_None};

public:
  virtual ~InteractMulti();
  InteractMulti(MultiCurve *c, p3d::Camera *curve);


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

  void draw();
  void drawSource();
  void drawResample();
  void drawCurrentCurve();



  /** ******************************************* **/

  void computeResample();
  void computeCurrentCurve();

  void incLevel() {if (_levelView<_subdivCurve->nbLevel()) {_levelView++;_computeCurrentCurveRequest=true;}}
  void decLevel() {if (_levelView>0) {_levelView--;_computeCurrentCurveRequest=true;}}
  void switchSource() {_activeSource=!_activeSource;}
  void switchDrawSample() {_drawSample=!_drawSample;}
  void activeMultiResolution();
  void switchDrawHighest() {_drawHighest=!_drawHighest;}
  void switchDrawDetail() {_drawDetail=!_drawDetail;}


  void nbSample(int nb);
  void stepAnalysis();
  void stepSynthesis();
  void drawHighestCurve();
  void drawDetail();
private:
  MultiCurve *_subdivCurve;


  EInteract _activeInteract;
  p3d::Vector2 _oldMouse;

  std::vector<p3d::Vector3>  _interactPoint;



  p3d::Camera *_cameraCurve;

  int _width,_height;
  bool _computeResampleRequest=false;
  bool _computeCurrentCurveRequest=false;

  bool _activeSource=true;

  int _levelView;

  bool _drawSample;
  bool _drawMulti=false;
  bool _drawHighest=true;
  bool _drawDetail=false;
};

#endif // INTERACTDRAWNURBS_H

