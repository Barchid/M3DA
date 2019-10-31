#ifndef SUBDIVCURVE_H
#define SUBDIVCURVE_H

#include <vector>
#include "Vector4.h"
#include "Vector3.h"


class SubdivCurve
{
public:
  virtual ~SubdivCurve();
  SubdivCurve();

  /** *********************************** **/
  /** Getters/Setters  **/

  std::vector<p3d::Vector3> *sourcev() {return &_source;}
  std::vector<p3d::Vector3> *resultv() {return &_result;}

  void addPoint(const p3d::Vector3 &p);
  void point(int i, const p3d::Vector3 &p);
  p3d::Vector3 point(int i) {return _source[i];}
  int nbPoint() {return _source.size();}

  void incNbIteration() {_nbIteration++;}
  void decNbIteration() {if (_nbIteration>0) _nbIteration--;}
  void switchClose() {_closeCurve=!_closeCurve;}
  bool isClosed() {return _closeCurve;}


  /** *********************************** **/
  void chaikin(); 
  void chaikinIter(const std::vector<p3d::Vector3> &p);
  void dynLevinIter(const std::vector<p3d::Vector3> &p);
  void dynLevin();
private:

  /** Subdivision data **/
  std::vector<p3d::Vector3> _source;
  std::vector<p3d::Vector3> _result;

  // do not clone => copy constructor/operator are private
  SubdivCurve(const SubdivCurve &) {}
  SubdivCurve &operator=(const SubdivCurve &) {return *this;}
  int _nbIteration=0;
  bool _closeCurve=false;
};

#endif // SUBDIVCURVE_H

