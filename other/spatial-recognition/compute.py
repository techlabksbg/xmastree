import numpy as np
import sys

# 2 base-points, as arrays
# 2 direction vectors, as arrays
def getPoint(b, d):
    n = np.cross(d[0],d[1])
    e = [np.cross(n,d[i]) for i in range(2)]
    # (b[0] + t*d[0]) dot e[1] = b[1] dot e[1]
    # t = ((b[1]-b[0]) dot e[1] ) / ( d[0] dot e[1] )
    t = [np.dot(e[1-i], (b[1-i]-b[i])) / np.dot(e[1-i],d[i]) for i in range(2)]
    pts = [b[i]+t[i]*d[i] for i in range(2)]
    return (pts[0]+pts[1])/2


def median(pts):
    return [sorted([p[i] for p in pts])[len(pts)//2] for i in range(3)]


base = []
dirs = []
conf = []
for f in sys.argv[1:]:
    data = np.loadtxt(f)
    base.append(data[0,:3])
    dirs.append(data[1:,:3]-base[-1])
    conf.append(data[1:,3])
numleds = len(dirs[0])
nummeasures = len(base)
positions = []


for i in range(nummeasures-1):
    for j in range(i+1,nummeasures):
        positions.append([getPoint([base[i], base[j]], [dirs[i][k], dirs[j][k]]) for k in range(numleds)])

if len(positions)>1:
    pts = []
    for k in range(numleds):
        kp = [p[k] for p in positions]
        kp = median(kp)
        pts.append(kp)
    pts = [[round(x,1) for x in e] for e in pts]
    print(pts)
    datafile = open("posdata.txt", "w") 
    for p in pts:
        datafile.write("%.1f %.1f %.1f\n" % tuple(p))
    datafile.close
    datafile = open("posdata.py", "w")
    datafile.write("leds = ")
    datafile.write(str(pts))
    datafile.write("\n")
    datafile.close()
else:   
    print([list(map(lambda x:round(x,1), list(e))) for e in positions[0]])



