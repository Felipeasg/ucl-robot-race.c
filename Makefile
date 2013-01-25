hellomake: main.c sensors/*.c model/*.c ai/*.c common.c
	@gcc -g -o go main.c sensors/*.c model/*.c ai/*.c common.c -lM

.PHONY: hellomake
