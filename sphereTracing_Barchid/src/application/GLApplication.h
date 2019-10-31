#ifndef GLAPPLICATION_H
#define GLAPPLICATION_H
#include "GLWidget.h"

#include "Shader.h"
#include "Camera.h"
#include "Matrix4.h"

/*!
*
* @file
*
* @brief
* @author F. Aubert
*
*/

class GLApplication : public GLWidget {
public:
  GLApplication();
  virtual ~GLApplication();

  void updateCamera();

  /** ***** **/

  virtual void initialize();
  virtual void update();
  virtual void draw();
  virtual void resize(int w, int h);
  /** ***** **/
private:
  p3d::Shader _sphereTrace;
  std::vector<p3d::Matrix4> _leaf;
  std::vector<p3d::Vector4> _color;
  std::vector<int> _tree;
  GLuint _squareVao;
  p3d::Camera _camera;

  int _nbNode;
  int _nbLeaf;

};

#endif // EXERCICE_H

