//
//  Line.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/31/17.
//
//

#include "Line.hpp"

extern int pid;
extern float xmin, xmax, ymin, ymax;
enum{TOP=0x1,BOTTOM=0x2,RIGHT=0x4,LEFT=0x8};
void setPixelXY(int x, int y, double c);
void setPixelYZ(int x, int y, double c);
void setPixelXZ(int x, int y, double c);

Line::Line(Edge *edge){
    this->x1 = edge->p1->x;
    this->y1 = edge->p1->y;
    this->z1 = edge->p1->z;
    this->x2 = edge->p2->x;
    this->y2 = edge->p2->y;
    this->z2 = edge->p2->z;
    n = 2;
    name = "Line";
}

Line::Line(float x1, float y1, float x2, float y2, float z1, float z2){
    this->x1 = x1;
    this->y1 = y1;
    this->z1 = z1;
    this->x2 = x2;
    this->y2 = y2;
    this->z2 = z2;
    n = 2;
    name = "Line";
}


void Line::updateParameters() {
    this->x1 = vertices[0]->x;
    this->y1 = vertices[0]->y;
    this->z1 = vertices[1]->z;
    this->x2 = vertices[1]->x;
    this->y2 = vertices[1]->y;
    this->z2 = vertices[1]->z;
    n = 2;
}

// Assigns code to a vertex based on the clipping window size
unsigned int Line::code(float x,float y){
    unsigned int code=0;
    if(y>ymax)
        code|=TOP;
    if(y<ymin)
        code|=BOTTOM;
    if(x>xmax)
        code|=RIGHT;
    if(x<xmin)
        code|=LEFT;
    return code;
}

void Line::draw(){
//    lineDDA();
}

// draws a single line given a vertex pair and color according to DDA algorithm
void Line::lineDDA(float _x1, float _y1, float _x2, float _y2, std::string plane)
{
    float dX,dY,iSteps;
    float xInc,yInc,iCount,x,y;
    
    dX = _x1 - _x2;
    dY = _y1 - _y2;
    
    if (fabs(dX) > fabs(dY))
    {
        iSteps = fabs(dX);
    }
    else
    {
        iSteps = fabs(dY);
    }
    
    xInc = dX/iSteps;
    yInc = dY/iSteps;
    
    x = _x1;
    y = _y1;
    
    for (iCount=1; iCount<=iSteps; iCount++)
    {
        if (plane == "xy"){
            setPixelXY(floor(x),floor(y),color);
        }
        else if (plane == "yz"){
            setPixelYZ(floor(x),floor(y),color);
        }
        else if (plane == "xz"){
            setPixelXZ(floor(x),floor(y),color);
        }


        x -= xInc;
        y -= yInc;
    }
    return;
}


// draws a single line given a vertex pair and color according to Bresenham algorithm
void Line::lineBres(float _x1, float _y1, float _x2, float _y2, std::string plane){
    
    const bool steep = (fabs(_y2 - _y1) > fabs(_x2 - _x1));
    
    if(steep)
        // dy > dx, therefore slope > 1
    {
        std::swap(_x1, _y1);
        std::swap(_x2, _y2);
    }
    
    if(_x1 > _x2)
        // going from left to right, make sure left is smaller of two
    {
        std::swap(_x1, _x2);
        std::swap(_y1, _y2);
    }
    
    const float dx = _x2 - _x1;
    const float dy = fabs(_y2 - _y1);
    
    float error = dx / 2.0f;
    const int ystep = (_y1 < _y2) ? 1 : -1;
    int y = (int)_y1;
    
    const int maxX = (int)_x2;
    
    for(int x=(int)_x1; x<maxX; x++)
    {
        if(steep)
        {
            if(plane == "xy"){
                setPixelXY(y, x, color);
            }
            else if(plane == "yz"){
                setPixelYZ(y, x, color);
            }
            else if(plane == "xz"){
                setPixelYZ(y, x, color);
            }
        }
        else
        {
            if(plane == "xy"){
                setPixelXY(x, y, color);
            }
            else if(plane == "xy"){
                setPixelXY(x, y, color);
            }
            else if(plane == "xz"){
                setPixelXZ(x, y, color);
            }

        }
        
        error -= dy;
        if(error < 0)
        {
            y += ystep;
            error += dx;
        }
    }
}


int Line::clip(){
    unsigned int outcode0,outcode1;
    bool accept = false, done = false;
    
    float _x1 = x1;
    float _x2 = x2;
    float _y1 = y1;
    float _y2 = y2;
    
    
    outcode0 = code(_x1,_y1);
    outcode1 = code(_x2,_y2);
    
    do
    {
        if(outcode0 == 0 && outcode1==0)
        {
            accept = true;
            done = true;
        }
        else if(outcode0 & outcode1)
        {
            done = true;
        }
        else
        {
            float x,y;
            int ocd=outcode0 ? outcode0 : outcode1;
            if(ocd & TOP)
            {
                x=_x1+(_x2-_x1)*(ymax-_y1)/(_y2-_y1);
                y=ymax;
            }
            else if(ocd & BOTTOM)
            {
                x=_x1+(_x2-_x1)*(ymin-_y1)/(_y2-_y1);
                y=ymin;
            }
            else if(ocd & LEFT)
            {
                y=_y1+(_y2-_y1)*(xmin-_x1)/(_x2-_x1);
                x=xmin;
            }
            else
            {
                y=_y1+(_y2-_y1)*(xmax-_x1)/(_x2-_x1);
                x=xmax;
            }
            if(ocd==outcode0)
            {
                _x1=x;
                _y1=y;
                outcode0=code(_x1,_y1);
            }
            else
            {
                _x2=x;
                _y2=y;
                outcode1=code(_x2,_y2);
            }
        }
        
    }
    while(done == false);
    
    if(accept == true){
        x1 = _x1;
        y1 = _y1;
        x2 = _x2;
        y2 = _y2;
        return 1;
    }
    return -1;
}
    
