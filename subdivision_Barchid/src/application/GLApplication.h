#ifndef GLAPPLICATION_H
#define GLAPPLICATION_H


#include "GLWidget.h"

#include "Camera.h"
#include "Vector2.h"

#include "SubdivCurve.h"
#include "SubdivSurface.h"
#include "InteractDrawSubdiv.h"

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
  enum EMenu {M_Close,M_Chaikin, M_Dyn, M_Catmull, M_Test_Catmull, M_Cube, M_Hole, M_Dolphin,M_Source};
  EMenu _activeMenu;




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
  p3d::Camera _cameraCurve,_cameraSurface;
  double _frustum=0.01;


  SubdivCurve _subdivCurve;
  SubdivSurface _subdivSurface;

  InteractDrawSubdiv *_interactDrawSubdiv;

};

#endif // GLAPPLICATION_H

