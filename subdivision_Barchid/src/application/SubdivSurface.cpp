#include "SubdivSurface.h"
#include "GLTool.h"

using namespace p3d;
using namespace std;

SubdivSurface::~SubdivSurface() {

}


SubdivSurface::SubdivSurface() {

}


void SubdivSurface::input(p3d::Mesh *m) {
    _input=m;
}

void SubdivSurface::source(p3d::Mesh *m) {
    _source=m;
}


void SubdivSurface::source(const std::string &filename) {
    delete _source;
    _source=new Mesh();
    _source->readInit(filename,false);
    _source->requestInitDraw();
}


/** returns the edge index of [v1,v2] (-1 if not found)
 *
 * @pre : vector _edgeForVertex must be intialized before
 */
int SubdivSurface::findEdge(int v1,int v2) {
    ///
    vector<int> &edgeV1=_edgeOfVertex[v1];
    for(unsigned int i=0;i<edgeV1.size();++i) {
        int b=_edge[edgeV1[i]]._b;
        if (b==v2) return edgeV1[i];
    }
    vector<int> &edgeV2=_edgeOfVertex[v2];
    for(unsigned int i=0;i<edgeV2.size();++i) {
        int b=_edge[edgeV2[i]]._b;
        if (b==v1) return edgeV2[i];
    }
    return -1;
}



void SubdivSurface::prepare() {
    if (!_input) return;
    /// compute edge :
    ///
    int v1,v2;
    _edgeOfVertex.clear();
    _edge.clear();
    _edgeOfVertex.resize(_input->nbPosition());
    for(unsigned int i=0;i<_input->nbFace();++i) { // i is the face (index)
        v1=_input->indexPositionVertexFace(i,-1); // the vertex before the first one in face i (i.e. the last one)
        for(unsigned int j=0;j<_input->nbVertexFace(i);++j) { // j is the j-th vertex in the face i
            v2=_input->indexPositionVertexFace(i,j);
            int foundEdge=findEdge(v1,v2);
            if (foundEdge!=-1) { // the edge already exists
                _edge[foundEdge]._right=i; // if already exists, the _left face is already set
            }
            else { // create the edge [v1,v2]
                Edge e;
                e._a=v1;
                e._b=v2;
                e._left=i; // the current face is put on _left (preserve the orientation of the mesh)
                e._right=-1; // will be set if the same edge is encountered after
                _edge.push_back(e);
                _edgeOfVertex[v1].push_back(_edge.size()-1);
                _edgeOfVertex[v2].push_back(_edge.size()-1);
            }
            v1=v2; // next
        }
    }
}



void SubdivSurface::computePointFace() {
    /* TODO : compute all point face (set the vector _pointFace : _pointFace[i] for the i-th face of _input).
   * - input = Mesh *_input
   * - _input->nbFace()
   * - _input->nbVertex(i) : number of vertices of i-th face
   * - _input->positionMesh(i,j) : the position of the j-th vertex of the i-th face
   */
    _pointFace.clear();

    // POUR CHAQUE [face]
    for(unsigned int i = 0; i < _input->nbFace(); i++) {
        Vector3 res = Vector3();
        // POUR CHAQUE [sommet] DE [la face]
        for(unsigned int j = 0; j < _input->nbVertexFace(i); j++) {
            Vector3 v = _input->positionVertexFace(i,j); // récupérer le jème sommet de la ième face
            res+=v;
        }

        res/= _input->nbVertexFace(i);
        _pointFace.push_back(res);
    }
}

void SubdivSurface::computePointEdge() {
    /* TODO : compute all point face (set the vector _pointEdge : _pointEdge[i] for the i-th edge).
   * input = Mesh *_input
   * _edge[i]._a, _edge[i]._b : give the index of the two positions of i-th edge (._a and ._b are indexes for _input)
   * _input->position(i) : the i-th position of input (as referred by ._a and ._b).
   * _edge[i]._left, _edge[i]._right : indexes of the two faces incident to i-th face
   * - _pointFace[i] : should give the point Face of the i-th face.
   */
    _pointEdge.clear();
    // POUR CHAQUE [arrête]
    for(unsigned int i = 0; i < _edge.size(); i++) {
        // Appliquer la formule du cours
        Vector3 v1 = _input->positionMesh(_edge[i]._a);
        Vector3 v2 = _input->positionMesh(_edge[i]._b);
        Vector3 f1 = _pointFace[_edge[i]._left];
        Vector3 f2 = _pointFace[_edge[i]._right];
        Vector3 e = (v1 + v2 + f1 + f2) / 4;

        _pointEdge.push_back(e);
    }
}


