hellomake: main.c sensors/*.c queue.c common.c
	@gcc -g -o go main.c queue.c sensors/*.c common.c -I.

sensors: getSensors.c sensors/*.c queue.c common.c
	@gcc -g -o go getSensors.c queue.c sensors/*.c common.c -I.

task12: task1.2.c sensors/*.c common.c
	@gcc -g -o task12 task1.2.c sensors/*.c common.c -I.

task13: task1.3final.c sensors/*.c common.c
	@gcc -g -o task13 task1.3final.c sensors/*.c common.c -I.

task14: task1.4enhanced.c sensors/*.c common.c
	@gcc -g -o task14 task1.4enhanced.c sensors/*.c common.c -I.

.PHONY: hellomake
