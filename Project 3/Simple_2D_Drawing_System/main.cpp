//
//  main.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/12/17.
//
//
#include "Line.hpp"
#include "Polygon.hpp"
#include "threeD.hpp"
#include "Edge.hpp"
#include "Color.hpp"
#include "Point2D.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <map>


#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

// global variables
int display_count = 0;
int window_width, window_height;
int PixelBufferSize;
float *PixelBufferXY, *PixelBufferYZ, *PixelBufferXZ, *tmpXY, *tmpYZ, *tmpXZ;
float xmin, xmax, ymin, ymax;
int winXY, winYZ, winXZ, MainWindow;
float maxRGB, bigK;
bool run_Phong_once = true;
bool TURN_HALF_TONE_ON = false;
std::map<std::string, Color> ColorMap = {{"red", {1, 0, 0}},
    {"green", {0, 1, 0}},
    {"blue", {0, 0, 1}},
    {"white", {1, 1, 1}}};


Point lightsource = {49, 49, 49};
Point viewsource = {49, 49, 49};
Color ambientIntensity = {.25, .25, .25};
Color lightIntensity = ColorMap["white"];


// global variable to hold all polygons
std::vector<threeD*>all_shapes;
std::vector<threeD*>rotate_axis;


// function prototypes;
void displayXY();
void displayYZ();
void displayXZ();
void setPixelXZ(int x, int z, double c);
void setPixelYZ(int z, int y, double c);
void setPixelXY(int x, int y, double c);



// finds max RGB over all pixels
float findmaxRGB(){
    float maxColorVal = 0;
    for (auto shape : all_shapes){
        for(auto vertex: shape->vertices){
            if(vertex->color.r > maxColorVal){
                maxColorVal = vertex->color.r;
            }
            if (vertex->color.g > maxColorVal){
                maxColorVal = vertex->color.g;
            }
            if (vertex->color.b > maxColorVal){
                maxColorVal = vertex->color.b;
            }
        }
    }
    
    return maxColorVal;
}

threeD *getObjectByID(int id){
    for(auto i: all_shapes){
        if (i->id == id){
            return i;
        }
    }
    return nullptr;
}


Point2D getPixel(int i, float PixelBuff []){
    Point2D pt;
    pt.color.r = PixelBuff[i];
    pt.color.g = PixelBuff[i+1];
    pt.color.b = PixelBuff[i+2];
    return pt;
}

// sets the local in pixelBuffer to c given x and y coordinates
void setPixelYZ(int z, int y, Color color){
    int i = (y * window_width/2 + z) * 3;
    PixelBufferYZ[i] = color.r;
    PixelBufferYZ[i + 1] = color.g;
    PixelBufferYZ[i + 2] = color.b;
    return;
    
}

// sets the local in pixelBuffer to c given x and y coordinates
void setPixelXZ(int x, int z, Color color){
    int i = (z * window_width/2 + x) * 3;
    PixelBufferXZ[i] = color.r;
    PixelBufferXZ[i + 1] = color.g;
    PixelBufferXZ[i + 2] = color.b;
    return;
    
}

// sets the local in pixelBuffer to c given x and y coordinates
void setPixelXY(int x, int y, Color color){
    int i = (y * window_width/2 + x) * 3;
    PixelBufferXY[i] = color.r;
    PixelBufferXY[i + 1] = color.g;
    PixelBufferXY[i + 2] = color.b;
    return;
    
}

void HFsetPixXY(int i, Color color){
    tmpXY[i] = color.r;
    tmpXY[i+1] = color.g;
    tmpXY[i+2] = color.b;
    return;
}

void HFsetPixYZ(int i, Color color){
    tmpYZ[i] = color.r;
    tmpYZ[i+1] = color.g;
    tmpYZ[i+2] = color.b;
    return;
}

void HFsetPixXZ(int i, Color color){
    tmpXZ[i] = color.r;
    tmpXZ[i+1] = color.g;
    tmpXZ[i+2] = color.b;
    return;
}

// clears all of the pixels from the screen
void clearXYPixels(){
    for(int i = 0; i < PixelBufferSize/4; i++){
        PixelBufferXY[i] = 0.0;
    }
}
void clearYZPixels(){
    for(int i = 0; i < PixelBufferSize/4; i++){
        PixelBufferYZ[i] = 0.0;
    }
    
}

