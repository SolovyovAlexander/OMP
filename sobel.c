//
// Created by alexander on 22.04.19.
//

#include "sobel.h"


//makes the image black and white
Image *black_and_white(Image *image) {
    for (int i = 0; i < image->height; ++i) {
        for (int j = 0; j < image->width; ++j) {
            int r = (image->matrix + i * image->width + j)->pixels[0];
            int g = (image->matrix + i * image->width + j)->pixels[1];
            int b = (image->matrix + i * image->width + j)->pixels[2];
            int avg = (r + g + b) / 3;

            (image->matrix + i * image->width + j)->pixels[0] = avg;
            (image->matrix + i * image->width + j)->pixels[1] = avg;
            (image->matrix + i * image->width + j)->pixels[2] = avg;
        }
    }
    return image;
}


//implementation of sobel operator, doesn't support multithreading
Image *sobel_operator(Image *image) {

    int A[3][3];
    int Gx;
    int Gy;
    int G;
    Pixel *new = (Pixel *) malloc(sizeof(Pixel) * (image->height) * (image->width));
    for (int i = 1; i < image->height - 1; ++i) {
        for (int j = 1; j < image->width - 1; ++j) {
            //fill the matrix

            A[1][1] = (image->matrix + i * image->width + j)->pixels[0];
            A[0][0] = (image->matrix + i * image->width + j - image->width - 1)->pixels[0];
            A[0][1] = (image->matrix + i * image->width + j - image->width)->pixels[0];
            A[0][2] = (image->matrix + i * image->width + j - image->width + 1)->pixels[0];
            A[1][0] = (image->matrix + i * image->width + j - 1)->pixels[0];
            A[1][2] = (image->matrix + i * image->width + j + 1)->pixels[0];
            A[2][0] = (image->matrix + i * image->width + j + image->width - 1)->pixels[0];
            A[2][1] = (image->matrix + i * image->width + j + image->width)->pixels[0];
            A[2][2] = (image->matrix + i * image->width + j + image->width + 1)->pixels[0];


            Gx = -1 * A[0][0] + -2 * A[1][0] + -1 * A[2][0] + A[0][2] + 2 * A[1][2] + A[2][2];

            Gy = -1 * A[0][0] + -2 * A[0][1] + -1 * A[0][2] + A[2][0] + 2 * A[2][1] + A[2][2];


            G = (int) sqrt(pow(Gx, 2) + pow(Gy, 2));
            if (G > 255) {
                G = 255;
            }


            (new + i * image->width + j)->pixels[0] = G;
            (new + i * image->width + j)->pixels[1] = G;
            (new + i * image->width + j)->pixels[2] = G;


        }
    }
    free(image->matrix);
    image->matrix = new;
    return image;
}


void *sobel_operator_multithread(void *thread_data) {
    pthrData *data = (pthrData *) thread_data;
    Image *image = data->image;
    int A[3][3];
    int Gx;
    int Gy;
    int G;

    for (int i = data->i_s; i < data->i_f; ++i) {
        if (i > image->width && i % image->width != 0 && i % image->width != image->width - 1 &&
            i < image->height * image->width - 1 - image->width) {
            A[1][1] = (image->matrix + i)->pixels[0];
            A[0][0] = (image->matrix + i - image->width - 1)->pixels[0];
            A[0][1] = (image->matrix + i - image->width)->pixels[0];
            A[0][2] = (image->matrix + i - image->width + 1)->pixels[0];
            A[1][0] = (image->matrix + i - 1)->pixels[0];
            A[1][2] = (image->matrix + i + 1)->pixels[0];
            A[2][0] = (image->matrix + i + image->width - 1)->pixels[0];
            A[2][1] = (image->matrix + i + image->width)->pixels[0];
            A[2][2] = (image->matrix + i + image->width + 1)->pixels[0];

            Gx = -1 * A[0][0] + -2 * A[1][0] + -1 * A[2][0] + A[0][2] + 2 * A[1][2] + A[2][2];

            Gy = -1 * A[0][0] + -2 * A[0][1] + -1 * A[0][2] + A[2][0] + 2 * A[2][1] + A[2][2];


            G = (int) sqrt(pow(Gx, 2) + pow(Gy, 2));
            if (G > 255) {
                G = 255;
            }


            (data->new_matrix + i)->pixels[0] = G;
            (data->new_matrix + i)->pixels[1] = G;
            (data->new_matrix + i)->pixels[2] = G;

        }

    }

}


void sobel_multi(int number_of_threads, Image *image) {


    pthread_t *pthreads = (pthread_t *) malloc(number_of_threads * sizeof(pthread_t));

    pthrData *threadData = (pthrData *) malloc(number_of_threads * sizeof(pthrData));
    Pixel *new_matrix = (Pixel *) malloc(sizeof(Pixel) * (image->height) * (image->width));


    div_t n = div((image->height * image->width), number_of_threads);


    if (n.rem == 0) {
        for (int i = 0; i < number_of_threads; ++i) {
            threadData[i].i_s = i * n.quot;
            threadData[i].i_f = threadData[i].i_s + n.quot - 1;
            threadData[i].new_matrix = new_matrix;
            threadData[i].image = image;
            pthread_create(&(pthreads[i]), NULL, sobel_operator_multithread, &threadData[i]);
        }

        for (int i = 0; i < number_of_threads; i++)
            pthread_join(pthreads[i], NULL);
        free(pthreads);
        free(threadData);
    } else {
        for (int i = 0; i < number_of_threads; ++i) {
            threadData[i].i_s = i * n.quot;
            threadData[i].i_f = threadData[i].i_s + n.quot - 1;
            threadData[i].new_matrix = new_matrix;
            threadData[i].image = image;
            pthread_create(&(pthreads[i]), NULL, sobel_operator_multithread, &threadData[i]);

        }


        // for remaining part of pixels
        pthread_t *pthreads1 = (pthread_t *) malloc(sizeof(pthread_t));
        //сколько потоков - столько и структур с потоковых данных
        pthrData *threadData1 = (pthrData *) malloc(sizeof(pthrData));
        threadData1->i_s = ((image->height) * (image->width)) - n.rem - 1;
        threadData1->i_f = ((image->height) * (image->width)) - 1;
        threadData1->new_matrix = new_matrix;
        threadData1->image = image;
        pthread_create(pthreads1, NULL, sobel_operator_multithread, threadData1);


        for (int i = 0; i < number_of_threads; i++)
            pthread_join(pthreads[i], NULL);
        pthread_join(*pthreads1, NULL);
        free(pthreads);
        free(threadData);
        free(pthreads1);
        free(threadData1);
    }


    free(image->matrix);
    image->matrix = new_matrix;

}