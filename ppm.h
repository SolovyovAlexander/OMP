//
// Created by alexander on 22.04.19.
//



#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define SIZE_RGB_COMPONENT 255


//stores information about pixel
typedef struct {
    unsigned char pixels[3];
} Pixel;


//stores image data: format, height, width and matrix of pixels
typedef struct {
    char format[2];
    int height;
    int width;
    Pixel *matrix;
} Image;


//read the image and stores data in Image structure
//filename - path to the image and its name
Image *readImage(const char *filename, Image *image);


//saves the image
//filename - path to the image and its name
void save_picture(const char *filename, Image *image);