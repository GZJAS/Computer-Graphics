//
//  main.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/12/17.
//
//
#include "Line.hpp"
#include "Polygon.hpp"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>

// global variables
int display_count = 0;
int pid = 0;
int window_width, window_height;
int PixelBufferSize;
float *PixelBuffer;
float xmin, xmax, ymin, ymax;

// global variable to hold all polygons
std::vector<Geometry *>all_shapes;

int getPosByID(Geometry *obj){
    for(int i = 0; i < all_shapes.size(); i++){
        if (all_shapes[i]->id == obj->id){
            return i;
        }
    }
    return -1;
}

Geometry *getObjectByID(int id){
    for(auto i: all_shapes){
        if (i->id == id){
            return i;
        }
    }
    return nullptr;
}


// sets the local in pixelBuffer to c given x and y coordinates
void setPixel(int x, int y, double c){
    int i = (y * window_width + x) * 3;
    for(int j = 0; j < 3; j++){
        PixelBuffer[i + j] = c;
    }
    return;
    
}

// returns a list of polygon ids inside all_shapes vector
std::vector<int>findPolygons(){
    std::vector<int>polygon_ids;
    for(auto i : all_shapes){
        if (i->name == "Polygon"){
            polygon_ids.push_back(i->id);
        }
    }
    return polygon_ids;
}


// clears all of the pixels from the screen
void clearAllPixels(){
    for(int i = 0; i < window_width; i++){
        for(int j = 0; j < window_height; j++){
            setPixel(i, j, 0.0);
        }
    }
}

void interface(){
    std::string answer;
    std::vector<std::array<float, 2>>points;
    
    std::cout << std::endl << "What would you like to do?" << std::endl <<
    "\t(1) Draw new line" << std::endl <<
    "\t(2) Draw new polygon" << std::endl <<
    "\t(3) Clip" << std::endl <<
    "\t(4) Translate" << std::endl <<
    "\t(5) Scale" << std::endl <<
    "\t(6) Rotate" << std::endl <<
    "\t(7) Save to file" << std::endl <<
    "\t(8) Exit" << std::endl;
    
    std::cout << ">> ";
    std::cin >> answer;
    
    if (answer == "1"){
        std::string algorithm;
        float x1, y1, x2, y2;
        std::cout << "Coordinates for first point: " << std::endl;
        std::cin >> x1 >> y1;
        std::cout << "Coordinates for second point: " << std::endl;
        std::cin >> x2 >> y2;
        Line *line = new Line(x1, y1, x2, y2);
        all_shapes.push_back(line);
        
        
        std::cout << std::endl << "How would you like to draw the line?" << std::endl <<
        "\t(a) Using DDA algorithm " << std::endl <<
        "\t(b) Using Bresenham's algorithm " << std::endl;
        std::cout << ">> ";
        std::cin >> algorithm;

        std::cout << "Line created with id " << line->id << std::endl << std::endl;

        if(algorithm == "a"){
            line->lineDDA();
        }
        else if (algorithm == "b"){
            line->lineBres();
        }
    }
    else if (answer == "2"){

        std::string val1, val2;
        int numPoints;
        points.clear();
        std::cout << "How many vertices will this polygon have? " << std::endl << "points = ";
        std::cin >> numPoints;
        
        for(int i = 0; i < numPoints; i++){
            std::cout << "Enter coordinates for vertex " << i + 1 << ": " << std::endl;
            std::cin >> val1 >> val2;
            float x = std::stof(val1);
            float y = std::stof(val2);
            std::array<float, 2>coordinates = {x, y};
            points.push_back(coordinates);
        }
        
        if(!points.empty()){
            Polygon *polygon = new Polygon(points);
            all_shapes.push_back(polygon);
            polygon->drawPolygon();
            std::cout << "Polygon created with id " << polygon->id << std::endl << std::endl;
        }

        
    }
    else if (answer == "3"){
        std::cout << "xmin = ";
        std::cin >> xmin;
        std::cout << "xmax = ";
        std::cin >> xmax;
        std::cout << "ymin = ";
        std::cin >> ymin;
        std::cout << "ymax = ";
        std::cin >> ymax;
        
        std::vector<int>delete_index;
        for(auto i : all_shapes){
            int exists = i->clip();
            if (exists <= 0){
                int pos = getPosByID(i);
                if(pos >= 0){
                    delete_index.push_back(pos);
                    
                }
            }
        }
        
        // delete all shapes whose vertices are outside clipping window
        for (auto i : delete_index){
            all_shapes[i] = all_shapes.back();
            all_shapes.pop_back();
        }
        
    }
    
    else if (answer == "4"){
        int shape_id;
        int x, y;
        std::vector<int>polygons;
        
        std::cout << "Enter the id of the polygon you wish to translate: " << std::endl;
        polygons = findPolygons();
        for(auto poly_id : polygons){
            std::cout << poly_id << std::endl;
        }
        std::cout << ">> ";
        std::cin >> shape_id;
        
        // get the object the user specified
        Geometry *shape = getObjectByID(shape_id);
        
        // get transformation parameters
        std::cout << "How would you like to translate the Polygon?" << std::endl;
        std::cout << "x = ";
        std::cin >> x;
        std::cout << "y = ";
        std::cin >> y;
        
        // carry out transformation operations
        shape->convertToMatrix();
        shape->translate(x, y);
        shape->convertToVector();
        shape->updateParameters();

    }
    else if (answer == "5"){
        int shape_id;
        double alpha, beta;
        std::vector<int>polygons;
        std::cout << "Enter the id of the polygon you wish to scale: " << std::endl;
        polygons = findPolygons();
        for(auto poly_id : polygons){
            std::cout << poly_id << std::endl;
        }
        std::cout << ">> ";
        std::cin >> shape_id;
        
        // get the object the user specified
        Geometry *shape = getObjectByID(shape_id);
        
        // get transformation parameters
        std::cout << "How would you like to scale the Polygon? " << std::endl;
        std::cout << "alpha = ";
        std::cin >> alpha;
        std::cout << "beta = ";
        std::cin >> beta;
        
        // carry out transformation operations
        shape->convertToMatrix();
        shape->scale(alpha, beta);
        shape->convertToVector();
        shape->updateParameters();
        
    }
    else if (answer == "6"){
        int shape_id;
        double angle;
        std::vector<int>polygons;
        std::cout << "Enter the id of the polygon you wish to rotate: " << std::endl;
        polygons = findPolygons();
        for(auto poly_id : polygons){
            std::cout << poly_id << std::endl;
        }
        std::cout << ">> ";
        std::cin >> shape_id;
        
        // get the object the user specified
        Geometry *shape = getObjectByID(shape_id);
        
        std::cout << "How would you like to rotate the Polygon? " << std::endl;
        std::cout << "angle = ";
        std::cin >> angle;
 
        
        // carry out transformation operations
        shape->convertToMatrix();
        shape->rotate(angle);
        shape->convertToVector();
        shape->updateParameters();
    }
    else if (answer == "7"){
        std::ofstream outputFile("file.txt", std::ios::out);
        
        // exit program if unable to create file
        if(!outputFile){
            std::cerr << "File could not be opened" << std::endl;
        }
        
        outputFile << all_shapes.size() << std::endl << std::endl;
        
        for(auto shape : all_shapes){
            outputFile << shape->n << std::endl;
            for(auto vertex : shape->vertices){
                outputFile << vertex->x << " " << vertex->y << std::endl;
            }
            outputFile << std::endl;
        }
    }
    else if (answer == "8"){
        exit(0);
    }
    else {
        std::cout << "Invalid answer. Please try again" << std::endl << std::endl;
        interface();
    }
    
}

