#include "Nurbs.h"
#include <cmath>
#include <iostream>

#include "Vector3.h"
#include "Matrix4.h"

using namespace std;
using namespace p3d;

Nurbs::~Nurbs() {
}

Nurbs::Nurbs() {
    _control.clear();
    _nbControl[D_U]=0;
    _nbControl[D_V]=0;
    _knot[D_U].clear();
    _knot[D_V].clear();
    _degree[D_U]=0;
    _degree[D_V]=0;

}

double Nurbs::startInterval(EDirection direction) {
    return _knot[direction][_degree[direction]];
}

double Nurbs::endInterval(EDirection direction) {
    return _knot[direction][_nbControl[direction]]-0.00001;
}


bool Nurbs::inInterval(EDirection direction, double u) {
    return (u>=startInterval(direction) && u<=endInterval(direction));
}


void Nurbs::knotUniform(EDirection direction,int nb) {
    _knot[direction].resize(nb);
    // On veut nb noeuds répartis uniformément entre 0 et 1
    double step = 1.0/(nb-1);
    double u = 0; // valeur du noeud à placer
    for(int i = 0; i < nb; i++) {
        _knot[direction][i] = u;
        u += step; // mise à jour pour la valeur du noeud suivant
    }
}


/** Eval the basis function Nkp(t) for the knot vector knot **/
// NOTE IMPORTANTE : j'ai renommé "knot" en U pour faciliter ma compréhension à la relecture pour les exams
double Nurbs::evalNkp(int k,int p,double u,const std::vector<double> &U) {
    double Nkp=0.0;

    /* TODO : compute Nkp(u)
   * - knot[i] : the knot i
   * - p : degree
   * - k : indice of the basis function.
   */
    // SI [p = 0] : alors on applique la formule marquée (2) dans l'énoncé
    if(p == 0) {
        Nkp = u >= U[k] && u < U[k+1] ? 1.0 : 0.0; // on a calulé Nk0
    }
    else {
        // On applique la formule (3) de l'énoncé
        // SI [ le dénominateur du premier terme de l'addition dans la formule est non nul] (les noeuds U_k+p et U_k sont confondus)
        if((U[k+p] - U[k]) != 0.) {
            // ALORS [ j'applique le premier terme de la formule]
            double Nk_pmoins1 = evalNkp(k, p-1, u, U); // récursion
            Nkp += ( (u-U[k]) / (U[k+p]-U[k]) )   *   Nk_pmoins1;
        }

        // SI [ le dénominateur du deuxième terme de l'addition dans la formule est non nul] (les noeuds U_k+p+1 et U_k+1 sont confondus)
        if((U[k+p+1] - U[k+1]) != 0.) {
            // ALORS [ j'applique le deuxième terme de la formule]
            double N_kplus1_pmoins1 = evalNkp(k+1, p-1, u, U); // récursion
            Nkp += ( (U[p+k+1] - u) / (U[k+p+1] - U[k+1]) )   *   N_kplus1_pmoins1;
        }
    }


    return Nkp;
}


double Nurbs::evalNkp(EDirection direction,int k,int p,double t) {
    return evalNkp(k,p,t,_knot[direction]);
}


void Nurbs::clearControl() {
    _nbControl[D_U]=0;
    _nbControl[D_V]=0;
    _control.clear();
}

void Nurbs::initControlGrid() {
    _nbControl[D_U]=5;
    _nbControl[D_V]=4;
    _control.clear();
    double u=-1;
    double v=-1;
    double stepU=2.0/(_nbControl[D_U]-1);
    double stepV=2.0/(_nbControl[D_V]-1);
    for(int i=0;i<_nbControl[D_V];++i) {
        u=-1;
        for(int j=0;j<_nbControl[D_U];++j) {
            _control.push_back(Vector4(u,v,double(rand())/RAND_MAX-0.5,1));
            u+=stepU;
        }
        v+=stepV;
    }
    knotRemap(D_U);
    knotRemap(D_V);
}


void Nurbs::addControlU(const Vector4 &p) {
    _control.push_back(p);
    _nbControl[D_U]++;
    knotRemap(D_U);
}


