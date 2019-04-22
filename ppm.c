#include "ppm.h"


//saves the image
//filename - path and name of the image
void save_picture(const char *filename, Image *image) {
    FILE *file;
    //open file for output
    file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        exit(1);
    }


    //image format
    if (image->format[1] == '6') {
        fprintf(file, "P6\n");
    }

    if (image->format[1] == '3') {
        fprintf(file, "P3\n");
    }


    //image size
    fprintf(file, "%d %d\n", image->width, image->height);

    // rgb component depth
    fprintf(file, "%d\n", SIZE_RGB_COMPONENT);

    // pixel data for P3 format
    if (image->format[1] == '3') {
        for (int i = 0; i < image->height; ++i) {
            for (int j = 0; j < image->width; ++j) {
                fprintf(file, "%d\n", (image->matrix + i * image->width + j)->pixels[0]);
                fprintf(file, "%d\n", (image->matrix + i * image->width + j)->pixels[1]);
                fprintf(file, "%d\n", (image->matrix + i * image->width + j)->pixels[2]);
            }
        }
    }


    if (image->format[1] == '6') {
        fwrite(image->matrix, 3 * image->width, image->height, file);
    }


    free(image);


    fclose(file);

}

Image *readImage(const char *filename, Image *image) {
    char format[16];

    FILE *file;
    int c, rgb_comp_color;

    file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        exit(1);
    }


    if (!fgets(format, sizeof(format), file)) {
        perror(filename);
        exit(1);
    }
    printf("%s", format);

    if (format[0] != 'P' || (format[1] != '6' && format[1] != '3')) {
        fprintf(stderr, "Invalid image format (must be 'P6' or 'P3')\n");
        exit(1);
    }


    image = (Image *) malloc(sizeof(Image));

    if (!image) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }

    image->format[0] = format[0];
    image->format[1] = format[1];


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




    //read pixels of image
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

    if (format[1] == '6') {
        if (fread(image->matrix, 3 * image->width, image->height, file) != image->height) {
            fprintf(stderr, "Error loading image '%s'\n", filename);
            exit(1);
        }
    }


    fclose(file);
    return image;
}