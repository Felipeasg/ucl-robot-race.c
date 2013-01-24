hellomake: main.c sensors/*.c common.c
	@gcc -g -o go -O3 main.c sensors/*.c common.c -lm -I.

sensors: getSensors.c sensors/*.c common.c
	@gcc -g -o go getSensors.c sensors/*.c common.c -lm -I.

.PHONY: hellomake