Vector3 Nurbs::pointCurve(double u) {
    Vector4 Pu(0,0,0,0); // Résultat Pu de la formule à calculer
    /* TODO :
 * - compute P(t) in result. Use the direction D_U only (curve)
 * - control(i) : control points
 * - nbControl(D_U) : number of control points
 * - evalNkp(k,p,u,_knot[D_U]) to eval basis function
 */
    // Appliquer la formule du cours
    int p = degree(D_U); // degree à la direction D_U
    int n = nbControl(D_U);
    for(unsigned int k = 0; k < n; k++) {
        // Récupérer le point de controle
        Vector4 Pk = _control[k];

        // Evaluer la valeur de fonction base Nkp(u)
        double Nkpu = evalNkp(D_U, k, p, u);
        Pu += Nkpu * Pk;
    }

    // renvoyer le vector3 de Pu (donc pas en coordonnées homogènes)
    return Vector3(Pu.x(),Pu.y(),Pu.z()) / Pu.w();
}


Vector3 Nurbs::pointSurface(double u,double v) {
    Vector4 Puv(0,0,0,0);
    /* TODO :
   * - compute P(u,v) in result.
   * - control(i,j) : control points (i= indice in direction D_U, j=indice in direction D_V)
   * - nbControl(D_U), nbControl(D_V) to know the number of control points in each direction.
   * - degree(D_U), degree(D_V) to get the degree in each direction.
   * - evalNkp(k,p,t,_knot[<D_U or D_V>]) to eval basis function in each direction
   */

    // DÉFINIR les variables comme dans la formule de l'énoncé
    int nu = nbControl(D_U);
    int nv = nbControl(D_V);
    int pu = degree(D_U);
    int pv = degree(D_V);

    // POUR CHAQUE [indice l dans la direction v]
    for(int l = 0; l < nv; l++) {

        // EVALUER P'_l grâce à la définition d'une courbe NURBS pour l'indice l donné
        Vector4 P_prime_l = Vector4(0,0,0,0);
        // POUR CHAQUE [indice k de la direction D_U]
        for(int k=0; k < nu; k++) {
            Vector4 Pkl = control(k, l); // point de contrôle Pkl de la formule (5) de l'énoncé
            double Nkpu = evalNkp(k, pu, u, _knot[D_U]); // evaluation N_kp(u)
            P_prime_l += Nkpu * Pkl;
        }

        // Avec P'_l, on peut compléter la formule notée (6) dans l'énoncé
        double Nlpv = evalNkp(l, pv, v, _knot[D_V]);
        Puv += Nlpv * P_prime_l;
    }

    return Puv.project(); // divide by w
}



void Nurbs::knotRemap(EDirection direction) {
    while (!checkNbKnot(direction)) {
        int nb=nbKnot(direction);
        _knot[direction].push_back(_knot[direction][nbKnot(direction)-1]);
        for(unsigned int i=nb-1;i>0;--i) {
            _knot[direction][i]=_knot[direction][i+1]-(_knot[direction][i]-_knot[direction][i-1])*(nb-1)/nb;
        }
    }
}


bool Nurbs::checkNbKnot(EDirection direction) {
    return (nbKnot(direction)>=nbControl(direction)+degree(direction)+1);
}


void Nurbs::knotOpenUniform(EDirection direction) {
    /* TODO : the first and the last knots have a degree+1 multiplicity
   *
   *
   *
   *
   */
    // NOTE : les noeuds de début et de fin qui ont une multiplicité = p+1, ça signifie qu'au début et à la fin du vecteur nodale, je dois avoir
    // p+1 noeuds de même valeur

    int n = nbControl(direction); // nombre de points de contrôle
    int p = degree(direction); // degré p de la courbe souhaitée
    int tailleVecteurNodale = n + 1 + p; // n+1+p noeuds dans le vecteur nodale
    _knot[direction].resize(tailleVecteurNodale);
    int k = 0; // indice utilisé pour ajouter dans le noeud U_k dans le vecteur nodale

    // AJOUTER les p+1 noeuds de début à 0 (pour avoir une multiplicité p+1
    for(int i = 0 ; i < p+1; i++) {
        _knot[direction][k] = 0.;
        k++;
    }

    // RÉPARTIR les noeuds du milieu entre 0 et 1
    double nbNoeudsRepartis = tailleVecteurNodale - 2*(p+1); // le nombre de noeus répartis entre 0 et 1 = le nombre total de noeuds - les noeuds de début et de fin confondus
    double step = 1.0/(nbNoeudsRepartis);
    double uk = step;
    for(int i = 0; i < nbNoeudsRepartis; i++) {
        _knot[direction][k] = uk;
        uk+= step;
        k++;
    }

    // AJOUTER les p+1 noeuds de fin à 1 (pour avoir une multiplicité p+1)
    for(int i = 0 ; i < p+1; i++) {
        _knot[direction][k] = 1.;
        k++;
    }
}


