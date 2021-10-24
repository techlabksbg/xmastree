import cv2 as cv
import http.client
from time import sleep
import numpy as np

# Measures in cm
#campos = np.mat([[150,200,110]]).transpose()
campos = np.mat([[230,50,90]]).transpose()
# Matrix to get space coordinates
v = campos[0:2,:]
v = v/np.linalg.norm(v)
S = np.mat([[-v[1,0],v[0,0],0], [0,0,1], [0,0,0]]).transpose()


baumpoly = np.mat(((0,0), (70,50), (0,210), (-70,50))).transpose()
baumpoly = np.vstack([baumpoly, np.ones(4)])

print("baumpoly")
print(baumpoly)

def setLed(nr):
    conn = http.client.HTTPConnection("192.168.1.75",80)
    conn.request("GET", "/setled?led=%d"%nr)
    r = conn.getresponse()
    data = r.read()
    conn.close()

# See https://www.pyimagesearch.com/2014/09/29/finding-brightest-spot-image-using-python-opencv/ for inspiration
def getCoords(diff):
    # global mask
    diff = cv.GaussianBlur(diff, (5,5), 0)
    (minVal, maxVal, minLoc, maxLoc) = cv.minMaxLoc(diff)
    print(maxVal)
    print(maxLoc)
    return maxLoc


unitPoints = []

def click(event, x, y, flags, param):
    global unitPoints
    if event == cv.EVENT_LBUTTONDBLCLK:
        print("unitPoint: %d, %d" % (x,y))
        unitPoints.append(np.mat([x,y,1]).transpose())



def getPoints(img):
    global unitPoints, baumpoly, F, Finv
    cv.imshow('diff', img)
    cv.setMouseCallback('diff',click)
    while len(unitPoints)<2:
        k = cv.waitKey(20) & 0xFF
        if k == 27:
            exit()

    cv.setMouseCallback('diff',lambda *args : None)


    unitPoints = np.hstack(unitPoints)

    neunzig = np.mat([[0,1,0],[-1,0,0],[0,0,1]]).transpose()
    print(neunzig)
    zraum = baumpoly[:,2]-baumpoly[:,0]
    print("zraum")
    print(zraum)
    zbild = unitPoints[:,1]-unitPoints[:,0]
    print("zbild")
    print(zbild)
    # Koordinatensystem im Raum
    O = np.hstack([np.linalg.inv(neunzig)*zraum, zraum, baumpoly[:,0]])
    # Koordinatensystem auf dem Bild
    B = np.hstack([neunzig*zbild, zbild, unitPoints[:,0]])
    # Abbildung vom Bild in den Raum
    F = O*np.linalg.inv(B)
    # Abbildung vom Raum auf das Bild
    Finv = np.linalg.inv(F)
    print("O")
    print(O)
    print("B")
    print(B)
    print("Finv")
    print(Finv)
    
    poly = Finv*baumpoly
    poly = np.int32(np.array(poly[0:2,:].transpose()))
    print(poly)

    # FIXME get image size from img
    mask = np.zeros((480,640),dtype=np.uint8)
    cv.fillPoly(mask,pts=[poly], color=255)
    cv.imshow('diff', mask)
    sleep(2)
    return mask




setLed(9999)

sleep(1)

cv.namedWindow(winname='diff')

vid = cv.VideoCapture(0)
if not vid.isOpened():
    raise IOError("Cannot open webcam")

ret, frameoff = vid.read()

mask = getPoints(frameoff)

sleep(1)

datafile = open("posdata-%d-%d.txt" % (campos[0,0], campos[1,0]), "w")
datafile.write("%f %f %f\n" % (campos[0,0], campos[1,0], campos[2,0]))

for nr in range(200):
    setLed(nr)
    # Capture the video frame
    # by frame
    sleep(0.1)
    ret, frameon = vid.read()
    diff = cv.cvtColor(cv.absdiff(frameon, frameoff),cv.COLOR_BGR2GRAY)
    diff = cv.bitwise_and(diff, mask)
    frameon = cv.bitwise_and(cv.cvtColor(frameon,cv.COLOR_BGR2GRAY), mask)

    # Display the resulting frame
    wo = getCoords(diff)
    cv.circle(frameon, wo, 10, (255, 0, 0), 2)
    cv.imshow('diff', frameon)

    wo = S*F*(np.mat(wo+(1,)).transpose())
    
    print(wo)
    datafile.write("%f %f %f\n" %(wo[0,0], wo[1,0], wo[2,0]))


    # the 'q' button is set as the
    # quitting button you may use any
    # desired button of your choice
    if (cv.waitKey(1) & 0xFF) in [ord('q'), 27, ord('Q')]:
        break
    nr+=1
    nr%=200

datafile.close()

# After the loop release the cap object
vid.release()
# Destroy all the windows
cv.destroyAllWindows()

