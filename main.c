#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
    for (int i = 0; i < image->width; ++i) {
        for (int j = 0; j < image->height; ++j) {
            int r = (image->matrix + i * image->height + j)->pixels[0];
            int g = (image->matrix + i * image->height + j)->pixels[1];
            int b = (image->matrix + i * image->height + j)->pixels[2];
            int avg = (r + g + b) / 3;

            (image->matrix + i * image->height + j)->pixels[0] = avg;
            (image->matrix + i * image->height + j)->pixels[1] = avg;
            (image->matrix + i * image->height + j)->pixels[2] = avg;
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
    for (int i = 0; i <image->width; ++i) {
        for (int j = 0; j < image->height; ++j) {
            fprintf(file, "%d\n", (image->matrix + i * image->height + j)->pixels[0]);
            fprintf(file, "%d\n", (image->matrix + i * image->height + j)->pixels[1]);
            fprintf(file, "%d\n", (image->matrix + i * image->height + j)->pixels[2]);
        }
    }



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
        for (int i = 0; i < image->width; ++i) {
            for (int j = 0; j < image->height; ++j) {


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

                if (fscanf(file, "%d\n", &b) != 1) {
                    fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
                    exit(1);
                }

                (image->matrix + i * image->height + j)->pixels[0] = r;
                (image->matrix + i * image->height + j)->pixels[1] = g;
                (image->matrix + i * image->height + j)->pixels[2] = b;




            }

        }
    }






    fclose(file);
    return image;
}


int main() {

    //open and read our ppm image
    Image *img = readImage("/home/alexander/Изображения/test.ppm");
    img = black_and_white(img);
    save_picture("/home/alexander/Изображения/try.ppm", img);


}