import cv2 as cv
import http.client
from time import sleep
import numpy as np
from datetime import datetime

NUMLEDS = 500

# Measures in cm
# campos = np.mat([[230,50,90]]).transpose()
#campos = np.mat([[146,210,118]]).transpose() # Auf tisch
# campos = np.mat([[-40,250,87]]).transpose() # Hinter balken
# campos = np.mat([[165, 197,118]]).transpose()
# campos = np.mat([[227,93,86]]).transpose()
#campos = np.mat([[-68,210,65]]).transpose() # Hinter balken
campos = np.mat([[237,24,65]]).transpose() # Hinter balken


# Matrix to get space coordinates
v = campos[0:2,:]
v = v/np.linalg.norm(v)
S = np.mat([[-v[1,0],v[0,0],0], [0,0,1], [0,0,0]]).transpose()


baumpoly = np.mat(((0,0), (80,30), (0,215), (-80,30))).transpose()
baumpoly = np.vstack([baumpoly, np.ones(4)])

print("baumpoly")
print(baumpoly)

def setLed(nr):
    conn = http.client.HTTPConnection("192.168.42.1",80)
#    conn = http.client.HTTPConnection("192.168.1.75",80)
    conn.request("GET", "/setled?led=%d"%nr)
    r = conn.getresponse()
    data = r.read()
    conn.close()

# See https://www.pyimagesearch.com/2014/09/29/finding-brightest-spot-image-using-python-opencv/ for inspiration
def getCoords(diff):
    # global mask
    diff = cv.GaussianBlur(diff, (5,5), 0)
    (minVal, maxVal, minLoc, maxLoc) = cv.minMaxLoc(diff)
    #print(maxVal)
    #print(maxLoc)
    return maxLoc


unitPoints = []

def click(event, x, y, flags, param):
    global unitPoints
    if event == cv.EVENT_LBUTTONDBLCLK:
        print("unitPoint: %d, %d" % (x,y))
        unitPoints.append(np.mat([x,y,1]).transpose())

def getHelp():
    global unitPoints
    print("Sag mir wo die LED ist, wo ist sie geblieben?")
    unitPoints = []
    cv.setMouseCallback('diff',click)
    while len(unitPoints)<1:
        k = cv.waitKey(20) & 0xFF
        if k == 27:
            exit()
    
    wo = S*F*unitPoints[0]
    print("Human says ", wo)
    return wo,1.0


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

    h,w = img.shape[0:2]
    mask = np.zeros((h,w),dtype=np.uint8)
    cv.fillPoly(mask,pts=[poly], color=255)
    cv.imshow('diff', mask)
    sleep(2)
    return mask


def getLedPos(nr, frameoff, prev=None, prevc=0.0, succ=None, succc=0.0):
    setLed(nr)
    # Capture the video frame
    # by frame
    for i in range(10):
        ret, frameon = vid.read()
    diff = cv.cvtColor(cv.absdiff(frameon, frameoff),cv.COLOR_BGR2GRAY)
    diff = cv.bitwise_and(diff, mask)
    frameon = cv.bitwise_and(cv.cvtColor(frameon,cv.COLOR_BGR2GRAY), mask)

    # Display the resulting frame
    wo = getCoords(diff)
    cv.circle(frameon, wo, 10, (255, 0, 0), 2)
    cv.imshow('diff', frameon)

    wo = S*F*(np.mat(wo+(1,)).transpose())

    esty = (25-abs(nr%50-25))/25*150+30
    estymin = 10
    estymax = (25-abs(nr%50-25))/25*140+60
    confidence = 1.0
    if (wo[2,0]<estymin or wo[2,0]>estymax):
        confidence=0.5
    dist = 0 
    if confidence<0.0:
        confidence = 0.0
    if not isinstance(succ, type(None)):
        dist = np.linalg.norm(succ-wo)
        if succc>0 and dist>20/succc:
            print("Oops! Dist to succ is %.1f>20" % dist)
            confidence *= 1-(dist-20)/20;
    if not isinstance(prev, type(None)):
        dist = np.linalg.norm(prev-wo)
        if prevc>0 and dist>20/prevc:
            print("Oops! Dist to prev is =%.1f>20" % dist)
            confidence *= 1-(dist-20)/20;
    if confidence<0:
        confidence = 0.0
    print("%.1f, %.1f, %.1f     esty=%.1f, dist=%.1f    c=%.3f" % (wo[0,0], wo[1,0], wo[2,0], esty, dist, confidence))
    return wo,confidence


def getOffFrame():
    setLed(9999)
    for i in range(24):
        ret, frameoff = vid.read()
    return frameoff

