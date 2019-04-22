#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "ppm.h"

// structure to send data to thread
// i_s - where this thread starts in matrix of image, i_f where this thread ends in matrix of image
typedef struct {
    int i_s;
    int i_f;
    Pixel *new_matrix;
    Image *image;
} pthrData;

Image *sobel_operator(Image *image);

//sobel operator with multithreading
void sobel_multi(int number_of_threads, Image *image);

//sobel_multi function use this function
//sobel operator for thread
static void *sobel_operator_multithread(void *thread_data);


//make image black and white
Image *black_and_white(Image *image);