void clearXZPixels(){
    for(int i = 0; i < PixelBufferSize/4; i++){
        PixelBufferXZ[i] = 0.0;
    }
}

int findEdges(threeD *shape, Point *point){
    for(int i = 0; i < shape->vertices.size(); i++){
        if(shape->vertices[i] == point){
            return i;
        }
    }
    return -1;
}


void HalfTone(float pixBuff [], std::string plane){
    //group PixelBuffer colors into MegaPixels
    std::vector<std::vector<Point2D>> allMP;
    for(int j = 0; j < window_height / 2 - 2; j+= 3){
        for(int i = 0; i < window_width * 3/2 - 2; i+=9){
            Point2D pt1 = getPixel(j * window_width/2 * 3 + i, pixBuff);
            Point2D pt2 = getPixel(j * window_width/2 * 3 + (i+3), pixBuff);
            Point2D pt3 = getPixel(j * window_width/2 * 3 + (i+6), pixBuff);
            Point2D pt4 = getPixel((j + 1) * window_width/2 * 3 + i, pixBuff);
            Point2D pt5 = getPixel((j + 1) * window_width/2 * 3 + (i+3), pixBuff);
            Point2D pt6 = getPixel((j + 1) * window_width/2 * 3 + (i+6), pixBuff);
            Point2D pt7 = getPixel((j + 2) * window_width/2 * 3 + i, pixBuff);
            Point2D pt8 = getPixel((j + 2) * window_width/2 * 3 + (i+3), pixBuff);
            Point2D pt9 = getPixel((j + 2) * window_width/2 * 3 + (i+6), pixBuff);
            Point2D MParray [] = {pt1, pt2, pt3, pt4, pt5, pt6, pt7, pt8, pt9};
            std::vector<Point2D> MegaPixel(MParray, MParray + 9);
            allMP.push_back(MegaPixel);
            i+=9;
        }
        j+=3;
    }
    
    
    // modify megapixels to make one color
    for(auto megapix : allMP){
        
        Color colorSum = Color(0,0,0);
        
        for(auto pix : megapix){
            colorSum = colorSum + pix.color;
        }
        
        int NumOn = round(max(colorSum.r, max(colorSum.g, colorSum.g)));
        float total = colorSum.r + colorSum.g + colorSum.b;
        int numR = (total < 0.5) ? 0 : round(colorSum.r/total) * NumOn;
        int numG = (total < 0.5) ? 0 : round(colorSum.g/total) * NumOn;
        int numB = (total < 0.5) ? 0 : round(colorSum.b/total) * NumOn;
    
        
        for(auto pix : megapix){
            
            int pxCol = rand() % 4;
            
            if(pxCol == 0 && numR != 0){
                pix.color = Color(1,0,0);
                numR--;
            }
            if(pxCol == 1 && numG != 0){
                pix.color = Color(0,1,0);
                numG--;
            }
            if(pxCol == 2 && numB != 0){
                pix.color = Color(0,0,1);
                numB--;
            }
            if(pxCol == 4){
                pix.color = Color(0,0,0);
            }
        }
        
    }

    
    int pix_per_row= (window_width/2 * 3);
    int pix_per_col = (window_height/2);
    
    
    //set megapixels back into buffer
    if(plane == "xy"){
        int x = 0;
        int y = 0;
        for(auto megapix : allMP){
            if(x == pix_per_row){
                x = 0;
                y += 3;
            }
            HFsetPixXY(y * window_width / 2 * 3 + x, megapix[0].color);
            HFsetPixXY(y * window_width / 2 * 3 + (x+3), megapix[1].color);
            HFsetPixXY(y * window_width / 2 * 3 + (x+6), megapix[2].color);
            HFsetPixXY((y+1) * window_width / 2 * 3 + x, megapix[3].color);
            HFsetPixXY((y+1) * window_width / 2 * 3 + (x+3), megapix[4].color);
            HFsetPixXY((y+1) * window_width / 2 * 3 + (x+6), megapix[5].color);
            HFsetPixXY((y+2) * window_width / 2 * 3 + x, megapix[6].color);
            HFsetPixXY((y+2) * window_width / 2 * 3 + (x+3), megapix[7].color);
            HFsetPixXY((y+2) * window_width / 2 * 3 + (x+6), megapix[8].color);
            x+=9;
        }
    }
    else if(plane == "yz"){
        int x = 0;
        int y = 0;
        for(auto megapix : allMP){
            if(x == pix_per_row){
                x = 0;
                y += 3;
            }
            HFsetPixYZ(y * window_width / 2 * 3 + x, megapix[0].color);
            HFsetPixYZ(y * window_width / 2 * 3 + (x+3), megapix[1].color);
            HFsetPixYZ(y * window_width / 2 * 3 + (x+6), megapix[2].color);
            HFsetPixYZ((y+1) * window_width / 2 * 3 + x, megapix[3].color);
            HFsetPixYZ((y+1) * window_width / 2 * 3 + (x+3), megapix[4].color);
            HFsetPixYZ((y+1) * window_width / 2 * 3 + (x+6), megapix[5].color);
            HFsetPixYZ((y+2) * window_width / 2 * 3 + x, megapix[6].color);
            HFsetPixYZ((y+2) * window_width / 2 * 3 + (x+3), megapix[7].color);
            HFsetPixYZ((y+2) * window_width / 2 * 3 + (x+6), megapix[8].color);
            x+=9;
        }
        
    }
    else if(plane == "xz"){
        int x = 0;
        int y = 0;
        for(auto megapix : allMP){
            if(x == pix_per_row){
                x = 0;
                y += 3;
            }
            HFsetPixXZ(y * window_width / 2 * 3 + x, megapix[0].color);
            HFsetPixXZ(y * window_width / 2 * 3 + (x+3), megapix[1].color);
            HFsetPixXZ(y * window_width / 2 * 3 + (x+6), megapix[2].color);
            HFsetPixXZ((y+1) * window_width / 2 * 3 + x, megapix[3].color);
            HFsetPixXZ((y+1) * window_width / 2 * 3 + (x+3), megapix[4].color);
            HFsetPixXZ((y+1) * window_width / 2 * 3 + (x+6), megapix[5].color);
            HFsetPixXZ((y+2) * window_width / 2 * 3 + x, megapix[6].color);
            HFsetPixXZ((y+2) * window_width / 2 * 3 + (x+3), megapix[7].color);
            HFsetPixXZ((y+2) * window_width / 2 * 3 + (x+6), megapix[8].color);
            x+=9;
        }
        
    }
    
}


