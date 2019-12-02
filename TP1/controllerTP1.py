#!/usr/bin/env python
# -*- coding: utf-8 -*-
import Sofa
from math import *
import numpy as np


class controller(Sofa.PythonScriptController):

    def initGraph(self, node):
        self.rootNode = node
        self.MechaObject = node.getObject('mObject')
        self.nbNodes = len(self.MechaObject.position)
        self.mesh = node.getObject('Mesh')
        self.k = 10
        self.m = 0.1

        ######################################################################
        # IMPORTANT : il faut calculer le tableau des l_0 pour chaque ressorts
        # (l_0 = longueur  initiale au repos du ressort)
        ######################################################################
        # tableau de position des noeuds:
        pos = self.MechaObject.position

        self.l_0 = []
        # POUR CHAQUE [ressort]
        for i in range(0, self.mesh.getNbEdges()):
            ressort = self.mesh.edges[i]
            # récupérer les position x_i et x_j des noeuds du ressort
            x_i = pos[ressort[0]]
            x_j = pos[ressort[1]]

            # longueur l_0 est la norme du vecteur noeud_i -> noeud_j
            self.l_0.append(
                sqrt(  # équation de la norme  ||x_j - x_i||
                    (x_j[0] - x_i[0]) ** 2 +
                    (x_j[1] - x_i[1]) ** 2 +
                    (x_j[2] - x_i[2]) ** 2
                )
            )

    def onKeyPressed(self, c):
        offset = 0.0
        if(c == "+"):
            offset = 1.0
        if(c == '-'):
            offset = -1.0
        self.applyTranslationOnX(offset)

    def applyTranslationOnX(self, offsetX):
        # tableau de position des noeuds:
        pos = self.MechaObject.position
        # boucle sur les noeuds
        for i in range(0, self.nbNodes):
            # pos[i][0] = coordonnée en x (et 1 = coord en y et 2 = coord en z)
            pos[i][0] = self.MechaObject.position[i][0] + \
                offsetX  # on applique l'offset sur X
            for j in range(1, 3):
                # les autres directions restent identiques
                pos[i][j] = self.MechaObject.position[i][j]

        # on renvoie à SOFA le nouveau tableau des position
        self.MechaObject.findData('position').value = pos

    # deltaTime = le pas de temps "DT" défini dans Sofa, on peut le changer (initialement il vaut 0.1)
    # but : on calcule ici le modèle de pendu
    # A chaque étape, il va falloir faire une boucle qui passe par chacun des points
    # On ne cherche pas "i de 1 à 2 mais i de 1 à nbNodes pour pas hardcodé"
    # etape 1
    # m * acceleration = m * gravite (selon y)
    # vitesse(t) = vitesse(t-1) + acceleration * dt
    # position(t) = position(t-1) + vitesse(t) * dt

    # Etape2
    # Ressorts pour simuler le lien entre deux masses
    # Passer sur chacun des ressorts, regarder quels sont les points qu'il relie, je peux aller regarder position du noeud i et du noeud j,
    # je fais le calcul de la norme et je calcule la force f_i
    # Une fois que j'ai trouvé la force sur le noeud i , je fais f_j = -f_i
    # une fois que j'ai stocké toutes les forces, sur mes noeuds je peux appliquer la loi fondamentale de la dynamique
    # (j'ai somme force donc je trouve l'accé et puis je peux calculer la vitesse et la position par intégration explicite)

    # Après, je rajoute un point en plus et un edge (voire plus)

    def onBeginAnimationStep(self, deltaTime):

        velX = 0.0
        offset = velX*deltaTime

        self.applyTranslationOnX(offset)

        # pour faire une boucle sur les edges:
        # nbEdges = self.mesh.getNbEdges()
        # EdgeI = self.mesh.edges[i]
        # EdgeI[0] # premier noeud de l'edge i
        # EdgeI[1] # deuxième noeud de l'edge i.

        # tableau de position des noeuds:
        pos = self.MechaObject.position

        # liste des forces s'appliquant sur chaque noeud
        forces = [[0, 0, 0]] * self.nbNodes

        # DEBUG
        print(forces)

        ############################################################################
        ############################################################################
        # Etape 2 : Calculer les forces des ressorts
        ############################################################################
        ############################################################################
        # POUR CHAQUE [ressort]
        for x in range(0, self.mesh.getNbEdges()):
            ressort = self.mesh.edges[x]

            # récupérer les position x_i et x_j des noeuds du ressort
            i = ressort[0]
            j = ressort[1]
            x_i = pos[ressort[0]]
            x_j = pos[ressort[1]]

            # Vecteur xi->xj = x
            # soustraction de deux listes
            vecIJ = [o - p for o, p in zip(x_j, x_i)]

            print(vecIJ) # DEBUG

            # longueur actuelle du ressort
            l = sqrt(  # formule de la norme  ||x_j - x_i||
                    (x_j[0] - x_i[0]) ** 2 +
                    (x_j[1] - x_i[1]) ** 2 +
                    (x_j[2] - x_i[2]) ** 2
            )

            # longueur initiale du ressort
            l_0 = self.l_0[i]

            # APPLIQUER [formule de la force selon le modèle masse-ressort (ad hoc)] (pour chaque coordonnée)
            #   --> f_i = k (||xj-xi|| - l_0) * ( (x_j-x_i)/||xj-xi||
            f_i = [
                self.k * (l - l_0) * (vecIJ[0] / l),  # pour x
                self.k * (l - l_0) * (vecIJ[1] / l),  # pour y
                self.k * (l - l_0) * (vecIJ[2] / l)  # pour z
            ]

            # f_j = - f_i
            f_j = [
                -f_i[0],
                -f_i[1],
                -f_i[2]
            ]

            # 

        ############################################################################
        ############################################################################
        # Etape 1 : intégration explicite des la vitesse et de la position des noeuds
        ############################################################################
        ############################################################################

        # POUR CHAQUE [noeuds]
        # for i in range(0,self.nbNodes)
