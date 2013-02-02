hellomake: main.c sensors/*.c model/*.c ai/*.c common.c
	@gcc -g -o go common.c main.c sensors/*.c model/*.c ai/*.c -lm -I ./inc/

wall: wall1.c sensors/*.c model/*.c ai/*.c common.c
	@gcc -g -o go common.c wall1.c sensors/*.c model/*.c ai/*.c -lm -I ./inc/

wall2: wall3.c sensors/*.c model/*.c ai/*.c common.c
	@gcc -g -o go common.c wall3.c sensors/*.c model/*.c ai/*.c -lm -I ./inc/

wallNew: wallNew.c sensors/*.c model/*.c ai/*.c common.c
	@gcc -g -o go common.c wallNew.c sensors/*.c model/*.c ai/*.c -lm -I ./inc/


.PHONY: hellomake
