from pygame.constants import K_ESCAPE
from vector import Vector
from leds import leds
import animations
import pygame, sys, math
pygame.init()

FG_COLOR = (255, 255, 255)
RPM = 5

screensize = width, height = 1920, 1080
flags = pygame.DOUBLEBUF | pygame.HWSURFACE# | pygame.FULLSCREEN

font = pygame.font.SysFont('notosans', 24)
clock = pygame.time.Clock()

screen = pygame.display.set_mode(screensize, flags)

screen.fill('black')

def dot(v, c=FG_COLOR):
    x = int(v.comp[0])
    y = int(v.comp[1])
    screen.set_at((x, y), c)

def circle(v, r, c=FG_COLOR):
    x = int(v.comp[0])
    y = int(v.comp[1])
    pygame.draw.circle(screen, c, (x,y), r)

def line(v1, v2, w=1, c=FG_COLOR):
    x1 = int(v1.comp[0])
    x2 = int(v2.comp[0])
    y1 = int(v1.comp[1])
    y2 = int(v2.comp[1])
    pygame.draw.line(screen, c, (x1,y1), (x2,y2), w)

# flip: adjust for y axis going downwards in pygame
def flip1(v):
    return Vector(
        v.comp[0],
        height - v.comp[1]
    )

def flip(vs):
    out = []
    for v in vs:
        out.append(flip1(v))
    return out

def rotatexy(vs, alpha):
    out = []
    for v in vs:
        out.append(v.rotatexy(alpha))
    return out

def draw_tree(vc, eye, alpha=0):
    for v, c in vc.items():
        p = v.rotatexy(alpha).projectxz(eye)
        p *= 5
        p += Vector(960, 0)
        p = flip1(p)
        circle(p, 4, c)


def main():
    eye = Vector(0, 300, 160)
    tree = [Vector(led) for led in leds]
    alpha = 0
    d_alpha = (2 * math.pi * RPM) / (60 * 60)
    plane_anim = animations.PlaneAnimation(tree, (0, 0, 128), (255, 255, 255))

    while 1:
        screen.fill((0,0,0))
        plane_anim.loop()
        vc = plane_anim.vc
        alpha += d_alpha
        draw_tree(vc, eye, alpha)

        for event in pygame.event.get():
            if event.type == pygame.QUIT: sys.exit()
            if event.type == pygame.KEYDOWN:
                if event.key == K_ESCAPE:
                    sys.exit()
                    
        pygame.display.flip()
        clock.tick(60)


if __name__ == '__main__':
    main()
