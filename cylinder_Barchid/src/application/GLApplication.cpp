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

enum EMenu {M_Draw_Square, M_Set_Section_Square, M_Set_Section_Circle,M_Draw_Section,M_Draw_Path,M_Spline_Line,M_Build_Extrusion,M_Build_Revolution};
enum EDraw {D_Square,D_Path,D_Section,D_Extrusion};
enum EPath {Path_Line,Path_Spline};
EMenu _activeMenu;
EDraw _activeDraw;
EPath _activePath;

GLApplication::GLApplication() {
    //
    _leftPanelMenu << "Draw square (drawing example)" << "Input section (= square)" << "Input section (= circle)" << "Input section" << "Input Path" << "Switch Linear/Spline Path";
    _leftPanelMenu  << "Draw extrusion" << "Draw revolution (Path = circle)";
    _activeMenu=M_Draw_Square;
    _activeDraw=D_Square;
    _activePath=Path_Line;

    pathDefault();
    sectionCircle();

    _cameraSection.ortho(-2.5,2.5,-2.5,2.5,0,2);
    _cameraPath.ortho(-2.5,2.5,-2.5,2.5,0,2);
    _cameraPath.position(0,1,0);
    _cameraPath.lookAt(Vector3(0,0,0));
    _cameraExtrusion.frustum(-_frustum,_frustum,-_frustum,_frustum,0.03,1000);
    _cameraExtrusion.position(0,0,10);
    _cameraExtrusion.lookAt(Vector3(0,0,0));

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
    _cameraPath.viewport(0,0,width,height);
    _cameraSection.viewport(0,0,width,height);
    _cameraExtrusion.viewport(0,0,width,height);
    // ...
}

void GLApplication::update() {
    // appelée toutes les 20ms (60Hz)
    // => mettre à jour les données de l'application
    // avant l'affichage de la prochaine image (animation)
    // ...

    if (_activeDraw==D_Path) {
        if (mouseLeftPressed()) {
            _path.push_back(_cameraPath.windowToWorld(mouseX(),mouseY()));
        }
        if (keyPressed(Qt::Key_X)) _path.clear();
    }
    if (_activeDraw==D_Section) {
        if (mouseLeft()) {
            _section.push_back(_cameraSection.windowToWorld(mouseX(),mouseY()).xy());
        }
        if (keyPressed(Qt::Key_X)) _section.clear();
    }
    if (_activeDraw==D_Extrusion) {
        updateCameraExtrusion();
    }
}

/* ************************************************************ */

void GLApplication::updateCameraExtrusion() {
    if (mouseLeft()) {
        Vector3 center=_cameraExtrusion.pointTo(Coordinate_Local,Vector3(0,0,0));
        Vector3 vertical=Vector3(0,1,0);
        _cameraExtrusion.translate(center,Coordinate_Local);
        _cameraExtrusion.rotate(-deltaMouseX()/2.0,vertical,Coordinate_Local);
        _cameraExtrusion.rotate(deltaMouseY()/2.0,Vector3(1,0,0),Coordinate_Local);
        _cameraExtrusion.translate(-center,Coordinate_Local);
    }
    if (left()) _cameraExtrusion.left(0.3);
    if (right()) _cameraExtrusion.right(0.3);
    if (forward()) _cameraExtrusion.forward(0.3);
    if (backward()) _cameraExtrusion.backward(0.3);
    if (accelerateWheel()) {
        _frustum*=1.05;
        _cameraExtrusion.frustum(-_frustum,_frustum,-_frustum,_frustum,0.03,1000);
    }
    if (decelerateWheel()) {
        _frustum/=1.05;
        _cameraExtrusion.frustum(-_frustum,_frustum,-_frustum,_frustum,0.03,1000);
    }
}


void GLApplication::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch(_activeDraw) {
    case D_Square: p3d::apply(_cameraSection);drawSquare();break;
    case D_Section: p3d::apply(_cameraSection);drawSection();break;
    case D_Extrusion: p3d::apply(_cameraExtrusion);drawExtrusion();break;
    case D_Path:
        apply(_cameraPath);    if (_activePath==Path_Line) {
            drawPathLine();
        }
        else if (_activePath==Path_Spline) {
            drawPathSpline();
        }
    default:break;
    }
}


