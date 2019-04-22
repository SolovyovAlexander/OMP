my_converter:sobel.h ppm.h main.c sobel.c ppm.c
	gcc -o my_converter sobel.h ppm.h main.c sobel.c ppm.c -lm -lpthread
