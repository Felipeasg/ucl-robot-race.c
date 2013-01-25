hellomake: main.c sensors/*.c model/*.c ai/*.c common.c
	@gcc -g -o go -O3 main.c model/*.c sensors/*.c ai/*.c common.c -lm -I.

.PHONY: hellomake