/** ************************************************************************ **/

/**
 * @brief GLApplication::drawSquare
 * Example of drawing primitives : p3d::drawPoints and p3d::drawLineStrip and p3d:draw for text. Notice shader activation before the p3d::draw... calls
 */
void GLApplication::drawSquare() {



    // compute points of the square in array of Vector2
    vector<Vector2> pts;
    pts.resize(5);
    pts[0]=Vector2(-0.5,-0.5);
    pts[1]=Vector2(0.5,-0.5);
    pts[2]=Vector2(0.5,0.5);
    pts[3]=Vector2(-0.5,0.5);
    pts[4]=pts[0];

    // draw square (line strip)
    p3d::ambientColor=Vector4(1,0,0,1); // the drawing color for the shader : must be set before the call p3d::shaderVertexAmbient
    p3d::shaderVertexAmbient(); // enable a simple shader.
    p3d::drawLineStrip(pts,5); // draw the array of Vector2D with the actual shader (you must set the number of points to draw : 5 here for the square).

    // draw vertices (points)
    glPointSize(10);
    p3d::ambientColor=Vector4(0,0,1,1);
    p3d::shaderVertexAmbient();
    p3d::drawPoints(pts);

    // draw text
    p3d::ambientColor=Vector4(1,0,1,1);
    p3d::draw("V0",Vector3(pts[0],0));


}


/** ************************************************************************ **/

/**
 * @brief GLApplication::sectionSquare
 * Example of cross section setup (here a square : notice the repeat of the first vertex to close the cross section).
 */

void GLApplication::sectionSquare() {
    _section.clear();
    _section.push_back(Vector2(-1,-1));
    _section.push_back(Vector2(1,-1));
    _section.push_back(Vector2(1,1));
    _section.push_back(Vector2(-1,1));
    _section.push_back(Vector2(-1,-1));
}


/**
 * @brief GLApplication::crossSectionCircle
 * set the cross section (i.e. set _inputCrossSection) as a circle
 */
void GLApplication::sectionCircle() {
    _section.clear();
    // En utilisant la théorie ici https://www.mathopenref.com/coordparamcircle.html
    float nbPoints = 25; // nombre de points voulus pour tracer le cercle
    float x,y; // coordonnées des points à trouver
    float t = 0; // angle du point à trouver par rapport au centre du cercle

    for(int i = 0; i < nbPoints; i++) {
        // application de la formule
        x = cos(t);
        y = sin(t);
        _section.push_back(Vector2(x,y));

        // mise à jour de l'angle pour le prochain point
        t += (2.0*M_PI) /(nbPoints-1.0);
    }
    //_section.push_back(Vector2(x,y));
}


/** ************************************************************************ **/

void GLApplication::pathDefault() {
    // Q7 : commenter et mettre l'autre segment pour que ce soit non orthogonal au plan XY
    //    _path.clear();
    //    _path.push_back(Vector3(0,0,-2));
    //    _path.push_back(Vector3(0,0,2));
    _path.clear();
    _path.push_back(Vector3(-2,0,-2));
    _path.push_back(Vector3(2,0,2));

    // Q8 : remplacer le pathDefault par ça
    _path.clear();
    _path.push_back(Vector3(-2,0,-2));
    _path.push_back(Vector3(0,0,2));
    _path.push_back(Vector3(2,0,-1));
}

void GLApplication::pathCircle() {
    _path.clear();
    // En utilisant la théorie ici https://www.mathopenref.com/coordparamcircle.html
    float nbPoints = 25; // nombre de points voulus pour tracer le cercle
    float x,z; // coordonnées des points à trouver (on place le cercle en profondeur d'où z)
    float t = -1; // angle du point à trouver par rapport au centre du cercle

    for(t = -1; t <= 2*M_PI;t+=(2*M_PI)/nbPoints) {
        // application de la formule
        x = cos(t);
        z = sin(t);

        // ajout dans path
        _path.push_back(Vector3(x,0,z));
    }

}

