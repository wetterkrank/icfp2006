um : um.c tools.c tools.h
	cc -O3 -o um um.c tools.c

um32 : um32.c
	cc -O3 -m32 -o um32 um32.c
