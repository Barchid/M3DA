#ifndef MULTICURVE_H
#define MULTICURVE_H

#include <vector>
#include "Vector4.h"
#include "Vector3.h"


class MultiCurve
{
public:
  virtual ~MultiCurve();
  MultiCurve();

  /** *********************************** **/
  /** Getters/Setters  **/

  std::vector<p3d::Vector3> *sourcev() {return &_source;}
  std::vector<p3d::Vector3> *samplev() {return &_sample;}
  std::vector<p3d::Vector3> *currentCurvev() {return &_currentCurve;}
  std::vector<p3d::Vector3> *detailv(int level) {return &_detail[level];}

  int nbLevel() {return _nbLevel;}
  int currentLevel();
  int nbCurrent() {return _currentCurve.size();}

  void addPoint(const p3d::Vector3 &p);
  void point(int i, const p3d::Vector3 &p);
  void pointDetail(int i,const p3d::Vector3 &p);
  p3d::Vector3 point(int i) {return _source[i];}
  int nbPoint() {return _source.size();}



  /** *********************************** **/
  void analysisStep();
  void analysis();

  void synthesis(int level);
  void synthesisStep();

  void resample();
  void nbSample(int nb);
  void pointCurrent(int i, const p3d::Vector3 &p);
  void synthesisHighest();
  void analysisHighest();
private:

  /** Subdivision data **/
  std::vector<p3d::Vector3> _source;
  std::vector<p3d::Vector3> _sample;
  std::vector<std::vector<p3d::Vector3> > _detail;
  std::vector<p3d::Vector3> _currentCurve;
  p3d::Vector3 _pointLevel0;

  // do not clone => copy constructor/operator are private
  MultiCurve(const MultiCurve &) {}
  MultiCurve &operator=(const MultiCurve &) {return *this;}
  int _nbSample;
  int _nbLevel;
};

#endif

