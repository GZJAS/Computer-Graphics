# Computer Graphics Project 1

## Introduction
This project is part of my ECS 175: Computer Graphics course at UC Davis. For the first project, students were asked to implement the DDA and the Bresenham line drawing algorithm, scan line algorithm for rasterizing polygons, the algorithm for two-dimensional clipping, and the two-dimensional transformations for scaling a polygon with respect to its centroid, translating a polygon by a translation vector and rotating a polygon about its centroid by an arbitrary angle. 

## Usage
This program can be run in several ways. If using a Mac, you can run directly through XCode by opening Simple_2D_Drawing_System.xcodeproj  and clicking `Run`. If you wish to use a command line interface, go to the directory where you cloned this repository and change to 'Simple_2D_Drawing_System/Simple_2D_Drawing_System'. From here:

1. Run `make`
2. Type `./graphics.out [window_width] [window_height]` 

Window_width and window_height can be set according to user personal preferences. 

If you wish to run this program on linux, decompress the tar file, and run the same commands aforementioned in the corresponding folder. Note: The header file inside 'main.cpp' has been changed as well as the Makefile linker flags. 

## Requirements
The requirements to run this program are OpenGL and GLUt which should be availalbe on most systems. If, however, they are not, you can download them at:
* [OpenGL](https://www.opengl.org/) - Open source graphics library
* [GLUT](https://www.opengl.org/resources/libraries/glut/) - The OpenGL Utility Toolkit


