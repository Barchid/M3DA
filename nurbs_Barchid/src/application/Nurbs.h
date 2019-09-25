#ifndef NURBS_H
#define NURBS_H

#include <vector>
#include "Vector4.h"
#include "Vector3.h"
#include "InteractPosition.h"

enum EDirection {D_U=0,D_V=1};

class Nurbs
{
public:
  enum EKnot {Knot_Uniform,Knot_OpenUniform,Knot_Bezier};

  virtual ~Nurbs();
  Nurbs();

  /** *********************************** **/
  /** Getters/Setters  **/


  int nbKnot(EDirection direction) {return _knot[direction].size();}
  double &knot(EDirection direction,int i) {return _knot[direction][i];}
  void knot(EDirection direction,int i,double t) {_knot[direction][i]=t;}
  const std::vector<double> &knot(EDirection direction) {return _knot[direction];}

  int nbControl(int direction) {return _nbControl[direction];}
  void degree(EDirection direction,int degree) {if (degree>=0) _degree[direction]=degree;knotRemap(direction);}
  int degree(EDirection direction) {return _degree[direction];}

  p3d::Vector4 control(int i) {return _control[i];}
  p3d::Vector4 control(int i,int j) {return _control[i+j*_nbControl[0]];}
  void control(int i,const p3d::Vector4 &p) {_control[i]=p;}
  void control(int i,int j,const p3d::Vector4 &p) {_control[i+j*_nbControl[0]]=p;}
  std::vector<p3d::Vector4> *controlv() {return &_control;}

  /// start of the interval definition
  double startInterval(EDirection direction);
  /// end of the interval definition
  double endInterval(EDirection direction);
  /// tests if u is in the interval definition
  bool inInterval(EDirection direction,double t);

  void addControlU(const p3d::Vector4 &p);

  /** *********************************** **/
  /** Knots setup **/

  bool checkNbKnot(EDirection direction);


  void knotUniform(EDirection direction,int nb=0);
  void knotOpenUniform(EDirection direction);
  void knotBezier(EDirection direction);



  void knotRemap(EDirection direction);

  /** ******************************************* **/
  /** Computations **/

  double evalNkp(int k, int p, double u, const std::vector<double> &knot);
  double evalNkp(EDirection direction, int k, int p, double u);
  p3d::Vector3 pointCurve(double u);
  p3d::Vector3 pointSurface(double u,double v);

  /** ******************************************* **/
  /** Control points manipulations **/

  void initControlGrid();
  void clearControl();
  void setCircle();
  void setRevolution(int nbV);

  /** ******************************************* **/

private:

  /** NURBS data **/
  std::vector<p3d::Vector4> _control; /// control points
  int _nbControl[2];

  std::vector<double> _knot[2];

  int _degree[2];


  // do not clone => copy constructor/operator are private
  Nurbs(const Nurbs &) {}
  Nurbs &operator=(const Nurbs &) {return *this;}
};

#endif // NURBS_H

