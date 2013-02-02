hellomake: main.c sensors/*.c model/*.c ai/*.c common.c
	@gcc -g -o go common.c main.c sensors/*.c model/*.c ai/*.c -lm -I ./inc/

wall: wall1.c sensors/*.c model/*.c ai/*.c common.c
	@gcc -g -o go common.c wall1.c sensors/*.c model/*.c ai/*.c -lm -I ./inc/

wallfollower: wallfollower.c sensors/*.c model/*.c ai/*.c common.c
	@gcc -g -o go common.c wallfollower.c sensors/*.c model/*.c ai/*.c -lm -I ./inc/

hallfollower: hallfollower.c sensors/*.c model/*.c ai/*.c common.c
	@gcc -g -o go common.c hallfollower.c sensors/*.c model/*.c ai/*.c -lm -I ./inc/


.PHONY: hellomake
