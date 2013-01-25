hellomake: main.c sensors/*.c model/*.c ai/*.c common.c
	@gcc -g -o go common.c main.c sensors/*.c model/*.c ai/*.c -lM -I ./inc/

.PHONY: hellomake
