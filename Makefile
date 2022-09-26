build:
	gcc -Wall -std=gnu11 -g quadtree.c -o quadtree -lm
clean:
	rm ./quadtree