/** ************************************************************************ **/

void GLApplication::drawSection() {
    p3d::ambientColor=Vector4(1,0,0,1);
    p3d::shaderVertexAmbient();
    p3d::drawPoints(_section);
    p3d::drawLineStrip(_section);
}

void GLApplication::drawPathLine() {
    if (_path.size()<1) return;

    p3d::ambientColor=Vector4(0,0,1,1);
    p3d::shaderVertexAmbient();
    p3d::drawPoints(_path);
    p3d::drawLineStrip(_path);
}


void GLApplication::drawExtrusion() {
    if (_extrusion.size()<4) return;
    int nbSlice=_section.size();

    p3d::ambientColor=Vector4(1,0,0,1);
    p3d::shaderVertexAmbient();


    //drawGrid(_extrusion,nbSlice); // comment this once last question done


    //  uncomment once normals computed (last question)
    p3d::lightPosition[0]=Vector4(0,0,10,1);
    p3d::lightIntensity[0]=1.0;
    p3d::material(Vector4(0,0,0.3,1),Vector3(0,0.2,0.8),Vector3(0,0.8,0.3),100);
    p3d::diffuseBackColor=Vector3(0.8,0,0);
    p3d::shaderLightPhong();
    fillGrid(_extrusion,_normalExtrusion,nbSlice);



    drawPath();

}



void GLApplication::drawPathSpline() {
    if (_path.size()>=2) {
        vector<Vector3> toDraw;
        toDraw.clear();
        int nbPts=100;
        double step=1.0/(nbPts-1);
        double t=0;
        for(int i=0;i<nbPts;++i) {
            toDraw.push_back(pointSpline(t));
            t+=step;
        }

        p3d::ambientColor=Vector4(0,0,1,1);
        p3d::shaderVertexAmbient();
        drawLineStrip(toDraw,toDraw.size());
    }
    if (_path.size()>0) {
        p3d::ambientColor=Vector4(0,0,1,1);
        p3d::shaderVertexAmbient();
        p3d::drawPoints(_path);
    }
}

/** ************************************************************************* **/

/**
 * @brief transform the point p in the plane (x,y,0) to the point in the plane with the normal n (i.e. rotation of the plane (x,y,0))
 * @param p : the point expressed in the plane (x,y,0)
 * @param n : the normal of the plane
 * @return the transformation of the point p
 */

Vector3 GLApplication::rotatePlane(const Vector3 &p,const Vector3 &n) {
    Vector3 result;
    Matrix4 matrix; // Matrice homogène qui va transformer le P du plan (x,y) au point dans le plan de la direction orthogonale de la section
    matrix.setRotation(Vector3(0,0,1), n); // seul axe des Z spécifié

    // transformation du point par la matrice homogène pour chgmt repère
    result = matrix.transformPoint(p);
    return result;
}



Vector3 GLApplication::pointSpline(double tNormalized) {
    // Calculer le point de la courbe C(t) du path
    Vector3 Ht; // point H(t) de la courbe
    unsigned int n = _path.size();
    // Trouver le point du path correspondant au t en paramètre (en d'autres termes, il faut trouver le bon i pour _path[i]
    // on sait que t = i * 1/(n-1)
    // on adapte et on obtient que i = t / (1/(n-1)) = t * (n-1)
    unsigned int i = tNormalized * (n-1);
    double t = tNormalized * (n-1) - i;

    // Définir P0, P1 et T0, T1
    Vector3 P0 = _path[i];
    Vector3 P1 = _path[i+1];
    Vector3 T0 = tangentPathLine(i);
    //    T0 = T0.normalize();
    Vector3 T1 = tangentPathLine(i+1);
    //    T1 = T1.normalize();

    // Calculer H(t) = t³(2P0−2P1+T0+T1)+t²*(−3P0+3P1−2T0−T1)+tT0+P0
    Ht = pow(t, 3) * (2*P0-2*P1+T0+T1) + pow(t, 2) * (-3*P0+3*P1-2*T0-T1) + t * T0 + P0;
    return Ht;
}


