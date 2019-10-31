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
  _leftPanelMenu << "NbSample = 16" << "NbSample = 2048" << "Analysis Step" << "Synthesis Step" << "Multiresolution" << "Draw Highest" << "Draw detail" << "Draw Sample";

  _cameraCurve.ortho(-2,2,-2,2,-2,2);

  _interactMulti=new InteractMulti(&_subdivCurve,&_cameraCurve);

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
  _interactMulti->width(width);
  _interactMulti->height(height);
  // ...
}

void GLApplication::update() {
  // appelée toutes les 20ms (60Hz)
  // => mettre à jour les données de l'application
  // avant l'affichage de la prochaine image (animation)
  // ...
  Vector2 mouse(mouseX(),mouseY());

  if (mouseLeftPressed()) {
    _interactMulti->leftClick(mouse);
  }
  if (mouseLeft()) {
    _interactMulti->leftMove(mouse);
  }
  if (mouseRightPressed()) {
    _interactMulti->rightClick(mouse);
  }
  if (mouseRight()) {
      _interactMulti->rightMove(mouse);
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
  case Qt::Key_A : _interactMulti->decLevel();break;
  case Qt::Key_Z : _interactMulti->incLevel();break;
  }


}

/* ************************************************************ */



void GLApplication::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


 _interactMulti->draw();

}




/** ********************************************************************** **/
/** i = button number, s = button text
 */
void GLApplication::leftPanel(int i,const std::string &) {
  _activeMenu=static_cast<EMenu>(i);

  switch (_activeMenu) {
  case M_NB16:_interactMulti->nbSample(16);break;
  case M_NB2048:_interactMulti->nbSample(2048);break;
  case M_Draw_Sample:_interactMulti->switchDrawSample();break;
  case M_Analysis_Step:_interactMulti->stepAnalysis();break;
  case M_Synthesis_Step:_interactMulti->stepSynthesis();break;
  case M_Multiresolution:_interactMulti->activeMultiResolution();break;
  case M_Draw_Highest:_interactMulti->switchDrawHighest();break;
  case M_Draw_Detail:_interactMulti->switchDrawDetail();break;
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



