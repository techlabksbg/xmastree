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



base = []
dirs = []
for f in sys.argv[1:]:
    data = np.loadtxt(f)
    base.append(data[0,:])
    dirs.append(data[1:,:]-base[-1])

numleds = len(dirs[0])
nummeasures = len(base)
positions = []


for i in range(nummeasures-1):
    for j in range(i+1,nummeasures):
        positions.append([getPoint([base[i], base[j]], [dirs[i][k], dirs[j][k]]) for k in range(numleds)])

#TODO weed out outliers, come up with a median of points or something (if more than 2 measurements have been performed)

#for p in positions[0]:
#    print("translate([%f,%f,%f]) cube(l);"% (p[0], p[1], p[2])) 

print([list(map(lambda x:round(x,1), list(e))) for e in positions[0]])



