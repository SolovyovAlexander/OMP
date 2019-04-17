#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    int x, y;
    unsigned char pixels[3];
} Pixel;

typedef struct {
    int height;
    int width;
    Pixel *matrix;
} Image;


static Image *readImage(const char *filename) {
    char format[16];
    char size[1024];
    Image *image;
    FILE *file;
    int c;
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
        while (getc(file) != '\n') ;
        c = getc(file);
    }

    ungetc(c, file);

    //read size
    if (!fgets(size, sizeof(size), file)) {
        perror(filename);
        exit(1);
    }
    printf("%s",size);
    char *height_or_width;
    char space[1] = " ";
    char space1[1] = "\000";
    strtok(size, space1);
    height_or_width = strtok(size, space);

    sscanf(height_or_width, "%d", &image->width);
    printf("%d\n", image->width);
    height_or_width = strtok(NULL, space);
    sscanf(height_or_width, "%d", &image->height);
    printf("%d\n", image->height);



    fclose(file);
    return image;
}


int main() {

    //open and read our ppm image
   readImage("/home/alexander/Изображения/boxes_1.ppm");
    readImage("/home/alexander/Изображения/test.ppm");
    readImage("/home/alexander/Изображения/test_b.ppm");


    //allocate memory for our image
    Image *image = malloc(sizeof(Image));
    image->matrix = malloc(sizeof(Pixel) * (image->height) * (image->width));

}