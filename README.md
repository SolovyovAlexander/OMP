# OMP
How to install and run this program:
  1) Download all files from this repository.
  2) Open terminal and enter in directory with downloaded files
  3) Write command: 'make'
  4) Now you should have file with name 'my_converter'
  5) Run this file from terminal using this command:
  
  ./my_converter -i 'input image' -r 'output image' -p 'number of threads'
  
  !!! Input and output image should have ppm format. It can be P3 or P6 type, program works with both
  
  example: ./my_converter -i /home/alexander/Изображения/hair.ppm -r /home/alexander/Изображения/test.ppm -p 5
  
  6) If you did everything right you should get .ppm image that was converted by sobel operator.
