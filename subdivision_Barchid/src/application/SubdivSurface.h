#ifndef SUBDIVSURFACE_H
#define SUBDIVSURFACE_H

#include <vector>
#include "Vector3.h"
#include "Mesh.h"

class Edge {
public:
  int _a,_b,_left,_right;

  Edge() : _a(-1),_b(-1),_left(-1),_right(-1) {}
};




class SubdivSurface {
public:
  virtual ~SubdivSurface();
  SubdivSurface();


  void source(p3d::Mesh *m);
  void catmullClark();
  void drawTest();

  p3d::Mesh *source() {return _source;}
  p3d::Mesh *result() {return _result;}


  void source(const std::string &filename);

  void incNbIteration() {_nbIteration++;}
  void decNbIteration() {_nbIteration--;}


private:
  int findEdge(int v1, int v2);
  void prepare();
  void input(p3d::Mesh *m);
  void catmullClarkIter();
  void computePointFace();
  void computePointEdge();
  void computePointVertex();
  int findNextEdge(int i, int j);
  void buildMesh();


  std::vector<std::vector<int> > _edgeOfVertex;
  std::vector<Edge> _edge;

  std::vector<p3d::Vector3> _pointFace;
  std::vector<p3d::Vector3> _pointEdge;
  std::vector<p3d::Vector3> _pointVertex;

  p3d::Mesh *_source=NULL;
  p3d::Mesh *_input=NULL;
  p3d::Mesh *_result=NULL;

  int _nbIteration=1;
};

#endif // SUBDIVSURFACE_H