void interface(){
    std::string answer;
    std::vector<std::array<float, 2>>points;
    
    if(TURN_HALF_TONE_ON){
        std::cout << std::endl << "What would you like to do?" << std::endl <<
        "\t(1) Move viewsource" << std::endl <<
        "\t(2) Move lightsource" << std::endl <<
        "\t(3) Change light intensity" << std::endl <<
        "\t(4) Change ambient intensity" << std::endl <<
        "\t(5) Revert to Normal" << std::endl <<
        "\t(6) Exit" << std::endl;
    } else {
        std::cout << std::endl << "What would you like to do?" << std::endl <<
        "\t(1) Move viewsource" << std::endl <<
        "\t(2) Move lightsource" << std::endl <<
        "\t(3) Change light intensity" << std::endl <<
        "\t(4) Change ambient intensity" << std::endl <<
        "\t(5) Half tone" << std::endl <<
        "\t(6) Exit" << std::endl;
    }
    
    std::cout << ">> ";
    std::cin >> answer;
    
    if (answer == "1"){
        float x, y, z;
        std::cout << "Enter the coordinates of the position you would like to change the viewsource to (separated by spaces)" << std::endl;
        std::cin >> x >> y >> z;
        viewsource.x = x;
        viewsource.y = y;
        viewsource.z = z;
    }
    else if (answer == "2"){
        float x, y, z;
        std::cout << "Enter the coordinates of the position you would like to change the lightsource to (separated by spaces)" << std::endl;
        std::cin >> x >> y >> z;
        lightsource.x = x;
        lightsource.y = y;
        lightsource.z = z;
        
    }
    else if (answer == "3"){
        float r, g, b;
        std::cout << "Enter the color of the light in r,g,b format (separated by spaces)" << std::endl;
        std::cin >> r >> g >> b;
        lightIntensity.r = r;
        lightIntensity.g = g;
        lightIntensity.b = b;
    }
    else if (answer == "4"){
        float r, g, b;
        std::cout << "Enter the color of the light in r,g,b format (separated by spaces)" << std::endl;
        std::cin >> r >> g >> b;;
        ambientIntensity.r = r;
        ambientIntensity.g = g;
        ambientIntensity.b = b;
        
    }
    else if(answer == "5"){
        if(TURN_HALF_TONE_ON){
            TURN_HALF_TONE_ON = false;
        }
        else{
            TURN_HALF_TONE_ON = true;
        }
    }
    
    else if(answer == "6"){
        exit(0);
    }
    else {
        std::cout << "Invalid answer. Please try again" << std::endl << std::endl;
        interface();
    }

}

