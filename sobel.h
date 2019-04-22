#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "ppm.h"


typedef struct {
    int i_s;
    int i_f;
    Pixel *new_matrix;
    Image *image;
} pthrData;

Image *sobel_operator(Image *image);

static void *sobel_operator_multithread(void *thread_data);

void sobel_multi(int number_of_threads, Image *image);

Image *black_and_white(Image *image);
