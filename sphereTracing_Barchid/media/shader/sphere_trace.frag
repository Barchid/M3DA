#version 150

// @author F. Aubert (Univ. Lille)

const int iMax=150; // max loop
const int nbNodeMax=20; // max node

// uniforms
uniform int nbNode;
uniform int nbLeaf;
uniform int tree[nbNodeMax*2];
uniform mat4 leafEyeMatrix[nbNodeMax];
uniform vec4 colorLeaf[nbNodeMax];

// in out
out vec4 fragColor;
in vec2 fVertex;

// globals
float distanceNode[nbNodeMax];
bool isSet[nbNodeMax];
vec4 colorNode[nbNodeMax];

// Calcul des distances :
// https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm

// Distance d'un point P (dans le repère de la sphère) à cette sphère (de centre (0,0,0) et de rayon 1)
float dSphere(vec3 p) {
    // Calcul vu en cours
    vec3 c = vec3(0,0,0);
    float r = 1.0;
    return length(c-p) - r;
}

// Distance d'un point P (qui n'est PAS dans le repère du cube) à ce cube centré à l'origine de son repère en (0,0,0)
// et où ses coins sont situés sur (+-1,+-1,+-1) (donc arrête de longueur 2)
float dCube(vec3 p, int i) {
    vec4 P_eye = vec4(p, 1.0);
    // changer Peye en P_cube grâce à M_cube->eye
    mat4 M_cube_eye = leafEyeMatrix[i];
    vec4 P_cube = M_cube_eye * P_eye; // (on passe en coordonnées homogènes avec w=1

    // Calcul du cube (voir Iquilezles.org)
    vec3 q = abs(P_cube.xyz) - vec3(1,1,1);
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}


// distance from p to an union of primitives
float dList(vec3 p) {
    float distance = 0.;

    // Ex 4.3 : distance du point p à une sphère dont le centre est situé à l'origine quand elle est DANS SON REPÉRE À ELLE
    // ####################################################################################################################
    // au départ, p est exprimé dans le repère de la caméra (P_eye)
    //        vec4 P_eye = vec4(p, 1.0);

    //        // changer Peye en Psphere grâce à M_sphere->eye
    //        mat4 M_sphere_eye = leafEyeMatrix[0];
    //        vec4 P_sphere = M_sphere_eye * P_eye; // (on passe en coordonnées homogènes avec w=1

    //        // Récupérer distance à la sphère avec dSphere en utilisant ce P_eye
    //        distance = dSphere(P_sphere.xyz);

    // -- end Ex 4.3

    // Ex 4.5 : faire ça pour deux sphères
    vec4 P_eye = vec4(p, 1.0);

    // changer Peye en Psphere grâce à M_sphere->eye
    mat4 M1_sphere_eye = leafEyeMatrix[0];
    mat4 M2_sphere_eye = leafEyeMatrix[1];

    vec4 P1_sphere = M1_sphere_eye * P_eye; // (on passe en coordonnées homogènes avec w=1
    vec4 P2_sphere = M2_sphere_eye * P_eye; // (on passe en coordonnées homogènes avec w=1

    // Récupérer distance à la sphère avec dSphere en utilisant ce P_eye
    float d1 = dSphere(P1_sphere.xyz);
    float d2 = dSphere(P2_sphere.xyz);

    // la plus petite distance est celle choisie
    // (car, avec le Sphere tracing, on ne veut pas avancer sur le rayon plus loin que la première sphère qui touche quelque chose)
    distance = min(d1, d2);

    return distance;
}

// calcule dSphere en utilsant la matrice de changement de repère i
float dSphereAvecChangementDeRepere(vec3 p, int i) {
    vec4 P_eye = vec4(p, 1.0);

    // changer Peye en Psphere grâce à M_sphere->eye
    mat4 M_sphere_eye = leafEyeMatrix[i];
    vec4 P_sphere = M_sphere_eye * P_eye; // (on passe en coordonnées homogènes avec w=1

    // Récupérer distance à la sphère avec dSphere en utilisant ce P_eye
    return dSphere(P_sphere.xyz);
}

