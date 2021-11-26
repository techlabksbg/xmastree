import cv2
import numpy as np
import sys





# Image
img = cv2.imread(sys.argv[1], 0) 
cv2.namedWindow(winname='out')
cv2.imshow('out', img)

# Grayscale or color?
gray = (len(img.shape)==2)
if not gray:
    raise "Only gray images supported for now"
    
# Dimensions of image
h,w = img.shape[0:2]
print("Image size: %dx%d" % (w,h))

out = open("out.vid", "wb")
ww = min([w, int(h/180.0*140.0)])
print("Writing header with %d x %d" % (ww,h))
header = [ww, h, 1, 1]  # 1 scrolling, 1 bpp
b = b''
for x in header:
    b+= int(x).to_bytes(2, byteorder='little')
out.write(b)

for x in range(w):
    for y in range(h):
       out.write(bytes([img[y][x]]))

out.close()
exit()

# led projected onto image
bpl = [(F*np.mat([[x[0]], [x[1]], [1]])).flatten().tolist()[0][0:2] for x in pl]
print(bpl)
out = open("values.py", "w")
out.write("values = [\\\n")
b = b''
binfile = open("values.bin", "w")
xoff = 0.0
while (xoff < w-propw):
    out.write("  ")
    out.write(str([round(img[int(bpl[i][1])][int(bpl[i][0]+xoff)]/255.0,3) for i in range(numleds)]))
    out.write(",\\\n")
    b+= bytes([img[int(bpl[i][1])][int(bpl[i][0]+xoff)] for i in range(numleds)])
    xoff+=1

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

