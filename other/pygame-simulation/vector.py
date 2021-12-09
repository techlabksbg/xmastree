#!/usr/bin/env python
# -*- coding: utf-8 -*-

# irgendwas über guetzli

import math

class Vector:
    def __init__(self, *comp):
        # allow numbers or a list
        if hasattr(comp[0], '__iter__'):
            comp = comp[0]
        self.comp = tuple(comp)
    
    def __str__(self):
        return "<{}>".format(",".join(str(c) for c in self.comp))
    
    def __add__(self, other):
        res = []
        for i in range(len(self.comp)):
            res.append(self.comp[i]+other.comp[i])
        return Vector(res)
    
    def __sub__(self, other):
        return self + other * -1

    def __mul__(self, other):
        if isinstance(other, Vector):
            res = 0
            for i, c in enumerate(self.comp):
                res += c * other.comp[i]
            return res
        else:
            return Vector(c * other for c in self.comp)
    
    def __rmul__(self, other):
        return self * other
    
    def projectxz(self, a):
        """
        project vector from viewpoint A to XZ-plane
        """
        v = self - a
        t = - a.comp[1] / v.comp[1]
        l = a+t*v
        return Vector(l.comp[0], l.comp[2])
    
    def rotatexy(self, alpha):
        """
        rotate vector by angle alpha around z-axis
        """
        return Vector(
            self.comp[0]*math.cos(alpha)-self.comp[1]*math.sin(alpha),
            self.comp[0]*math.sin(alpha)+self.comp[1]*math.cos(alpha),
            self.comp[2]
        )
    
    def len(self):
        a = 0
        for c in self.comp:
            a += c**2
        return math.sqrt(a)
    
    def normalize(self):
        l = self.len()
        return Vector(c / l for c in self.comp)