float dCone(vec3 p, int i)
{
    vec4 P_eye = vec4(p, 1.0);
    // changer Peye en P_cone grâce à M_cone->eye
    mat4 M_cone_eye = leafEyeMatrix[i];
    vec4 P_cone = M_cone_eye * P_eye; // (on passe en coordonnées homogènes avec w=1

    float theta = 0.785398163;

    float m1 = max(length(P_cone.xz) * cos(theta) - abs(P_cone.y) * sin(theta), P_cone.y - 1);
    return max(m1, -P_cone.y);
}

// Calcule la distance à un cylindre de hauteur 1 et de rayon r
float dCylinder(vec3 p, int i)
{
    vec4 P_eye = vec4(p, 1.0);
    // changer Peye en P_cylinder grâce à M_cylinder->eye
    mat4 M_cylinder_eye = leafEyeMatrix[i];
    vec4 P_cylinder = M_cylinder_eye * P_eye; // (on passe en coordonnées homogènes avec w=1

    float h = 1; // hauteur de 1
    float r = 1; // rayon de 1
    vec2 d = abs(vec2(length(P_cylinder.xz), P_cylinder.y)) - vec2(h,r);
    return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

// set distanceNode[indexNode] if indexNode is a primitive
void setDistanceLeaf(vec3 p,int indexNode,int primitive) {
    float distance = 0.0;

    // Trouver la primitive
    // SPHERE
    if(primitive == 0) {
        distance = dSphereAvecChangementDeRepere(p, indexNode);
    }
    // CUBE
    else if (primitive == 1) {
        distance = dCube(p, indexNode);
    }
    // CONE
    else if (primitive == 2) {
        distance = dCone(p, indexNode);
    }
    // Cylindre
    else if (primitive == 3) {
        distance = dCylinder(p, indexNode);
    }

    distanceNode[indexNode]= distance;
}


void updateParent(int indexNode) {
    int parent = tree[indexNode*2+1]; // indice du parent

    // SI [il y a déjà eu un premier enfant qui a touché au noeud]
    if(isSet[parent]) {
        // ALORS [j'effectue l'opération contenue dans le parent]
        float distanceEnfant2 = distanceNode[indexNode];
        float distanceEnfant1 = distanceNode[parent];

        // DÉCIDER DE l'opération à effectuer par le parent
        int operation = tree[parent*2];

        // RÉUNION (on prend le min entre les deux enfants)
        if(operation == 0) {
            distanceNode[parent] = min(distanceEnfant1, distanceEnfant2);
            colorNode[parent] = distanceEnfant2 < distanceEnfant1 ? colorNode[indexNode] : colorNode[parent]; // choisi de la couleur est le min entre les deux enfants
        }
        // INTERSECTION (on prend le max entre les deux enfants)
        else if(operation == 1) {
            distanceNode[parent] = max(distanceEnfant1, distanceEnfant2);
            colorNode[parent] = (distanceEnfant2 < distanceEnfant1) ? colorNode[indexNode] : colorNode[parent];
        }
        // DIFFERENCE
        else if(operation == 2) {
            distanceNode[parent] = max(distanceEnfant1, -distanceEnfant2);
            colorNode[parent] = distanceNode[parent] == -distanceEnfant2 ? colorNode[indexNode] : colorNode[parent];
        }
    }
    // SINON [je place juste la valeur de l'enfant dans le parent]
    else {
        float distanceEnfant1 = distanceNode[indexNode];
        distanceNode[parent]= distanceEnfant1;
        colorNode[parent] = colorNode[indexNode];
        isSet[parent] = true; // ne pas oublier de préciser que j'ai touché au parent (pour le deuxième enfant qui arrive)
    }
}

// Commentaires pour voir la compréhension du code de distanceTree
float distanceTree(vec3 p) {
    bool done=false;

    int operation;
    int parent;

    int i;

    // On marque qu'aucun noeud n'a été touché.
    // SI "isSet" d'un noeud est à true, alors ça veut dire qu'un de ses enfants a déjà touché au noeud
    for(i=0;i<nbNode;++i) {
        isSet[i]=false; // a child already updated the node i ?
    }

    // POUR CHAQUE [noeud i]
    for(i=0;i<nbNode-1;++i) {
        // On récupère l'opération (qui peut être une primitive ou une vraie opération sur deux primitives/opérations)
        operation=tree[i*2];
        parent=tree[i*2+1]; // on récupère le parent
        if (operation>=4) { // SI [on a une primitive] ALORS [on calcule la distance à la forme primitive]
            setDistanceLeaf(p,i,operation-4);
        }
        // mettre à jour le parent pour obtenir sa distance
        updateParent(i);
    }

    // Renvoyer la distance de la racine (car ce sera la distance finale à l'objet)
    return distanceNode[nbNode-1];
}

// calcul l'approximation de la dérivée partielle de d(p) en x avec la formule vue dans l'énoncé
float approxDeriveePartielleX(vec3 p, float eps) {
    return ( (distanceTree(vec3(p.x + eps, p.yz))) - (distanceTree(vec3(p.x-eps, p.yz))) ) / (2*eps);
}

// calcul l'approximation de la dérivée partielle de d(p) en y avec la formule vue dans l'énoncé
float approxDeriveePartielleY(vec3 p, float eps) {
    return ( (distanceTree(vec3(p.x, p.y+eps, p.z))) - (distanceTree(vec3(p.x, p.y-eps,p.z))) ) / (2*eps);
}

// calcul l'approximation de la dérivée partielle de d(p) en z avec la formule vue dans l'énoncé
float approxDeriveePartielleZ(vec3 p, float eps) {
    return ( (distanceTree(vec3(p.xy, p.z + eps))) - (distanceTree(vec3(p.xy, p.z-eps))) ) / (2*eps);
}

void main() {
    vec3 p = vec3(0,0,2); // ray origin --> caméra est à z=2

    // u est la direction qui va vers le point 2D du fragment sur l'écran z=0
    vec3 u = vec3(fVertex.x, fVertex.y, 0) - p; // ray direction
    u = normalize(u); // normaliser u

    fragColor = vec4(0.2,0.2,0.2,1); // default color of the pixel

    // init color of the nodes for leaves
    for(int i=0;i<nbLeaf;++i) {
        colorNode[i]=colorLeaf[i];
    }

    // init of the main loop for sphere tracing
    bool done=false;
    bool far=false;
    float d;
    int iter=iMax;


    // main loop of the sphere tracing
    while(!done) {
        // distance avec une sphère
        //d=dSphere(p);

        // distance avec une liste de sphères
        //        d=dList(p);

        // distance avec arbre CSG
        d = distanceTree(p);

        // Avancée sur le rayon de direction u
        p = p +  u * d;

        --iter;
        if (d<0.00001) done=true;
        else if (d>10.0) {done=true;far=true;}
        else if (iter==0) {far=true;done=true;}
    }

    // set the fragment color
    if (!far) {
        //        fragColor=vec4(1.0,0,0,1.0);
        //fragColor=vec4(1,0,0,1);


        float eps = 0.0001; // utiliser un léger epsilon (ça va servir pour l'approximation des dérivées partielles)
        vec3 L = vec3(0,0,1); // source directionnelle de lumière dans le modèle de Phong qu'on utilise

        // pour la calcul de normale n on a besoin des dérivées partielles en x, y et z
        float derivX = approxDeriveePartielleX(p, eps); // dérivée partiel en X de p
        float derivY = approxDeriveePartielleY(p, eps); // dérivée partiel en Y de p
        float derivZ = approxDeriveePartielleZ(p, eps); // dérivée partiel en Z de p

        // On obtient alors la normale n au point p
        vec3 n = vec3(derivX, derivY, derivZ);
        n = normalize(n); // ne pas oublier de normaliser n

        // appliquer l'intensité du diffus à la couleur (voir slides du modèle de Phong en M3DS)
        float angle_incidence = dot(L, n); // on obtient le cos de l'angle formé par L et n
        // SI [angle_incidence est négatif]
        if(angle_incidence < 0) {
            // Ca signifie qu'on est sur le côté de la face non éclairé donc on n'éclaire pas
            angle_incidence = 0;
        }

        // Mise à jour de la couleur diffuse
        fragColor = colorNode[nbNode - 1] * angle_incidence;
    }
}
