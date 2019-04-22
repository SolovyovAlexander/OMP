
#include <sys/time.h>

#include "sobel.h"


#define SIZE_RGB_COMPONENT 255

Image *image;


//returns time in ms
unsigned long get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long ret = tv.tv_usec;
    ret /= 1000;
    ret += (tv.tv_sec * 1000);
    return ret;
}


int main() {

    //open and read our ppm image
    image = readImage("/home/alexander/Изображения/hair.ppm", image);
    image = black_and_white(image);

    long start_time = get_time();
    sobel_multi(10, image);
    printf("Sobel took %ld ms.\n", get_time() - start_time);

    save_picture("/home/alexander/Изображения/try.ppm", image);

}