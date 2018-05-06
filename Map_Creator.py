#!/usr/bin/python3

import sys

fp = open(sys.argv[1], 'w')

types = ["0", "1"]
bricks = ["R", "G", "B"]

x = 0
y = 0
color = 0
mode = 1
while y < 15:
	if y > 8:
		mode = 0
	x = 0
	while x < 20:
		fp.write(types[mode])
		fp.write(' ')
		fp.write(bricks[color])
		fp.write(' ')

		if color == 2:
			color = 0
		else:
			color += 1
		x += 1
	fp.write("\n")
	y += 1
