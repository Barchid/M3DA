#include "GLApplication.h"
#include "GLWidget.h"
#include "Tools.h"

#include <QElapsedTimer>

/*!
*
* @file
*
* @brief
* @author F. Aubert
*
*/



using namespace p3d;
using namespace std;

GLApplication::~GLApplication() {
}


GLApplication::GLApplication() {
    _camera.position(0,0,3);
}


void GLApplication::updateCamera() {
    if (left()) {
        _camera.translate(-0.1,0,0,Coordinate_Local);
    }
    if (right()) {
        _camera.translate(0.1,0,0,Coordinate_Local);
    }
    if (forward()) {
        _camera.translate(0,0,-0.1,Coordinate_Local);
    }
    if (backward()) {
        _camera.translate(0,0,0.1,Coordinate_Local);
    }
    if (mouseLeft()) {
        // rotate camera : deltaMouseX(), deltaMouseY() give the mouse motion
        Vector3 vertical=_camera.directionTo(Coordinate_Local,Vector3(0,1,0));
        auto t=_camera.pointTo(Coordinate_Local,Vector3(0,0,0));
        _camera.translate(t,Coordinate_Local);
        _camera.rotate(-deltaMouseX()/5.0,vertical,Coordinate_Local);
        _camera.rotate(deltaMouseY()/5.0,1,0,0,Coordinate_Local);
        _camera.translate(-t,Coordinate_Local);
    }
}


/** ***************************************************************** **/
void GLApplication::resize(int w,int h) {
    glViewport(0,0,w,h);
    double ratio=double(w)/h;
    _camera.frustum(-0.01*ratio,0.01*ratio,-0.01,0.01,0.03,100.0);

}

void GLApplication::initialize() {
    vector<float> v{-1,-1,-1,1,1,-1,1,1};
    GLuint vb;

    /** init the VAO of the square **/
    glGenBuffers(1,&vb);
    glBindBuffer(GL_ARRAY_BUFFER,vb);
    glBufferData(GL_ARRAY_BUFFER,8*sizeof(GLfloat),v.data(),GL_STATIC_DRAW);

    glGenVertexArrays(1,&_squareVao);
    glBindVertexArray(_squareVao);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,0);

    glBindVertexArray(0);
    /** ** **/

    /** init the shader **/
    _sphereTrace.attribute("vertex",0);

    _sphereTrace.read("shader/sphere_trace");

    /** ** **/


    /** init the CSG (tree, transformation, color) **/

    //_tree={4,2,4,2,0,-1};

    // réunion de trois sphères
    //    _tree={
    //        4,3, // noeud N0 -> sphère
    //        4,3, // noeud N1 -> sphère
    //        4,4, // noeud N2 -> sphère
    //        0,4, // noeud N3 -> N0 U N1
    //        0,-1 // noeud N4 -> N2 U N3 (racine)
    //    };

    // intersection de deux sphères puis union avec une troisième sphère
    //    _tree={
    //        4,3, // noeud N0 -> sphère
    //        4,3, // noeud N1 -> sphère
    //        4,4, // noeud N2 -> sphère
    //        1,4, // noeud N3 -> N0 U N1
    //        0,-1 // noeud N4 -> N2 U N3 (racine)
    //    };

    // intersection de trois sphères
    //    _tree={
    //        4,3, // noeud N0 -> sphère
    //        4,3, // noeud N1 -> sphère
    //        4,4, // noeud N2 -> sphère
    //        1,4, // noeud N3 -> N0 AND N1
    //        1,-1 // noeud N4 -> N2 AND N3 (racine)
    //    };

    // sphere - (sphere U sphere)
    //    _tree={
    //        4,3, // noeud N0 -> sphère
    //        4,3, // noeud N1 -> sphère
    //        4,4, // noeud N2 -> sphère
    //        0,4, // noeud N3 -> N0 U N1
    //        2,-1 // noeud N4 -> N2 - N3 (racine)
    //    };

    // union de toutes les formes différentes que j'ai (càd sphere, cube, cone et cylindre)
    //    _tree={
    //        4,4, // noeud N0 -> sphère
    //        5,4, // noeud N1 -> cube
    //        6,5, // noeud N2 -> cone
    //        7,5, // noeud N3 -> cylindre
    //        0,6, // noeud N4 -> N0 U N1
    //        0,6, // noeud N5 -> N2 U N3
    //        0,-1 // noeud N6 -> N4 U N5
    //    };

    //    // SPHERE U CUBE
    //    _tree={
    //        4,2, // noeud N0 -> sphère
    //        7,2, // noeud N1 -> cube
    //        0,-1 // noeud N2 -> N0 U N1
    //    };

    // Arbre CSG pour avoir la même chose que sur l'énoncé

    _tree = {
        4,5,  // noeud N0 -> sphere
        5,5,  // noeud N1 -> cube
        7,6,  // noeud N2 -> cylindre
        7,6,  // noeud N3 -> cylindre
        7,7,  // noeud N4 -> cylindre
        1,8,  // noeud N5 -> N0 AND N1
        0,7,  // noeud N6 -> N2 U N3
        0,8,  // noeud N7 -> N4 U N6
        2,-1  // noeud N8 -> N5 - N7
    };

    // pour le truc par défaut (union deux sphères)
    //_leaf={Matrix4::fromTranslation(2,0,0),Matrix4::fromTranslation(0,0,0)};

    // pour SPHERE U SPHERE U SPHERE
    //_leaf={Matrix4::fromTranslation(2,0,0),Matrix4::fromTranslation(0,-1,0), Matrix4::fromTranslation(-2,0,0)};

    // POUR sphere AND sphere AND sphere
    //_leaf={Matrix4::fromTranslation(0.5,0,0),Matrix4::fromTranslation(0,-0.5,0), Matrix4::fromTranslation(-0.5,0,0)};

    // POUR (sphere U sphere) - sphere
    //_leaf={Matrix4::fromTranslation(0.5,0,0), Matrix4::fromTranslation(-0.5,0,0), Matrix4::fromTranslation(0,-0.5,0) };

    // POUR sphere U cube U cone U cylindre
    //_leaf={Matrix4::fromTranslation(-0.5,-0.5,0), Matrix4::fromTranslation(0.5,-0.5,0), Matrix4::fromTranslation(-0.5,0.5,0), Matrix4::fromTranslation(0.5,0.5,0) };

    //_leaf={Matrix4::fromTranslation(2,0,0),Matrix4::fromTranslation(0,0,0)};

    // Pour le CSG
    _leaf = {
        Matrix4::fromTranslation(0,0,0).scale(2,2,2),
        Matrix4::fromTranslation(0,0,0).scale(1.5,1.5,1.5),
        Matrix4::fromTranslation(0,0,0).scale(1, 2, 1),
        Matrix4::fromTranslation(0,0,0).scale(2, 1, 1).rotate(90, 0, 0, 1),
        Matrix4::fromTranslation(0,0,0).scale(1, 1, 2).rotate(90, 1, 0, 0)
    };

    //    };

    // avec un scale
    //    _leaf={
    //        Matrix4::fromScale(0.5,0.5,0.5).translate(2,0,0),
    //        Matrix4::fromTranslation(0,-1,0),
    //        Matrix4::fromScale(1.5,1.5,1.5).translate(-2,0,0)
    //    };


    // Couleur de trois sphères en union
    //_color={Vector4(1,0,0,1),Vector4(1,1,0,1), Vector4(1,0.5,1,1)};

    //    _color={Vector4(1,0,0,1),Vector4(1,1,0,1), Vector4(1,0.5,1,1), Vector4(0.5,1,0.5,1)};

    // deux couleurs pour une union
    _color={
        Vector4(0,0,1,1),
        Vector4(1,0,0,1),
        Vector4(0,1,0,1),
        Vector4(0,1,0,1),
        Vector4(0,1,0,1)
    };

    /** ** **/
}


