#ifndef GLAPPLICATION_H
#define GLAPPLICATION_H


#include "GLWidget.h"

#include "Camera.h"
#include "Vector2.h"

#include "MultiCurve.h"
#include "InteractMulti.h"

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
  enum EMenu {M_NB16,M_NB2048,M_Analysis_Step,M_Synthesis_Step,M_Multiresolution,M_Draw_Highest,M_Draw_Detail,M_Draw_Sample};
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

  /** ***** **/



private:
  p3d::Camera _cameraCurve;
  double _frustum=0.01;


  MultiCurve _subdivCurve;

  InteractMulti *_interactMulti;

};

#endif // GLAPPLICATION_H