Vector3 GLApplication::tangentPathSpline(double tNormalized) {
    Vector3 result; // expression de la derivee en t P'(t) pour obtenir la tangente en P(t)
    // Comme pour pointSpline, il faut trouver les P0, P1, T0 et T1
    // Donc, on copie-colle le code pour retrouver ces paramètres :

    unsigned int n = _path.size();
    // Trouver le point du path correspondant au t en paramètre (en d'autres termes, il faut trouver le bon i pour _path[i]
    // on sait que t = i * 1/(n-1)
    // on adapte et on obtient que i = t / (1/(n-1)) = t * (n-1)
    unsigned int i = tNormalized * (n-1);
    double t = tNormalized * (n-1) - i;

    // Définir P0, P1 et T0, T1
    Vector3 P0 = _path[i];
    Vector3 P1 = _path[i+1];
    Vector3 T0 = tangentPathLine(i);
    Vector3 T1 = tangentPathLine(i+1);

    // Appliquer formule dérivée d'une hermite
    // H'(t) = 3t²(2P0−2P1+T0+T1) + 2t*(−3P0+3P1−2T0−T1) + T0
    result = 3*pow(t, 2)*(2*P0-2*P1+T0+T1) + 2*t*(-3*P0+3*P1-2*T0-T1) + T0;
    return result;
}



Vector3 GLApplication::tangentPathLine(unsigned int i) {
    Vector3 directionOrthogonale; // la direction orthogonale à trouver

    // On cherche la direction orthogonale en chaque point courant du path situé à _path[i]

    // SI [point courant du path est le premier point]
    if(i == 0) {
        // la direction orthogonale est la direction du premier segment
        directionOrthogonale = _path[1] - _path[0];
    }
    // SINON SI [point courant du path est le dernier point du path]
    else if (i == _path.size() -1) {
        // la direction orthogonale est la direction du dernier segment
        directionOrthogonale = _path[_path.size() - 1] - _path[_path.size() - 2];
    }
    // SINON (le point courant du path est au milieu)
    else {
        // la direction orthogonale est le vecteur allant de i-1 à i+1
        directionOrthogonale = _path[i+1] - _path[i-1];
    }
    // La tangente est très grande du coup la spline essaye de rattraper le coup
    // Donc on la réduit en multipliant par 0.4 pour que ça donne un rendu convaincant (c'est une constante empirique)
    directionOrthogonale *= 0.4;
    return directionOrthogonale;
}

/** ************************************************************************* **/

void GLApplication::normalSection() {
    _normalSection.clear();

    // CALCULER les normales de chaque segments de la section
    std::vector<Vector2> normales;
    for(unsigned int i=0; i<_section.size()-1; i++){ // note : ne pas prendre le dernier point dans le for() parce qu'il n'y a pas de segment après
        // Segment courant
        Vector2 segment = Vector2(_section[i], _section[i+1]);

        // Calculer la normale de ce segment (vecteur perpendiculaire à vecteur (x,y) est le vecteur (-y,x)
        Vector2 normale = Vector2(-segment.y(), segment.x());
        normales.push_back(normale);
    }

    // CALCULER les moyennes des normales pour chaque point de la section
    Vector2 moyenne = Vector2(normales[0]);  // ATTENTION la normale pour le premier point de la section est la normale du premier point telle quelle sans moyenne
    moyenne.normalize(); // normaliser sinon l'éclairage de Phong ne fonctionne pas
    _normalSection.push_back(normales[0]);

    for(unsigned int i=0; i<_section.size()-1; i++){
        // Calcul de la moyenne des normales pour le point de la section
        moyenne = (normales[i]+normales[i+1])/2;
        moyenne.normalize(); // normaliser pour l'éclairage
        _normalSection.push_back(moyenne);
    }
}


