
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


int main(int argc, char **argv) {
    if (argc != 7) {
        printf("You should enter 3 arguments\n");
        printf("syntax : \n\t%s\n\t-i path and name of the image that you want to convert\n\t-r path and name of the image that you want to get \n\t-p number of threads\n",
               argv[0]);
        printf("Example: %s -i my_photo.ppm -r my_photo_sobel.ppm -p 10\n",argv[0]);
        return 0;
    }
    char *input = malloc(1024 * sizeof(char));
    char *output = malloc(1024 * sizeof(char));
    int num_threads;
    if (!strcmp(argv[1], "-i") && !strcmp(argv[3], "-r") && !strcmp(argv[5], "-p")) {

        if (sscanf(argv[2], "%s", input) != 1 || sscanf(argv[4], "%s", output) != 1 || sscanf(argv[6], "%d", &num_threads ) != 1) {
            printf("-i operation requires string\n -r operation requires string\n -p operation requires int");
        } else {
            //open and read our ppm image
            image = readImage(input, image);
            image = black_and_white(image);

            long start_time = get_time();
            sobel_multi(num_threads, image);
            printf("Time of sobel algorithm %ld ms.\n", get_time() - start_time);

            save_picture(output, image);
        }

    }


    free(input);
    free(output);


}