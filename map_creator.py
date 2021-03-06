#!/usr/bin/python3

import sys

if len(sys.argv) < 2:
    print("Missing argument")
    sys.exit(1)

fp = open(sys.argv[1], 'w')

types = ["0", "1"]
bricks = ["0", "1", "2"]

x = 0
y = 0
color = 0
mode = 1
for y in range(9):
    for x in range(20):
        fp.write("1")
        fp.write(' ')
        fp.write(bricks[color])
        if x != 19:
            fp.write(' ')
        color = (color + 1) % 3
    fp.write("\n")
