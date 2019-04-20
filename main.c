#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


#define SIZE_RGB_COMPONENT 255

typedef struct {
    int pixels[3];
} Pixel;

typedef struct {
    int height;
    int width;
    Pixel *matrix;
} Image;

static Image *black_and_white(Image *image) {
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


static void save_picture(const char *filename, Image *image) {
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
    Image *image;
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


    fclose(file);
    return image;
}


static Image *sobel_operator(Image *image) {

    int A[3][3];
    int Gx;
    int Gy;
    int G;
    Pixel* new = (Pixel *) malloc(sizeof(Pixel) * (image->height) * (image->width));
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


            G = (int) sqrt(pow(Gx,2)+pow(Gy,2));
            if (G > 255) {
                G = 255;
            }


            (new + i * image->width + j)->pixels[0] = G;
            (new + i * image->width + j)->pixels[1] = G;
            (new + i * image->width + j)->pixels[2] = G;


        }
    }
    free(image->matrix);
    image->matrix=new;
    return image;
}

static Image *sobel_operator_color(Image *image) {

    for (int i = 1; i < image->height - 1; ++i) {
        for (int j = 1; j < image->width - 1; ++j) {
            //creation of matrix
            int A[3][3];
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
            int Gx = -1 * A[0][0] + -2 * A[1][0] + -1 * A[2][0] + A[0][2] + 2 * A[1][2] + A[2][2];
            //Gy
            int Gy = -1 * A[0][0] + -2 * A[0][1] + -1 * A[0][2] + A[2][0] + 2 * A[2][1] + A[2][2];


            int G = (int) sqrt((Gx * Gx + Gy * Gy));
            if (G > 255) {
                G = 0;
            }
            if (G < 0) {
                G = 0;
            }
            (image->matrix + i * image->width + j)->pixels[0] = G;


            A[1][1] = (image->matrix + i * image->width + j)->pixels[1];
            A[0][0] = (image->matrix + i * image->width + j - image->width - 1)->pixels[1];
            A[0][1] = (image->matrix + i * image->width + j - image->width)->pixels[1];
            A[0][2] = (image->matrix + i * image->width + j - image->width + 1)->pixels[1];
            A[1][0] = (image->matrix + i * image->width + j - 1)->pixels[1];
            A[1][2] = (image->matrix + i * image->width + j + 1)->pixels[1];
            A[2][0] = (image->matrix + i * image->width + j + image->width - 1)->pixels[1];
            A[2][1] = (image->matrix + i * image->width + j + image->width)->pixels[1];
            A[2][2] = (image->matrix + i * image->width + j + image->width + 1)->pixels[1];

            Gx = -1 * A[0][0] + -2 * A[1][0] + -1 * A[2][0] + A[0][2] + 2 * A[1][2] + A[2][2];

            Gy = -1 * A[0][0] + -2 * A[0][1] + -1 * A[0][2] + A[2][0] + 2 * A[2][1] + A[2][2];
            G = (int) sqrt((Gx * Gx + Gy * Gy));
            if (G > 255) {
                G = 0;
            }
            if (G < 0) {
                G = 0;
            }


            (image->matrix + i * image->width + j)->pixels[1] = G;


            A[1][1] = (image->matrix + i * image->width + j)->pixels[2];
            A[0][0] = (image->matrix + i * image->width + j - image->width - 1)->pixels[2];
            A[0][1] = (image->matrix + i * image->width + j - image->width)->pixels[2];
            A[0][2] = (image->matrix + i * image->width + j - image->width + 1)->pixels[2];
            A[1][0] = (image->matrix + i * image->width + j - 1)->pixels[2];
            A[1][2] = (image->matrix + i * image->width + j + 1)->pixels[2];
            A[2][0] = (image->matrix + i * image->width + j + image->width - 1)->pixels[2];
            A[2][1] = (image->matrix + i * image->width + j + image->width)->pixels[2];
            A[2][2] = (image->matrix + i * image->width + j + image->width + 1)->pixels[2];

            Gx = -1 * A[0][0] + -2 * A[1][0] + -1 * A[2][0] + A[0][2] + 2 * A[1][2] + A[2][2];

            Gy = -1 * A[0][0] + -2 * A[0][1] + -1 * A[0][2] + A[2][0] + 2 * A[2][1] + A[2][2];
            G = (int) sqrt((Gx * Gx + Gy * Gy));
            if (G > 255) {
                G = 255;
            }
            if (G < 0) {
                G = 0;
            }


            (image->matrix + i * image->width + j)->pixels[2] = G;


        }
    }
    return image;
}


int main() {

    clock_t begin = clock();
    //open and read our ppm image
    Image *img = readImage("/home/alexander/Изображения/wik.ppm");
    img = black_and_white(img);
    img = sobel_operator(img);
    save_picture("/home/alexander/Изображения/try.ppm", img);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Program takes %lf\n",time_spent);



}