// display on XY subwindow
void displayXY(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    if(run_Phong_once){
        // compute big K value
        float vertexCount = 0;
        for(auto shape : all_shapes){
            for(int i = 0; i < shape->vertices.size(); i++){
                vertexCount++;
                bigK += Geometry::length(lightsource - *shape->vertices[i]);
            }
        }
        
        bigK /= vertexCount;
        
        // compute Phong for all vertices
        for(auto shape: all_shapes){
            shape->Phong();
        }
        
        maxRGB = findmaxRGB();
        if(maxRGB > 1){
            for(auto shape: all_shapes){
                for(auto vertex : shape->vertices){
                    vertex->color = vertex->color / maxRGB;
                }
            }
        }
        
        run_Phong_once = false;
    }
    
    
    for(auto shape : all_shapes){
        shape->findCentroid();
    }
    
    std::sort(all_shapes.begin(), all_shapes.end(), [](threeD *shape1, threeD *shape2){return shape1->centroid.z > shape2->centroid.z;});
    
    
    if(TURN_HALF_TONE_ON){
        HalfTone(PixelBufferXY, "xy");
        glDrawPixels(window_width/2, window_height/2, GL_RGB, GL_FLOAT, tmpXY);
    }
    else{
        for(auto shape : all_shapes){
            shape->drawXY();
        }
        glDrawPixels(window_width/2, window_height/2, GL_RGB, GL_FLOAT, PixelBufferXY);
    }
    
    
    //window refresh
    glFlush();
    glutPostRedisplay();
    
}

// display on YZ screen
void displayYZ(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    clearYZPixels();
    
    std::sort(all_shapes.begin(), all_shapes.end(), [](threeD *shape1, threeD *shape2){return shape1->centroid.x > shape2->centroid.x;});
    

    if(TURN_HALF_TONE_ON){
        HalfTone(PixelBufferYZ, "yz");
        glDrawPixels(window_width/2, window_height/2, GL_RGB, GL_FLOAT, tmpYZ);
        
    }
    else {
        for(auto i : all_shapes){
            i->drawYZ();
        }
        glDrawPixels(window_width/2, window_height/2, GL_RGB, GL_FLOAT, PixelBufferYZ);
    }
    
    
    glFlush();
    glutPostRedisplay();
}

// display on XZ screen
void displayXZ(){
    string answer;
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    clearXZPixels();
    
    if(display_count){
        interface();
    }
    
    std::sort(all_shapes.begin(), all_shapes.end(), [](threeD *shape1, threeD *shape2){return shape1->centroid.y > shape2->centroid.y;});
    
    
    if(TURN_HALF_TONE_ON){
        HalfTone(PixelBufferXZ, "xz");
        glDrawPixels(window_width/2, window_height/2, GL_RGB, GL_FLOAT, tmpXZ);
        
    }
    else {
        for(auto i : all_shapes){
            i->drawXZ();
        }
        glDrawPixels(window_width/2, window_height/2, GL_RGB, GL_FLOAT, PixelBufferXZ);
    }
    
    glDrawPixels(window_width/2, window_height/2, GL_RGB, GL_FLOAT, PixelBufferXZ);
    
    display_count = 1;
    
    glFlush();
    glutPostRedisplay();
    
}

void display(){
    glutSetWindow(MainWindow);
    glClear(GL_COLOR_BUFFER_BIT);
    
}