void GLApplication::extrudeLine() {
    if (_path.size()<1 || _section.size()<1) return;

    _extrusion.clear();
    _normalExtrusion.clear(); // for lighting (last question)

    normalSection(); // construire les normales

    float x; // coordonnées X de section
    float y; // coordonnées Y de section
    float z; // coordonnées Z de path
    Vector3 normale; // normale qui sera utilisée pour l'extrusion

    for(unsigned int i = 0; i < _path.size(); i++) {
        Vector3 ptPath = _path[i]; // point du path courant

        // Commenter pour rotatePlane
        //z = ptPath.z(); // Q6

        // Direction orthogonale du point courant du path (_path[i])
        Vector3 n = tangentPathLine(i);
        unsigned int j = 0;
        for(Vector2 ptSection : _section) {
            x = ptSection.x();
            y = ptSection.y();

            //Vector3 ptExtrusion = Vector3(x,y,z); // ancien ajout du point d'extrusion (avant Q7)

            // Point p, situé dans le plan (x,y)
            Vector3 p = Vector3(x, y, 0);

            // Segment du path (Q7 car on n'a qu'un seul segment dans _path)
            // Vector3 n = _path[1] - _path[0];

            // Obtenir point p après changement de repère
            Vector3 extru = rotatePlane(p, n);
            _extrusion.push_back(ptPath + extru);

            // Ajout de la normale pour le point d'extrusion ajouté
            normale = Vector3(_normalSection[j], 0);
            _normalExtrusion.push_back(normale);
            j++; // mise à jour de l'index
        }
    }
}

void GLApplication::extrudeSpline() {
    if (_path.size()<1 || _section.size()<1) return;

    _extrusion.clear();
    _normalExtrusion.clear(); // for lighting (last question)

    normalSection();

    unsigned int n = 100; // 100 points de la courbe
    double step=1.0/(n-1); // step qui permet de faire évoluer le t à chaque tour de boucle
    double t=0;
    Vector3 normale; // normale utilisée pour l'éclairement dans le point d'extrusion

    for(unsigned int i = 0; i < n ; i++) {
        Vector3 ptSpline = pointSpline(t); // Origine de la section passe de point du path à pointSpline
        Vector3 tanSpline = tangentPathSpline(t); // direction orthogonale devient tangentSpline

        unsigned j = 0;
        for(Vector2 ptSection : _section) {
            // Coordonnées x et y de la section
            float x = ptSection.x();
            float y = ptSection.y();

            Vector3 p = Vector3(x,y,0);
            Vector3 extru = rotatePlane(p, tanSpline);
            _extrusion.push_back(ptSpline + extru);

            // Ajout de la normale pour le point d'extrusion ajouté
            normale = Vector3(_section[j], 0);
            _normalExtrusion.push_back(normale);
            j++;
        }

        // Évolution de t pour la prochaine itération
        t+= step;
    }
}


/** ************************************************************************* **/


double GLApplication::scale(double tNormalized) {
    return 1.0;

}

void GLApplication::buildExtrusion() {
    if (_activePath==Path_Line)
        extrudeLine();
    else if (_activePath==Path_Spline) {
        extrudeSpline();
    }
}

void GLApplication::drawPath() {
    if (_activePath==Path_Line) {
        drawPathLine();
    }
    else if (_activePath==Path_Spline) {
        drawPathSpline();
    }
}


/** ********************************************************************** **/
/** i = button number, s = button text
 */
void GLApplication::leftPanel(int i,const std::string &) {
    _activeMenu=static_cast<EMenu>(i);

    switch (_activeMenu) {
    case M_Draw_Square:
        _activeDraw=D_Square;
        break;
    case M_Set_Section_Square:
        sectionSquare();
        _activeDraw=D_Section;
        break;
    case M_Set_Section_Circle:
        sectionCircle();
        _activeDraw=D_Section;
        break;
    case M_Build_Extrusion:
        buildExtrusion();
        _activeDraw=D_Extrusion;
        break;
    case M_Build_Revolution:
        pathCircle();
        buildExtrusion();
        _activeDraw=D_Extrusion;
        break;
    case M_Draw_Path:
        _activeDraw=D_Path;
        break;
    case M_Draw_Section:
        _activeDraw=D_Section;
        break;
    case M_Spline_Line:
        if (_activePath==Path_Line) _activePath=Path_Spline;
        else _activePath=Path_Line;
        if (_activeDraw==D_Extrusion) {
            buildExtrusion();
        }
        break;
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



