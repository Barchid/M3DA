#ifndef GLUTILTEXT_H
#define GLUTILTEXT_H

#include "glsupport.h"
#include "Vector2.h"

#include <sstream>
#include <iomanip>

/*!
*
* @file
*
* @brief
* @author F. Aubert
*
*/



namespace p3d {

template <class T> std::string toString(const T &v) {std::stringstream s;s<< std::setprecision(2) <<v;return s.str();}


namespace GLText {
  void end();




  GLuint toTexture(const std::string &s, int *width, int *height);
  void initTextShader();
  void initialize();
  // render text in OpenGL >= 3.2 : slow cause the texture is computed each call
  void draw(const std::string &s,double x,double y,double z=0,const float *modelview=NULL,const float *projection=NULL,const float *color=NULL);
}

}

#endif // GLUTILTEXT_H

