import cv2 as cv
import http.client
from time import sleep
import numpy as np
from datetime import datetime
import math

NUMLEDS = 500


def setLed(nr):
    conn = http.client.HTTPConnection("ledwall.local",80)
    conn.request("GET", "/setled?led=%d"%nr)
    r = conn.getresponse()
    data = r.read()
    conn.close()

# See https://www.pyimagesearch.com/2014/09/29/finding-brightest-spot-image-using-python-opencv/ for inspiration
def getCoords(diff):
    diff = cv.GaussianBlur(diff, (5,5), 0)
    (minVal, maxVal, minLoc, maxLoc) = cv.minMaxLoc(diff)
    return maxLoc


unitPoints = []

def click(event, x, y, flags, param):
    global unitPoints
    if event == cv.EVENT_LBUTTONDBLCLK:
        print("unitPoint: %d, %d" % (x,y))
        unitPoints.append([x,y])


def getPerspectiveMatrix(img):
    print("Eckpunkte or, ol, ul, ur Abfahren (o ist unterer Rand der Leiste)")
    wall = np.float32([[0,0], [219,0], [219, 110], [0,110]])
    global unitPoints
    cv.imshow('default', img)
    cv.setMouseCallback('default',click)
    unitPoints = []
    num = 0
    while True:
        k = cv.waitKey(20) & 0xFF
        if k == 27:
             exit()
        if len(unitPoints)>num:
            cv.circle(img, unitPoints[-1], 10, (255, 0, 0), 2)
            cv.imshow('default', img)
            num  = len(unitPoints)
        if num==4:
            break
    return cv.getPerspectiveTransform(np.float32(unitPoints), wall)


def getHelp():
    global unitPoints
    print("Sag mir wo die LED ist, wo ist sie geblieben?")
    unitPoints = []
    cv.setMouseCallback('default',click)
    while len(unitPoints)<1:
        k = cv.waitKey(20) & 0xFF
        if k == 27:
            exit()
    
    wo = S*F*unitPoints[0]
    print("Human says ", wo)
    return wo,1.0


def getLedPos(nr, frameoff, matrix):
    setLed(nr)
    # Capture the video frame
    # by frame
    for i in range(10):
        ret, frameon = vid.read()
    diff = cv.cvtColor(cv.absdiff(frameon, frameoff),cv.COLOR_BGR2GRAY)
    # Display the resulting frame
    wo = getCoords(diff)
    print("Cam coords")
    print(wo)
    cv.circle(diff, wo, 10, (255, 0, 0), 2)
    cv.imshow('default', diff)
    cv.waitKey(200)

    position = cv.perspectiveTransform(np.array([np.array([wo],dtype='float32')]), matrix)
    print("Real coords in cm")
    print(position[0][0])
    return position[0][0]


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


setLed(9999)

sleep(1)

cv.namedWindow(winname='default')
allcams = findCam()

vid = cv.VideoCapture(min(allcams))
if not vid.isOpened():
    raise IOError("Cannot open webcam")

width = int(vid.get(cv.CAP_PROP_FRAME_WIDTH))
height = int(vid.get(cv.CAP_PROP_FRAME_HEIGHT))

print(width,height)

frameoff = getOffFrame()
matrix = getPerspectiveMatrix(frameoff)

sleep(1)

frameoff = getOffFrame()

wo = None
ledPos = [[] for i in range(NUMLEDS)]

for nr in range(NUMLEDS):
    ledPos[nr] = getLedPos(nr, frameoff, matrix)


stamp = datetime.now().strftime("%Y%m%d-%H%M%S")

datafile = open("posdata-"+stamp+".txt", "w")
for nr in range(NUMLEDS):
    datafile.write("%.1f %.1f 0\n" %(ledPos[nr][0], ledPos[nr][1]))
datafile.close()

# After the loop release the cap object
vid.release()
# Destroy all the windows
cv.destroyAllWindows()

