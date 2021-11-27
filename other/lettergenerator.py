from gpanel import *
import time

makeGPanel(Size(256, 256))

alphabet = "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z 1 2 3 4 5 6 7 8 9 0 : . - _ + * \" \\ ? ! ^ % &"

letters = alphabet.split()

print(letters)
    

fon = Font("Arial", Font.BOLD, 256)

ind = 0
for letter in letters:
    f = open(str(ind) + ".bin", "w")
    clear()
    text(0, 0, letter, fon, "black", "white")
    read = []
    xmin = 256
    xmax = 0
    ymin = 256
    ymax = 0 
    for x in range(256):
        row = []
        for y in range(256):
            col = getPixelColor(x/256,y/256)
            if str(col) == "java.awt.Color[r=0,g=0,b=0]":
                if x < xmin:
                    xmin = x
                if x > xmax:
                    xmax = x
                if y < ymin:
                    ymin = y
                if y > ymax:
                    ymax = y
                row.append(1)
            else:
                row.append(0)
        read.append(list(row))
    f.write(str(xmax + 1 - xmin) + " " + str(ymax + 1 - ymin) + "\n")
    for x in range(xmin, xmax + 1):
        for y in range(ymin, ymax + 1):
            f.write(str(read[x][y]) + " ")
        f.write("\n")
    f.close()
    ind += 1