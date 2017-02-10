# Computer Graphics Project 1

## Introduction
This is project 2 for ECS 175: Computer graphics. In this part, students were asked to implement a 3D representation of different shapes by way of axonometric projects on the xy, yz, and xz planes. In addition, shapes must be able to be translated, scaled, and rotated by an arbitrary axis in 3D space. All shapes should be transformed from their 3D world coordinates to a "simplified" normalized device coordinated system. I chose to implement the full NDC system. Shapes are stored by their 3D world coordinates and saved in a called "file.txt" to be read in at start of program. Likewise, users can save a particular layout after applying series of transformations that will be stored back into "file.txt". A hand drawing of the images is included as "shapes.jpg" for user convenience in visualizing shapes.

## Usage
1. Run `make`
2. Type `./p2.out [window_width] [window_height]` 
3. Menu in the console will appear. Press 1 - 5 for the corresponding action you wish to execute. 
4. Enter transformation parameters one at a time and press enter to input each operator.
5. For rotating shapes, when inputting the axis about which you would like to rotate a shape, input the each edge as such: "x1 y1 z1" 

## Requirements
The requirements to run this program are OpenGL and GLUt which should be available on most systems. If, however, they are not, you can download them at:
* [OpenGL](https://www.opengl.org/) - Open source graphics library
* [GLUT](https://www.opengl.org/resources/libraries/glut/) - The OpenGL Utility Toolkit

