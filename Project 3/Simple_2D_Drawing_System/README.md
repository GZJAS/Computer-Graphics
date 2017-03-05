# Computer Graphics Project 3

## Introduction
This is project 3 of Computer Graphics. In this project, I was asked to implement Phong's Lighting model for finding the color of vertices, gouraud shading algorithm, painter's algorithm, and half toning. I was not able to implement Phong's algorithm correctly or half toning. When the lightsource and viewsource are moved around, the scene does not seem to change, meaning the computation of the vertices color is not changing with respect to the lightsource or viewsource. However, the shapes do seem to be shaded properly and in order. Half-toning is a mess in it of itself, but hey - at least I tried. 


## Usage
1. Run `make`
2. Type `./p2.out [window_width] [window_height]` 
3. Menu in the console will appear. Press 1 - 5 for the corresponding action you wish to execute. 

## Requirements
The requirements to run this program are OpenGL and GLUt which should be available on most systems. If, however, they are not, you can download them at:
* [OpenGL](https://www.opengl.org/) - Open source graphics library
* [GLUT](https://www.opengl.org/resources/libraries/glut/) - The OpenGL Utility Toolkit

