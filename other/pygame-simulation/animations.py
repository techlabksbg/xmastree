from vector import Vector
import random

class PlaneAnimation:
    def __init__(self, vs, bg, fg):
        self.vs = vs # list of vectors (leds)
        self.vc = {} # dict with vector and color
        self._started = False
        self.bg = bg
        self.fg = fg
        self.t = 0 # 0..1 plane position
        for p in vs:
            self.vc[p] = bg

    def start(self):
        # start new loop of animation
        self.n = Vector(
            -random.random(),
            -random.random(),
            -random.random()
        ).normalize()
        ds = []
        for v in self.vs:
            ds.append(self.n * v)
        self.dmin = min(ds)
        self.dmax = max(ds)
        self.dd = self.dmax - self.dmin
        self.t = 0
        self._started = True
    
    def blend(self, c1, c2, weight):
        return (
            (c1[0]*weight + c2[0]*(1-weight)),
            (c1[1]*weight + c2[1]*(1-weight)),
            (c1[2]*weight + c2[2]*(1-weight))
            )
    
    def loop(self, speed = 100):
        # speed: num. of steps in t
        if not self._started:
            self.start()
        step = 1 / speed
        border = self.dmin + self.t * self.dd
        for v in self.vs:
            d = self.n * v
            cc = self.vc[v]
            if border - 20 < d < border and cc == self.bg:
                self.vc[v] = self.fg
            elif d < border:
                c = self.blend(cc, self.bg, 0.9)
                self.vc[v] = c
            else:
                self.vc[v] = self.bg
        if self.t >= 1:
            self._started = False
        self.t += step
    
    
    def done(self, t):
        if not self._started:
            return True
        if t < 1:
            return False
        else:
            self._started = False
            return True

