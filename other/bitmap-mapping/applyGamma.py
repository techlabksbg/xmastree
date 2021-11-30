import sys
out = open("out.vid", "wb")
with open(sys.argv[1], "rb") as f:
    header = f.read(8)
    out.write(header)
    while True:
        byte = f.read(1)
        if (len(byte)==0):
            break
        byte = byte[0]
        byte = (byte*byte)//255;
        out.write(bytes([byte]));

out.close()

