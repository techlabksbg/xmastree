import cv2
import numpy as np
import sys

from posdata import leds
leds = np.array(leds)

# Measures in cm
campos = np.array([300,300,160])


# perspective projection onto normal plane from viewpoint
def project(campos, p):
    n = campos
    n[2] = 0
    n = n/np.linalg.norm(n)
    n2 = n
    n2 = np.array([-n[1],n[0],0])
    # Ray from viewpoint
    d = p-campos
    t = -np.dot(n,campos)/np.dot(n,d)
    pro = campos+t*d
    return np.array([np.dot(n2,pro), pro[2]])


# Projected coordinates of leds on tree
pl = [project(campos,p) for p in leds]
# Bounding box of projected leds
tmax = [max([x[i] for x in pl]) for i in range(2)]
tmin = [min([x[i] for x in pl]) for i in range(2)]

# Image
img = cv2.imread(sys.argv[1], 0) 
cv2.namedWindow(winname='out')
cv2.imshow('out', img)

# Grayscale or color?
gray = (len(img.shape)==2)
# Dimensions of image
h,w = img.shape[0:2]
print("Image size: %dx%d" % (w,h))

# Width and height of projected leds
wt = tmax[0]-tmin[0]
ht = tmax[1]-tmin[1]
print("Tree size: %.1fx%.1f" % (wt,ht))
# Width of image when height is projected onto tree
propw = wt/ht*h

# Two 3x3-matrices, B for bitmap image coordinates, T for projected tree coordinates
B = np.mat([[propw,0,0],[0,-h+0.01,0], [0,h-0.01,1]]).transpose()
T = np.mat([[wt,0,0],[0,ht,0], [tmin[0],tmin[1],1]]).transpose()
# F*T = B   <=>   F = B * T^-1
F = B * np.linalg.inv(T)

# led projected onto image
bpl = [(F*np.mat([[x[0]], [x[1]], [1]])).flatten().tolist()[0][0:2] for x in pl]
print(bpl)
out = open("values.py", "w")
out.write("values = [\\\n")
b = b''
binfile = open("values.bin", "w")
for xoff in range(0,int(w-propw)):
    out.write("  ")
    out.write(str([round(img[int(bpl[i][1])][int(bpl[i][0]+xoff)]/255.0,3) for i in range(200)]))
    out.write(",\\\n")
    b+= bytes([img[int(bpl[i][1])][int(bpl[i][0]+xoff)] for i in range(200)])

out.write("]\n")
out.close()

binf = open("values.bin", "wb")
binf.write(b)
binf.close()


while True:
    if (cv2.waitKey(1) & 0xFF) in [ord('q'), 27, ord('Q')]:
        break

# Destroy all the windows
cv2.destroyAllWindows()