void GLApplication::update() {
    updateCamera();

    _nbNode=_tree.size()/2;
    _nbLeaf=0;

    // animations
    //_leaf[0].translate(-0.01,0,0); // bouge en x
    //    _leaf[1].scale(0.1, 0.1, 0.1); // grossit
    //    _leaf[2].scale(0.1, 1, 1);
    //    _leaf[2].rotate(1, 1, 0, 0);

    for(int i=0;i<_nbNode;++i) {
        if (_tree[2*i]>=4) _nbLeaf++;
    }


}



static int nbFrame=0;
static double nextClock=1;
QElapsedTimer qtime;

void GLApplication::draw() {
    if (nbFrame==0) {
        p3d::startTime();
        qtime.start();
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// | GL_STENCIL_BUFFER_BIT);


    /** compute leaf->eye **/
    vector<Matrix4> leafInv;
    for(auto &m:_leaf) {
        leafInv.push_back(m.inverse()*_camera.worldCamera()*Matrix4::fromTranslation(0,0,-2));
    }
    /** ** **/

    /** passing uniforms to the shader **/
    _sphereTrace.use();

    _sphereTrace.uniform("nbLeaf",_nbLeaf);
    _sphereTrace.uniform("nbNode",_nbNode);
    _sphereTrace.uniform("tree",_tree);
    _sphereTrace.uniform("leafEyeMatrix",leafInv);
    _sphereTrace.uniform("colorLeaf",_color);

    /** ** **/

    /** draw **/
    glBindVertexArray(_squareVao);
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
    /** ** **/

    /** FPS **/
    nbFrame++;
    if (qtime.elapsed()/1000.0>nextClock) {
        nextClock++;
        double fps= nbFrame/p3d::elapsedStartTime();
        cout << "nbFrame = " << nbFrame << " " << p3d::elapsedStartTime()/nbFrame << " or " << double(nbFrame)/p3d::elapsedStartTime() << endl;
        if (fps<2.0) exit(1);
    }
    /** ** **/

}

