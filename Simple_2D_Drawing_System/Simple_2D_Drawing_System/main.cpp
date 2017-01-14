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
#include <stdlib.h>
#include <math.h>

float *PixelBuffer;
int window_width = 200;
int window_height = 200;
int PixelBufferSize = window_width * window_height * 3;


void clearAllPixels();
void setPixel(int x, int y, double c);
void display();

// Breseham line-drawing for |m| < 1.0
void mLessThan1Plus(int x0, int y0, int xEnd, int yEnd){
    int dx = std::abs(xEnd - x0), dy = std::abs(yEnd - y0);
    int p = 2 * dy - dx;
    int twoDy = 2 * dy, twoDyMinusDx = 2 * (dy - dx);
    int x, y;
    
    // Determine which endpoint to use as start position
    if (x0 > xEnd){
        x = xEnd;
        y = yEnd;
        xEnd = x0;
    }
    else {
        x = x0;
        y = y0;
    }
    
    setPixel(x, y, 1.0);
    
    while (x < xEnd){
        x++;
        if ( p < 0)
            p+= twoDy;
        else {
            y++;
            p += twoDyMinusDx;
        }
        setPixel(x,y, 1.0);
    }
}

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

    mLessThan1Plus(50, 50, 150, 100);
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