void Nurbs::knotBezier(EDirection direction) {

    /* TODO : define a bezier curve : degree = nbControl-1,
   *
   *
   *
   */
    int n = nbControl(direction); // n est le nombre de points de contrôle
    int p = n - 1; // la courbe de bézier est une courbe de degré p = n-1
    degree(direction, p);
    int tailleVecteurNodale = n + 1 + p; // n+1+p noeuds dans le vecteur nodale
    _knot[direction].resize(tailleVecteurNodale);

    // courbe de bézier est une courbe dont la première moitié des noeuds sont uk = 0, et l'autre moitié est uk = 1
    double uk = 0.;
    for(int i = 0; i < tailleVecteurNodale; i++) {
        uk = i < tailleVecteurNodale/2 ? 0. : 1.; // première moitié uk = 0, deuxième moitié uk = 1
        _knot[direction][i] = uk;
    }
}

void Nurbs::setCircle() {
    /* Have to set : _control, _degree[D_U], _knot[D_U], _nbControl[D_U]
   *
   */
    _control.clear();

    // POINTS DE CONTRÔLE
    _nbControl[D_U] = 7; // n = 7 points de contrôles

    // les poids w_i des points de contrôle
    double w0, w1, w2, w3, w4, w5, w6;
    w1 = w3 = w5 = 0.5; // w1, w3 et w5 = cos(60°) = 0.5
    w0 = w2 = w4 = w6 = 1; // les autres poids à 1

    // placer les points de contrôle pour faire un triangle équilatéral
    Vector4 p0 = Vector4(0, 0, 0, w0);
    Vector4 p1 = Vector4(-1, 0, 0, w1);
    Vector4 p2 = Vector4(-1, 1, 0, w2);
    Vector4 p3 = Vector4(0, 1, 0, w3);
    Vector4 p4 = Vector4(1, 1, 0, w4);
    Vector4 p5 = Vector4(1, 0, 0, w5);
    Vector4 p6 = Vector4(0, 0, 0, w6);

    _control.push_back(p0);
    _control.push_back(p1);
    _control.push_back(p2);
    _control.push_back(p3);
    _control.push_back(p4);
    _control.push_back(p5);
    _control.push_back(p6);


    // DEGRÉ = 2
    degree(D_U, 2);

    // VECTEUR NODALE = [0,0,0,1,1,2,2,3,3,3]
    _knot[D_U] = {0., 0., 0., 1., 1., 2., 2., 3., 3., 3.};

    // normaliser les valeurs du vecteur nodale pour bien voir les courbes sur l'affichage à l'écran
    for(int i = 0; i < _knot[D_U].size(); i++) {
        _knot[D_U][i] /= 3; // la valeur maximale est 3 donc on normalise en divisant par 3
    }
}


void Nurbs::setRevolution(int nbV) {
    if (nbV<2) return;
    _nbControl[D_V]=nbV;
    _degree[D_V]=_degree[D_U];
    _control.resize(_nbControl[D_U]*_nbControl[D_V]);
    knotRemap(D_V);
    knotOpenUniform(D_V);

    double stepTheta=360.0/(nbV-1);
    double theta=stepTheta;
    Matrix4 rotate;
    for(int slice=nbControl(D_U);slice<nbControl(D_U)*nbControl(D_V);slice+=nbControl(D_U)) {
        rotate.setRotation(theta,0,1,0);
        for(int istack=0;istack<nbControl(D_U);++istack) {
            _control[slice+istack]=rotate*_control[istack];
        }
        theta+=stepTheta;
    }
}