int main(int argc, char *argv[]){
    
    window_width = std::atoi(argv[1]);
    window_height = std::atoi(argv[2]);
    PixelBufferSize = window_width * window_height * 3;
    
    int numPoints, numFaces;
    std::string chars;
    
    std::ifstream inputFile("file.txt", std::ios::in);
    
    // exit program if unable to create file
    if(!inputFile){
        std::cerr << "File could not be opened" << std::endl;
    }
    
    std::vector<Point *>points;
    std::vector<Point *>normals;
    std::vector<Polygon *>faces;
    
    // read in file
    while(!inputFile.eof()){
        
        getline(inputFile, chars);
        numPoints = atoi(chars.c_str());
        
        points.clear();
        normals.clear();
        faces.clear();
        
        
        if(chars == ""){
            break;
        }
        
        // get vertices
        for(int i = 0; i < numPoints; i++){
            getline(inputFile, chars);
            std::stringstream iss;
            iss.str(chars);
            std::string val1, val2, val3;
            iss >> val1 >> val2 >> val3;
            Point *newPoint = new Point();
            newPoint->x = std::stof(val1);
            newPoint->y = std::stof(val2);
            newPoint->z = std::stof(val3);
            points.push_back(newPoint);
        }
        
        threeD *newShape = new threeD(points);
        
        // get normal vectors for each vertex
        for(int i = 0; i < numPoints; i++){
            getline(inputFile, chars);
            std::stringstream iss;
            iss.str(chars);
            std::string val1, val2, val3;
            iss >> val1 >> val2 >> val3;
            Point *normalvec = new Point();
            normalvec->x = std::stof(val1);
            normalvec->y = std::stof(val2);
            normalvec->z = std::stof(val3);
            normals.push_back(normalvec);
        }
        
        newShape->normals = normals;
        
        
        // get faces
        getline(inputFile, chars);
        numFaces = atoi(chars.c_str());
        
        for(int i = 0; i < numFaces; i++){
            getline(inputFile, chars);
            std::stringstream iss(chars);
            int index;
            std::vector<Point *>vertices;
            while(iss >> index){
                vertices.push_back(&(*newShape->vertices.at(index)));
            }
            Polygon *poly = new Polygon(vertices);
            poly->findCentroid();
            newShape->faces.push_back(poly);
        }
        
        
        // ka
        getline(inputFile, chars);
        std::stringstream iss(chars);
        float kar, kag, kab;
        iss >> kar >> kag >> kab;
        Color ka = {kar, kag, kab};
        newShape->ka = ka;
        
        // kd
        getline(inputFile, chars);
        std::stringstream is1(chars);
        float kdr, kdg, kdb;
        is1 >> kdr >> kdg >> kdb;
        Color kd = {kdr, kdg, kdb};
        newShape->kd = kd;
        
        // ks
        getline(inputFile, chars);
        std::stringstream is2(chars);
        float ksr, ksg, ksb;
        is2 >> ksr >> ksg >> ksb;
        Color ks = {ksr, ksg, ksb};
        newShape->ks = ks;
        
        // ns
        getline(inputFile, chars);
        float ns = std::stof(chars);
        newShape->ns = ns;
        all_shapes.push_back(newShape);
        
        getline(inputFile, chars);
        
        
    }
    
    
    //intialize each buffer to be 1/4 of main window size
    PixelBufferXY = new float[PixelBufferSize/4];
    PixelBufferYZ = new float[PixelBufferSize/4];
    PixelBufferXZ = new float[PixelBufferSize/4];
    tmpXY = new float[PixelBufferSize/4];
    tmpYZ = new float[PixelBufferSize/4];
    tmpXZ = new float[PixelBufferSize/4];
    
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    
    //set window size
    glutInitWindowSize(window_width, window_height);
    
    //set window position
    glutInitWindowPosition(100, 100);
    
    //create and set main window title
    MainWindow = glutCreateWindow("Hello Graphics!!");
    glutDisplayFunc(display);
    
    // subwindow winXY will be located in upper left of main window
    winXY = glutCreateSubWindow(MainWindow, 0, 0, window_width/2, window_height/2);
    glutDisplayFunc(displayXY);
    
    // subwindow winYZ will be located in upper right of main window
    winYZ = glutCreateSubWindow(MainWindow, window_width/2, 0, window_width/2, window_height/2);
    glutDisplayFunc(displayYZ);
    
    // subwindow winXZ will be located in bottom right of main window
    winXZ = glutCreateSubWindow(MainWindow, window_width/2, window_height/2, window_width/2, window_height/2);
    glutDisplayFunc(displayXZ);
    
    //clears the buffer of OpenGL
    glClearColor(0, 0, 0, 0);
    
    //main display loop, will display until terminate
    glutMainLoop();
    return 0;
}


