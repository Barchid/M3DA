#!/usr/bin/env python
# -*- coding: utf-8 -*-
import Sofa
from math import *


def norm(v):
    """
    Calcule la norme du vecteur v en paramètre
    """
    return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    
def normV(v):
    """
    Calcule la normale de v
    """
    n = [0,0,0]
    norm_v= norm(v)
    n[0]=v[0]/norm_v;
    n[1]=v[1]/norm_v;
    n[2]=v[2]/norm_v;
    
    return n;
    
 
def diff(a,b):
    """
    Calcule la différence entre vecteur a et vecteur b --> donc calcule le segment ab-> 
    """
    v= [0,0,0]
    v[0]=b[0]-a[0];
    v[1]=b[1]-a[1];
    v[2]=b[2]-a[2];
    return v;


class controller(Sofa.PythonScriptController):




    def initGraph(self, node):
        self.rootNode = node
        self.MechaObject = node.getObject('mObject')
        self.nbNodes= len(self.MechaObject.position)
        self.mesh = node.getObject('Mesh')

        # Paramétrisation
        self.k = 1500
        self.m = 1 # masse d'une boule
        
        
        # Nombre des
        nbEdges = self.mesh.getNbEdges()
        self.l0 = [0]*nbEdges
        pos = self.MechaObject.position;
        
        for e in range(0,nbEdges) : 
            Edge = self.mesh.edges[e]
            i = Edge[0];
            j = Edge[1];
            dx_ij = diff(pos[j],pos[i]);
            self.l0[e] = norm(dx_ij)
        

        print (self.l0)
                
                                 

    def onKeyPressed(self,c):
        offset = 0.0;
        if(c=="+"):
            offset = 5.0
        if(c=='-'):
            offset = -1.0
        self.applyTranslationOnX(offset)
        
        
            
            
    def applyTranslationOnX(self,offsetX):
        # tableau de position des noeuds:
        pos = self.MechaObject.position;
        # boucle sur les noeuds     
        for i in range(0,self.nbNodes) :
            pos[i][0] = self.MechaObject.position[i][0] + offsetX # on applique l'offset sur X
            for j in range (1,3):
                pos[i][j] = self.MechaObject.position[i][j] # les autres directions restent identiques 
        
        # on renvoie à SOFA le nouveau tableau des position         
        self.MechaObject.findData('position').value= pos

    


        
        
     
     
    def onBeginAnimationStep(self,deltaTime):
        
        velX=0.0;
        offset= velX*deltaTime
        
        pos = self.MechaObject.position;
        vel = self.MechaObject.velocity;
        force = [ [ ]  ]*self.nbNodes;
        for i in range(0,self.nbNodes) :
            force[i] = force[i] + [0,0,0]
        
        self.applyTranslationOnX(offset)
        
        
        nbEdges = self.mesh.getNbEdges()
        
        for e in range(0,nbEdges) : 
            Edge = self.mesh.edges[e]
            i = Edge[0];
            j = Edge[1];
            dx_ij = diff(pos[j],pos[i]);
            
            
            
            fk= self.k*(norm(dx_ij) - self.l0[e]);
            

            
            
            n=normV(dx_ij)

            
            for k in range(0,3):
                force[i][k] -= fk*n[k]
                force[j][k] += fk*n[k]
 
        
        
        
        for i in range(0,self.nbNodes):
            # gravity
            force[i][2] = force[i][2] - self.m*9.81
            
            for k in range(0,3):
                acc= force[i][k]/self.m;
                
                if i!=0: # integration sauf sur noeud fixe
                    vel[i][k] +=  acc*deltaTime;
                    pos[i][k] +=  vel[i][k]*deltaTime;
                    
            
                    
            
        
        self.MechaObject.findData('position').value= pos  
        self.MechaObject.findData('velocity').value= vel 
            


            
        
        # pour faire une boucle sur les edges:
        # nbEdges = self.mesh.getNbEdges()
        # EdgeI = self.mesh.edges[i]
        # EdgeI[0] # premier noeud de l'edge i
        # EdgeI[1] # deuxième noeud de l'edge i. 

        
        
        
        
