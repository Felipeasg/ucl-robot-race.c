encoders: encoders.c ../sensors/*.c ../common.c
	@gcc -g -o encoders encoders.c ../sensors/*.c ../common.c -I.


position: position.c ../sensors/*.c ../model/*.c ../ai/*.c ../common.c
	@gcc -g -o go ../common.c position.c ../sensors/*.c ../model/*.c ../ai/*.c -lm -I ../inc/


.PHONY: encoders