void SubdivSurface::computePointVertex() {
    /* TODO : compute all point vertex (set the vector _pointVertex : _pointVertex[i] for the i-th position).
   * input = Mesh *_input
   * - _edgeOfVertex[i][j] : gives the index (for the vector _edge) of the j-th edge of the i-th vertex
   */
    _pointVertex.clear();

    float nbSommets = _edgeOfVertex.size(); // nombre de sommets du mesh
    Vector3 V_ip1; // Sommet V_i+1 à trouver

    // POUR CHAQUE SOMMET [V_i
    for(int i = 0; i < nbSommets; i++) {
        Vector3 V_i = _input->positionMesh(i);

        // CALCULER la somme des points d'arrêtes précédemment calculés
        // CALCULER la somme des points faces précédemment calculés
        Vector3 sumE = Vector3();
        Vector3 sumF = Vector3();

        // POUR CHAQUE [arrête du sommet V_i]
        vector<int> edges = _edgeOfVertex[i];
        for (int j : edges) {
            Vector3 e_j = _pointEdge[j];
            Edge arrete = _edge[j];

            sumE += e_j; // AJOUTER le jème point edge du ième sommet

            // AJOUTER la face adjacente
            // SI [le sommet V_i est le sommet de départ de l'arrête]
            if(arrete._a == i) {
                // ALORS [le point face est celui de gauche]
                sumF += _pointFace[arrete._left];
            }
            // SINON, c'est le point face de droite
            else {
                sumF += _pointFace[arrete._right];
            }
        }
        int n = _edgeOfVertex[i].size(); // n est le nombre d'arrête adjacentes au sommet V_i
        // Appliquer la formule de la slide
        V_ip1 = (n-2.0)/n * V_i + (1.0/(n*n)) * sumE + (1.0/(n*n)) * sumF;
        _pointVertex.push_back(V_ip1);
    }
}

int SubdivSurface::findNextEdge(int i,int j) {

    return -1; // happens for a boundary edge
}

void SubdivSurface::buildMesh() {
    Mesh *m=new Mesh();
    /* TODO : build the new mesh
   * - m->addPositionMesh(aVector3) to add a vertex
   * - m->addFaceMesh({v1,v2,v3,...}) to add a face : caution : v1,v2,v3,... are indexes (int) of the positions of m
   * - caution with the indexes (indexes of m are not the same that the ones for _input : track them).
   *
   */

    // Dans le tableau des positions du mesh, on déclare les indices où se trouveront les points
    int startFace = 0; // on commence par ajouter les points face donc les positions des points face commencent en 0
    int startArete = _pointFace.size(); // après les points face, on ajoute les points arête
    int startSommet = _pointFace.size() + _pointEdge.size(); // en dernier, on ajoute les points sommet

    // AJOUTER les points face
    for(Vector3 ptFace : _pointFace) {
        m->addPositionMesh(ptFace);
    }

    // AJOUTER les points d'arête
    for(Vector3 ptArete : _pointEdge) {
        m->addPositionMesh(ptArete);
    }

    // AJOUTER les points sommets
    for(Vector3 ptSommet : _pointVertex) {
        m->addPositionMesh(ptSommet);
    }

    // CRÉER les faces du maillage
    int ip; // indice du point sommet considéré
    int ie1; // indice de l'arête incidente au point sommet considéré et adjacente à la face
    int If; // point de la face
    int ie2; // autre indice de l'arrête incidente au sommet considéré et adjacente à la face

    // POUR CHAQUE point sommet
    for(int i = 0; i < _pointVertex.size(); i++) {
        ip = startSommet + i; // on a déjà trouvé ip (ne pas oublier de bien se replacer dans les indices pour le maillage)

        // POUR CHAQUE [arête incidente au sommet] (ça marche parce qu'on est dans un modèle sans bord ici)
        for(int idxArete : _edgeOfVertex[i]) {
            Edge arete = _edge[idxArete];

            // l'arête courante est ie1
            ie1 = startArete + idxArete;

            // sélectionner le point face suivant la direction de l'arrête où se trouve le sommet original du point sommet considéré
            int faceAdjacente = arete._a ==i ? arete._left : arete._right; // face adjacente choisie
            If = startFace + faceAdjacente;

            // TROUVER l'autre arête incidente au point sommet considéré et adjacente à f (ie2)
            ie2 = 0;
            for(int indArete : _edgeOfVertex[i]) {
                if(idxArete == indArete) continue; // ne pas considérer la même arête que celle de ie1
                Edge autreArete = _edge[indArete];

                // SI [l'arête possède la même face adjacente que l'arête de ie1]
                if(autreArete._right == faceAdjacente || autreArete._left == faceAdjacente) {
                    ie2 = startArete + indArete; // autreArete est incidente au point sommet considéré et possède la même face adjacente ==> c'est l'arête de ie2
                    break; // on a trouvé donc on se barre de la boucle
                }
            }

            // ajouter la face avec les indices trouvés
            m->addFaceMesh({ip, ie1, If, ie2});
        }
    }


    /* end TODO */


    _result=m;
    _result->computeNormal();
    _result->computeTexCoord();
}


void SubdivSurface::catmullClarkIter() {
    prepare();
    computePointFace();
    computePointEdge();
    computePointVertex();
    buildMesh();
}

void SubdivSurface::catmullClark() {
    delete _result;
    _pointVertex.clear();
    _pointEdge.clear();
    _pointFace.clear();
    _result=_source->clone();
    for(int i=0;i<_nbIteration;++i) {
        _input=_result;
        catmullClarkIter();
        delete _input;
    }
}




void SubdivSurface::drawTest() {
    glPointSize(10);


    p3d::ambientColor=Vector4(1,0,0,1);
    p3d::shaderVertexAmbient();
    if (_pointVertex.size()>0) p3d::drawPoints(_pointVertex);


    p3d::ambientColor=Vector4(0,1,0,1);
    p3d::shaderVertexAmbient();
    if (_pointEdge.size()>0) p3d::drawPoints(_pointEdge);


    p3d::ambientColor=Vector4(0,0,1,1);
    p3d::shaderVertexAmbient();
    if (_pointFace.size()>0) p3d::drawPoints(_pointFace);
}





