hellomake: main.c sensors/*.c model/*.c ai/*.c common.c
	@gcc -g -o go common.c main.c sensors/*.c model/*.c ai/*.c -lM -I ./inc/

wall1: wall1.c sensors/*.c model/*.c ai/*.c common.c
	@gcc -g -o go common.c wall1.c sensors/*.c model/*.c ai/*.c -lM -I ./inc/

wall2: wall2.c sensors/*.c model/*.c ai/*.c common.c
	@gcc -g -o go common.c wall2.c sensors/*.c model/*.c ai/*.c -lM -I ./inc/


.PHONY: hellomake
