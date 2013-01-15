hellomake: main.c sensors/*.c common.c
	@gcc -g -o go main.c sensors/*.c common.c -I.

.PHONY: hellomake