def findCam():
    cams = []
    for i in range(5):
        cap = cv.VideoCapture(i)
        if cap.read()[0]:
            cap.set(cv.CAP_PROP_FRAME_WIDTH, 10000)
            cap.set(cv.CAP_PROP_FRAME_HEIGHT, 10000)
            cams.append(i)
            print(i)
            print("Index %i -> %s", (i,cap.getBackendName()))
            cap.release()
    return cams

def newConf(d, c1, c2):
    c = max(c1,c2)
    c = (1-d/2)*1+d/2*c
    if (c<0):
        c = 0
    return c


def weightedMedian(wo, conf):
    if (len(wo)==0):
        return None,0.0
    if (len(wo)==1):
        return wo[0],conf[0]
    if (len(wo)==2):
        return (conf[0]*wo[0]+conf[1]*wo[1])/(conf[0]+conf[1]), newConf(np.linalg.norm(wo[0]- wo[1]), conf[0], conf[1])
    res = []
    c = []
    s = sum(conf)
    for i in range(3):
        xw = sorted(list(zip([[i,0] for c in wo], conf)))
        ss = 0.0
        for k in range(len(xw)):
            if ss<=s/2 and ss+xw[k][1]>=s/2:
                if k==0:
                    res.append(xw[k][0])
                    c.append(xw[k][1])
                else:
                    res.append((xw[k-1][0]*xw[k-1][1] + xw[k][0]*xw[k][1])/(xw[k-1][1]+xw[k][1]))
                    c.append(newConf(abs(xw[k-1][0]-xw[k][0]), xw[k-1][1], xw[k][1]))
                break
            ss+=xw[k][1]
    print("res, c")
    print(res)
    print(c)
    return np.mat([res]).transpose(),min(c)



setLed(9999)

sleep(1)

cv.namedWindow(winname='diff')
allcams = findCam()

vid = cv.VideoCapture(min(allcams))
if not vid.isOpened():
    raise IOError("Cannot open webcam")

vid.set(cv.CAP_PROP_FRAME_WIDTH, 10000)
vid.set(cv.CAP_PROP_FRAME_HEIGHT, 10000)
#vid.set(cv.CAP_PROP_FRAME_WIDTH, 1280)
#vid.set(cv.CAP_PROP_FRAME_HEIGHT, 720)
width = int(vid.get(cv.CAP_PROP_FRAME_WIDTH))
height = int(vid.get(cv.CAP_PROP_FRAME_HEIGHT))

print(width,height)

frameoff = getOffFrame()

mask = getPoints(frameoff)

sleep(1)

wo = None
ledPos = [[] for i in range(NUMLEDS)]
confidence = [[] for i in range(NUMLEDS)]
tries = [0 for i in range(NUMLEDS)]

limitconf = 0.8
ledsok = 0
nr = 0
while ledsok<NUMLEDS:
    if (len(ledPos[nr])==0 or max(confidence[nr])<limitconf):
        print("Locating LED %d" % nr)
        prev = None
        succ = None
        prevc = 0.0
        succc = 0.0
        if (nr>0):
            prev,prevc = weightedMedian(ledPos[nr-1], confidence[nr-1])
        if (nr<NUMLEDS-1):
            succ,succc = weightedMedian(ledPos[nr+1], confidence[nr+1])
        wo,c = getLedPos(nr, frameoff, prev, prevc, succ, succc)
        tries[nr]+=1
        if (tries[nr]>5):
            wo,c = getHelp()
        if (c>0.3): # Do not even store too bad a reading
            ledPos[nr].append(wo) 
            confidence[nr].append(c)
            if (len(ledPos[nr])>1):
                w,c = weightedMedian(ledPos[nr], confidence[nr])
                ledPos[nr] = [w]
                confidence[nr] = [c]
                print("Combined confidence:%.3f" % c)
            if (c>limitconf):
                ledsok+=1

        # the 'q' button is set as the
        # quitting button you may use any
        # desired button of your choice
        if (cv.waitKey(1) & 0xFF) in [ord('q'), 27, ord('Q')]:
            break
    nr = (nr+1)%NUMLEDS

stamp = datetime.now().strftime("%Y%m%d-%H%M%S")

datafile = open("posdata-"+stamp+"-%d-%d-%d.txt" % (campos[0,0], campos[1,0],campos[2,0]), "w")
datafile.write("%f %f %f 0.0\n" % (campos[0,0], campos[1,0], campos[2,0]))
for nr in range(NUMLEDS):
    wo,c = weightedMedian(ledPos[nr], confidence[nr])
    datafile.write("%.1f %.1f %.1f %.3f\n" %(wo[0,0], wo[1,0], wo[2,0], c))
datafile.close()

# After the loop release the cap object
vid.release()
# Destroy all the windows
cv.destroyAllWindows()

