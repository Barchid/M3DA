#ifndef GLAPPLICATION_H
#define GLAPPLICATION_H


#include "GLWidget.h"

#include "Camera.h"
#include "Vector2.h"

#include "Nurbs.h"
#include "InteractDrawNurbs.h"

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

  QSize sizeHint() const {return QSize(1000,1000);}
  /** ***** **/

  virtual void leftPanel(int i, const std::string &);
  void updateCameraSurface();

  /** ***** **/



private:
  p3d::Camera _cameraCurve,_cameraSurface,_cameraKnot;
  double _frustum=0.01;


  Nurbs _nurbs;
  InteractDrawNurbs *_interactDrawNurbs;

};

#endif // GLAPPLICATION_H

