import cv2
import numpy as np
import sys


#cv2.namedWindow(winname='out')

out = open("out.vid", "wb")

scroll = 1 if len(sys.argv)==2 else 0
headwritten = False

def gamma(colorvalues) {
    return bytes([v*v//255 for v in colorvalues])

for fname in sys.argv[1:]:
    print(fname, end="\r");
    # Image
    img = cv2.imread(fname, cv2.IMREAD_UNCHANGED) 

    if not headwritten:
        headwritten = True
        # Grayscale or color?
        print(img.shape)
        gray = (len(img.shape)==2)
        bpp = 1 if gray else 3

        # Dimensions of image
        h,w = img.shape[0:2]
        print("Image size: %dx%d" % (w,h))

        ww = min([w, int(h/180.0*140.0)])
        print("Writing header with %d x %d" % (ww,h))
        header = [ww, h, scroll, bpp]  # 1 scrolling, 1 bpp
        b = b''
        for x in header:
            b+= int(x).to_bytes(2, byteorder='little')
        out.write(b)

    for x in range(ww):
        for y in range(h):
            if gray:
                out.write(bytes([img[y][x]]))
            else:
                out.write(gamma(bytes(img[y][x][0:3])))


out.close()


# Destroy all the windows
cv2.destroyAllWindows()

