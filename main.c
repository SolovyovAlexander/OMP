#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>


#define SIZE_RGB_COMPONENT 255


typedef struct {
    unsigned char pixels[3];
} Pixel;

typedef struct {
    int height;
    int width;
    Pixel *matrix;
} Image;

typedef struct {
    int i_s;
    int i_f;


    Pixel *new_matrix;
} pthrData;

static Image *image;

static Image *black_and_white() {
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


static void save_picture(const char *filename) {
    FILE *file;
    //open file for output
    file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        exit(1);
    }

    //write the header file
    //image format
    fprintf(file, "P3\n");



    //image size
    fprintf(file, "%d %d\n", image->width, image->height);

    // rgb component depth
    fprintf(file, "%d\n", SIZE_RGB_COMPONENT);

    // pixel data
    for (int i = 0; i < image->height; ++i) {
        for (int j = 0; j < image->width; ++j) {
            fprintf(file, "%d\n", (image->matrix + i * image->width + j)->pixels[0]);
            fprintf(file, "%d\n", (image->matrix + i * image->width + j)->pixels[1]);
            fprintf(file, "%d\n", (image->matrix + i * image->width + j)->pixels[2]);
        }
    }

    free(image);


    fclose(file);

}

static Image *readImage(const char *filename) {
    char format[16];

    FILE *file;
    int c, rgb_comp_color;
    //open PPM file for reading
    file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        exit(1);
    }

    //read image format
    if (!fgets(format, sizeof(format), file)) {
        perror(filename);
        exit(1);
    }
    printf("%s", format);

    //check the image format
    if (format[0] != 'P' || (format[1] != '6' && format[1] != '3')) {
        fprintf(stderr, "Invalid image format (must be 'P6' or 'P3')\n");
        exit(1);
    }




    //alloc memory form image
    image = (Image *) malloc(sizeof(Image));
    if (!image) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }

    //check for comments
    c = getc(file);
    while (c == '#') {
        while (getc(file) != '\n');
        c = getc(file);
    }

    ungetc(c, file);


    //read image size
    if (fscanf(file, "%d %d", &image->width, &image->height) != 2) {
        fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
        exit(1);
    }
    printf("%d %d\n", image->width, image->height);

    //read size of rgb component
    if (fscanf(file, "%d", &rgb_comp_color) != 1) {
        fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
        exit(1);
    }

    if (rgb_comp_color != SIZE_RGB_COMPONENT) {
        fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
        exit(1);
    }

    while (fgetc(file) != '\n');

    //memory allocaion for matrix
    image->matrix = (Pixel *) malloc(sizeof(Pixel) * (image->height) * (image->width));
    if (!image->matrix) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }




    //read pixels of image works only for P3 format
    if (format[1] == '3') {
        for (int i = 0; i < image->height; ++i) {
            for (int j = 0; j < image->width; ++j) {


                int r;

                if (fscanf(file, "%d", &r) != 1) {
                    fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
                    exit(1);
                }


                int g;

                if (fscanf(file, "%d", &g) != 1) {
                    fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
                    exit(1);
                }


                int b;

                if (fscanf(file, "%d", &b) != 1) {
                    fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
                    exit(1);
                }

                (image->matrix + i * image->width + j)->pixels[0] = r;
                (image->matrix + i * image->width + j)->pixels[1] = g;
                (image->matrix + i * image->width + j)->pixels[2] = b;


            }

        }
    }
    if (format[1]=='6'){
        if (fread(image->matrix, 3 * image->width, image->height, file) != image->height) {
            fprintf(stderr, "Error loading image '%s'\n", filename);
            exit(1);
        }
    }


    fclose(file);
    return image;
}


static Image *sobel_operator() {

    int A[3][3];
    int Gx;
    int Gy;
    int G;
    Pixel *new = (Pixel *) malloc(sizeof(Pixel) * (image->height) * (image->width));
    for (int i = 1; i < image->height - 1; ++i) {
        for (int j = 1; j < image->width - 1; ++j) {
            //creation of matrix

            A[1][1] = (image->matrix + i * image->width + j)->pixels[0];
            A[0][0] = (image->matrix + i * image->width + j - image->width - 1)->pixels[0];
            A[0][1] = (image->matrix + i * image->width + j - image->width)->pixels[0];
            A[0][2] = (image->matrix + i * image->width + j - image->width + 1)->pixels[0];
            A[1][0] = (image->matrix + i * image->width + j - 1)->pixels[0];
            A[1][2] = (image->matrix + i * image->width + j + 1)->pixels[0];
            A[2][0] = (image->matrix + i * image->width + j + image->width - 1)->pixels[0];
            A[2][1] = (image->matrix + i * image->width + j + image->width)->pixels[0];
            A[2][2] = (image->matrix + i * image->width + j + image->width + 1)->pixels[0];

            //Gx
            Gx = -1 * A[0][0] + -2 * A[1][0] + -1 * A[2][0] + A[0][2] + 2 * A[1][2] + A[2][2];
            //Gy
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
            //Gy
            Gy = -1 * A[0][0] + -2 * A[0][1] + -1 * A[0][2] + A[2][0] + 2 * A[2][1] + A[2][2];


            G = (int) sqrt(pow(Gx, 2) + pow(Gy, 2));
            if (G > 255) {
                G = 255;
            }
            if (G < 0) {
                printf("%d %d %d\n", G, Gx, Gy);

            }


            (data->new_matrix + i)->pixels[0] = G;
            (data->new_matrix + i)->pixels[1] = G;
            (data->new_matrix + i)->pixels[2] = G;

        }

    }

}

unsigned long get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long ret = tv.tv_usec;
    ret /= 1000;
    ret += (tv.tv_sec * 1000);
    return ret;
}

void sobel_multi(int number_of_threads) {


    pthread_t *pthreads = (pthread_t *) malloc(number_of_threads * sizeof(pthread_t));

    pthrData *threadData = (pthrData *) malloc(number_of_threads * sizeof(pthrData));
    Pixel *new_matrix = (Pixel *) malloc(sizeof(Pixel) * (image->height) * (image->width));


    div_t n = div((image->height * image->width), number_of_threads);


    if (n.rem == 0) {
        for (int i = 0; i < number_of_threads; ++i) {
            threadData[i].i_s = i * n.quot;
            threadData[i].i_f = threadData[i].i_s + n.quot - 1;
            threadData[i].new_matrix = new_matrix;
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
            pthread_create(&(pthreads[i]), NULL, sobel_operator_multithread, &threadData[i]);

        }


        // for remaining part of pixels
        pthread_t *pthreads1 = (pthread_t *) malloc(sizeof(pthread_t));
        //сколько потоков - столько и структур с потоковых данных
        pthrData *threadData1 = (pthrData *) malloc(sizeof(pthrData));
        threadData1->i_s = ((image->height) * (image->width)) - n.rem - 1;
        threadData1->i_f = ((image->height) * (image->width)) - 1;

        threadData1->new_matrix = new_matrix;
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


int main() {

    //open and read our ppm image
    image = readImage("/home/alexander/Изображения/test.ppm");
    image = black_and_white();
    //work with threads
    long start_time = get_time();
    sobel_multi(10);
    printf("Sobel took %ld ms.\n", get_time() - start_time);

    save_picture("/home/alexander/Изображения/try.ppm");


}