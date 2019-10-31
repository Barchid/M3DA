#include "GLApplication.h"
#include "GLTool.h"

#include "Vector3.h"
#include "Vector2.h"


#include <iostream>

/*!
*
* @file
*
* @brief
* @author F. Aubert
*
*/


using namespace std;
using namespace p3d;

GLApplication::~GLApplication() {
}


GLApplication::GLApplication() {


  // warning : should correspond to enums order (see .h)
  _leftPanelMenu << "Close curve" << "Subdiv Chaikin" << "Subdiv Dyn" << "Subdiv Catmull" <<"Test Catmull" << "model Cube" << "model Hole" << "model Dolphin"<< "show source";
  _activeMenu=M_Chaikin;


  _cameraCurve.ortho(-2,2,-2,2,0,2);
  _cameraSurface.frustum(-_frustum,_frustum,-_frustum,_frustum,0.03,1000);
  _cameraSurface.position(0,0,10);
  _cameraSurface.lookAt(Vector3(0,0,0));

  _interactDrawSubdiv=new InteractDrawSubdiv(&_subdivCurve,&_subdivSurface,&_cameraCurve,&_cameraSurface);

  _subdivSurface.source("002_cube.obj");

}


/** ********************************************************************** **/
void GLApplication::initialize() {
  // appelée 1 seule fois à l'initialisation du contexte
  // => initialisations OpenGL
  glClearColor(1,1,1,1);

  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClearDepth(1);

  p3d::initGLTool();

  // ...
}

void GLApplication::resize(int width,int height) {
  // appelée à chaque dimensionnement du widget OpenGL
  // (inclus l'ouverture de la fenêtre)
  // => réglages liés à la taille de la fenêtre
  _cameraCurve.viewport(0,0,width,height);
  _cameraSurface.viewport(0,0,width,height);
  _interactDrawSubdiv->width(width);
  _interactDrawSubdiv->height(height);
  // ...
}

void GLApplication::update() {
  // appelée toutes les 20ms (60Hz)
  // => mettre à jour les données de l'application
  // avant l'affichage de la prochaine image (animation)
  // ...
  Vector2 mouse(mouseX(),mouseY());

  if (mouseLeftPressed()) {
    _interactDrawSubdiv->leftClick(mouse);
  }
  if (mouseLeft()) {
    _interactDrawSubdiv->leftMove(mouse);
  }
  if (mouseRightPressed()) {
    _interactDrawSubdiv->rightClick(mouse);
  }
  if (mouseRight()) {
    if (_interactDrawSubdiv->isModeSurface()) {
      updateCameraSurface();
    }
    else {
      _interactDrawSubdiv->rightMove(mouse);
    }
  }
  /*
  if (accelerateWheel()) {
    _interactDrawNurbs->changeValue(mouse,+1.1);
  }
  if (decelerateWheel()) {
    _interactDrawNurbs->changeValue(mouse,0.9);
  }
  */



  switch (keyPressed()) {
  case Qt::Key_A : _interactDrawSubdiv->decNbIteration();break;
  case Qt::Key_Z : _interactDrawSubdiv->incNbIteration();break;
  case Qt::Key_Q : _interactDrawSubdiv->decScale();break;
  case Qt::Key_S : _interactDrawSubdiv->incScale();break;
  }


}

/* ************************************************************ */

void GLApplication::updateCameraSurface() {
  if (mouseRight()) {
    Vector3 center=_cameraSurface.pointTo(Coordinate_Local,Vector3(0,0,0));
    Vector3 vertical=Vector3(0,1,0);
    _cameraSurface.translate(center,Coordinate_Local);
    _cameraSurface.rotate(-deltaMouseX()/2.0,vertical,Coordinate_Local);
    _cameraSurface.rotate(deltaMouseY()/2.0,Vector3(1,0,0),Coordinate_Local);
    _cameraSurface.translate(-center,Coordinate_Local);
  }
  if (left()) _cameraSurface.left(0.3);
  if (right()) _cameraSurface.right(0.3);
  if (forward()) _cameraSurface.forward(0.3);
  if (backward()) _cameraSurface.backward(0.3);
  if (accelerateWheel()) {
    _frustum*=1.05;
    _cameraSurface.frustum(-_frustum,_frustum,-_frustum,_frustum,0.03,1000);
  }
  if (decelerateWheel()) {
    _frustum/=1.05;
    _cameraSurface.frustum(-_frustum,_frustum,-_frustum,_frustum,0.03,1000);
  }
}


void GLApplication::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


 _interactDrawSubdiv->draw();

}




/** ********************************************************************** **/
/** i = button number, s = button text
 */
void GLApplication::leftPanel(int i,const std::string &) {
  _activeMenu=static_cast<EMenu>(i);

  switch (_activeMenu) {
  case M_Chaikin:_interactDrawSubdiv->enableChaikin();break;
  case M_Dyn:_interactDrawSubdiv->enableDyn();break;
  case M_Catmull:_interactDrawSubdiv->enableCatmull();break;
  case M_Close:_interactDrawSubdiv->enableCurve();_interactDrawSubdiv->switchClose();break;
  case M_Cube:_subdivSurface.source("002_cube.obj");_interactDrawSubdiv->enableCatmull();break;
  case M_Hole:_subdivSurface.source("cubeHole.obj");_interactDrawSubdiv->enableCatmull();break;
  case M_Dolphin:_subdivSurface.source("dolphin.obj");_interactDrawSubdiv->enableCatmull();break;
  case M_Test_Catmull:_interactDrawSubdiv->switchTestCatmull();break;
  case M_Source:_interactDrawSubdiv->switchSource();break;
  default:break;
  }

  /*
  switch (i) {
    case 0:...;break;
    case 1:...;break;
    ...
  }
  */
}



