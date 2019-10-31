#include "SubdivCurve.h"
#include <cmath>
#include <iostream>

#include "Vector3.h"
#include "Matrix4.h"

using namespace std;
using namespace p3d;

SubdivCurve::~SubdivCurve() {
}

SubdivCurve::SubdivCurve() {
    _nbIteration=1;
    _source.clear();
    _result.clear();

}


void SubdivCurve::addPoint(const p3d::Vector3 &p) {
    _source.push_back(p);
}

void SubdivCurve::point(int i,const p3d::Vector3 &p) {
    _source[i]=p;
}


void SubdivCurve::chaikinIter(const vector<Vector3> &p) {
    /* TODO : one iteration of Chaikin : input = p, output = you must set the vector _result (vector of Vector3)
   */
    _result.clear();
    unsigned int i ;
    // POUR CHAQUE POINT Pi (sauf le dernier vu qu'on aura besoin de Pi+1)
    for(i= 0; i<p.size()-1;i++) {
        // Appliquer la formule pour Q_2i
        Vector3 Q2i = 0.75 * p[i] + 0.25 * p[i+1];

        // Appliquer la formule pour Q_2i+1
        Vector3 Q2ip1 = 0.25 * p[i] + 0.75 * p[i+1];

        _result.push_back(Q2i);
        _result.push_back(Q2ip1);
    }

    // SI [on a une ligne fermée]
    if(isClosed()) {
        i = p.size() - 1;
        // Appliquer la formule sauf que le dernier point utilise le point P0 en tant que i+1
        Vector3 Q2i = 0.75 * p[i] + 0.25 * p[0];

        // Appliquer la formule pour Q_2i+1
        Vector3 Q2ip1 = 0.25 * p[i] + 0.75 * p[0];

        _result.push_back(Q2i);
        _result.push_back(Q2ip1);
    }
}

void SubdivCurve::dynLevinIter(const vector<Vector3> &p) {
    /* TODO : one iteration of DynLevin : input = p, output = you must set the vector _result (vector of Vector3)
   */
    _result.clear();
    unsigned int i;
    Vector3 Q2i, Q2ip1;

    // Gérer le cas fermé avec i=0
    if(isClosed()) {
        // CAS pour i=0
        i = 0;
        Q2i = p[i];
        Q2ip1 = -0.0625 * (p[i+2] + p[p.size()-1]) + 0.5625 * (p[i+1] + p[i]); // i-1 devient n
        _result.push_back(Q2i);
        _result.push_back(Q2ip1);
    }

    // Appliquer la formule pour le cas ouvert (on s'occupe donc des points de i = 1 à i = n-2
    for(i = 1; i < p.size() - 2; i ++) {
        // Appliquer la formule pour Q_2i
        Q2i = p[i];

        // Appliquer la formule pour Q_2i+1
        Q2ip1 = -0.0625 * (p[i+2] + p[i-1]) + 0.5625 * (p[i+1] + p[i]);

        _result.push_back(Q2i);
        _result.push_back(Q2ip1);
    }

    // Gérer les cas où c'est fermé
    if(isClosed()) {
        // CAS pour i=n-2
        i = p.size() - 2;
        Q2i = p[i];
        Q2ip1 = -0.0625 * (p[0] + p[i-1]) + 0.5625 * (p[i+1] + p[i]); // i+2 devient 0
        _result.push_back(Q2i);
        _result.push_back(Q2ip1);

        // CAS pour i=n-1
        i = p.size() - 1;
        Q2i = p[i];
        Q2ip1 = -0.0625 * (p[1] + p[i-1]) + 0.5625 * (p[0] + p[i]); // i+2 devient 1 et i+1 devient 0
        _result.push_back(Q2i);
        _result.push_back(Q2ip1);
    }
}


void SubdivCurve::chaikin() {
    if (_source.size()<2) return;
    vector<Vector3> current;
    _result=_source;
    for(int i=0;i<_nbIteration;++i) {
        current=_result;
        chaikinIter(current);
    }
}

void SubdivCurve::dynLevin() {
    if (_source.size()<2) return;
    if (!isClosed()) return;
    vector<Vector3> current;
    _result=_source;
    for(int i=0;i<_nbIteration;++i) {
        current=_result;
        dynLevinIter(current);
    }
}