// main display loop, this function will be called again and again by OpenGL
void display(){
    
    std::string answer;
    //Misc.
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    if(display_count){
        interface();
    }
    
    clearAllPixels();
    
    for(auto i : all_shapes){
        i->draw();
    }
    
    //draws pixel on screen, width and height must match pixel buffer dimension
    glDrawPixels(window_width, window_height, GL_RGB, GL_FLOAT, PixelBuffer);
    
    //window refresh
    glFlush();
    
    display_count = 1;
    glutPostRedisplay();
}


int main(int argc, char *argv[]){
    
    window_width = std::atoi(argv[1]);
    window_height = std::atoi(argv[2]);
    PixelBufferSize = window_width * window_height * 3;
    
    int numPolygons, numPoints = 0;
    std::string chars;
    
    std::ifstream inputFile("file.txt", std::ios::in);
    
    // exit program if unable to create file
    if(!inputFile){
        std::cerr << "File could not be opened" << std::endl;
    }
    
    // read in first line of file = number of polygons we are creating
    getline(inputFile, chars);
    numPolygons = atoi(chars.c_str());
    
    
    // container to hold coordinates
    std::vector<std::array<float, 2>>points;
    
    getline(inputFile, chars);
    
    // read in file
    while(!inputFile.eof()){
        
        while(chars != ""){
            std::istringstream iss;
            iss.str(chars);
            std::string val1, val2;
            iss >> val1 >> val2;
            float x = std::stof(val1);
            float y = std::stof(val2);
            std::cout << std::endl;
            std::array<float, 2>coordinates = {x, y};
            points.push_back(coordinates);
            getline(inputFile, chars);
        }
        
        if(!points.empty()){
            if (numPoints == 2){
                Line *line = new Line(points);
                all_shapes.push_back(line);
            }
            else if (numPoints > 2){
                Polygon *polygon = new Polygon(points);
                all_shapes.push_back(polygon);
            }
            points.clear();
        }
        
        getline(inputFile, chars);
        numPoints = atoi(chars.c_str());
        getline(inputFile, chars);
    }
    
    //allocate new pixel buffer, need initialization!!
    PixelBuffer = new float[PixelBufferSize];
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    
    //set window size to 200*200
    glutInitWindowSize(window_width, window_height);
    
    //set window position
    glutInitWindowPosition(100, 100);
    
    //create and set main window title
    glutCreateWindow("Hello Graphics!!");
    
    //clears the buffer of OpenGL
    glClearColor(0, 0, 0, 0);
    
    //sets display function
    glutDisplayFunc(display);
    
    //main display loop, will display until terminate
    glutMainLoop();
    return 0;
}


