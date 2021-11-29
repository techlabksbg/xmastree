#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Klasse: Vorlage, was dieses Objekt hat und kann
#   "Keks-Form"
# Instanz: "Keks" mit eigenen Variablen
#    Davon kann es mehrere geben.

# Dateinamen vector.py
#from gpanel import *
from math import cos, sin

class Vector:
    # Initialisierung mit einer Liste von Zahlen
    def __init__(self, comp):
        self.comp = [x for x in comp]  # Kopie
    
    def __str__(self):  # Methode
        res = "<"
        for x in self.comp:  # Alle Komponenten durchgehen
            res += str(x)+","
        res += ">"
        return res
    
    def __add__(self, other): # + ueberladen
        res = []
        for i in range(len(self.comp)):
            res.append(self.comp[i]+other.comp[i])        
        return Vector(res)
               
    def __mul__(self, other):  # Vector * other
        return Vector([c*other for c in self.comp])

    def __sub__(self,other):  # self - other
        return self+-1*other
    
    def projectxz(self,a):  # Vektor vom Augpunkt A auf x/z projizieren
        v = self-a  # Vektor AL
        t = -a.comp[1]/v.comp[1]
        l = a+t*v
        return Vector([l.comp[0], l.comp[2]])      
    
    
    def norm(self):
        return sum(map(lambda x: x*x, self.comp))**0.5
        
    def dot(self, other):
        return sum(a[0]*a[1] for a in zip(self.comp, other.comp))
                
    def __rmul__(self, other):  # other * Vector
            return self*other
        
    def circle(self):
        move(self.comp[0], self.comp[1])
        circle(3)
        
    
    def rotatexy(self, alpha):
        copy = [c for c in self.comp]
        copy[0] = self.comp[0]*cos(alpha)-self.comp[1]*sin(alpha)
        copy[1] = self.comp[0]*sin(alpha)+self.comp[1]*cos(alpha)
        return Vector(copy)
    
    def rotateyz(self, alpha):
        copy = [c for c in self.comp]
        copy[1] = self.comp[1]*cos(alpha)-self.comp[2]*sin(alpha)
        copy[2] = self.comp[1]*sin(alpha)+self.comp[2]*cos(alpha)
        return Vector(copy)
    
    def lineTo(self, other):
        line(self.comp[0], self.comp[1],\
             other.comp[0], other.comp[1])
        
