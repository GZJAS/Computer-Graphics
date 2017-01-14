//
//  main.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/12/17.
//
//

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <iostream>

float *PixelBuffer;
int window_width = 200;
int window_height = 200;
int PixelBufferSize = window_width * window_height * 3;


void clearAllPixels();
void setPixel(int x, int y, double c);
void display();

// clears all of the pixels from the screen
void clearAllPixels(){
    for(int i = 0; i < 200; i++){
        for(int j = 0; j < 200; j++){
            setPixel(i, j, 1.0);
        }
    }
}

// sets the local in pixelBuffer to c given x and y coordinates
void setPixel(int x, int y, double c){
    int i = (y * window_width + x) * 3;
    for(int j = 0; j < 3; j++){
        PixelBuffer[i + j] = c;
    }
    return;
}

// main display loop, this function will be called again and again by OpenGL
void display(){
    
    //Misc.
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    //draws pixel on screen, width and height must match pixel buffer dimension
    glDrawPixels(window_width, window_height, GL_RGB, GL_FLOAT, PixelBuffer);
    
    //window refresh
    glFlush();
}



int main(int argc, char *argv[]){
    //allocate new pixel buffer, need initialization!!
    PixelBuffer = new float[PixelBufferSize];
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    //set window size to 200*200
    glutInitWindowSize(window_width, window_height);
    //set window position
    glutInitWindowPosition(100, 100);
    
    //create and set main window title
    int MainWindow = glutCreateWindow("Hello Graphics!!");
    glClearColor(0, 0, 0, 0); //clears the buffer of OpenGL
    //sets display function
    glutDisplayFunc(display);
    
    glutMainLoop();//main display loop, will display until terminate
    return 0;
}


