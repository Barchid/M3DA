#ifndef GLAPPLICATION_H
#define GLAPPLICATION_H


#include "GLWidget.h"

#include "Camera.h"
#include "Vector2.h"

#include <string>

/*!
*
* @file
*
* @brief
* @author F. Aubert
*
*/


class GLApplication : public GLWidget {
  Q_OBJECT
public:
  GLApplication();
  virtual ~GLApplication();


  /** ***** **/
  /** GLApplication must implement these methods : */
  virtual void initialize();
  virtual void update();
  virtual void draw();
  virtual void resize(int width,int height);

  QSize sizeHint() const {return QSize(512,512);}
  /** ***** **/

  virtual void leftPanel(int i, const std::string &);
  void updateCameraExtrusion();

  /** ***** **/
  void drawSquare();
  void drawSection();

  void sectionSquare();
  void sectionCircle();
  void normalSection();

  void pathDefault();
  void pathCircle();

  p3d::Vector3 tangentPathLine(unsigned int i);

  p3d::Vector3 pointSpline(double tNormalized);
  p3d::Vector3 tangentPathSpline(double tNormalized);

  void extrudeLine();
  void extrudeSpline();
  void buildExtrusion();

  void drawPathLine();
  void drawPathSpline();
  void drawPath();

  void drawExtrusion();
  void drawRevolution();

  p3d::Vector3 rotatePlane(const p3d::Vector3 &p,const p3d::Vector3 &normal);
  p3d::Vector3 tangentCurve(int i);

  double scale(double tNormalized);


private:
  p3d::Camera _cameraSection,_cameraPath,_cameraExtrusion;
  double _frustum=0.01;


  std::vector<p3d::Vector3> _path;
  std::vector<p3d::Vector2> _section;

  std::vector<p3d::Vector3> _extrusion;

  std::vector<p3d::Vector2> _normalSection;
  std::vector<p3d::Vector3> _normalExtrusion;


};

#endif // GLAPPLICATION_H

