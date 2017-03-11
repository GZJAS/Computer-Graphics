//
//  Line.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/31/17.
//
//

#include "Line.hpp"

extern float xmin, xmax, ymin, ymax;
extern int window_width, window_height;
void setPixel(int x, int y, Color c);


Line::Line(Point p1, Point p2){
    this->p1 = p1;
    this->p2 = p2;
}
// draws a single line given a vertex pair and color according to DDA algorithm
void Line::lineDDA()
{
    float dX,dY,iSteps;
    float xInc,yInc,iCount,x,y;
    
    dX = p1.x - p2.x;
    dY = p1.y - p2.y;

    
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
    
    
    x = p1.x;
    y = p1.y;
    
    
    for (iCount=0; iCount<=iSteps; iCount++)
    {

        setPixel(x, y, color);

        x -= xInc;
        y -= yInc;
    }
    return;
}


// draws a single line given a vertex pair and color according to Bresenham algorithm
void Line::lineBres(){

    
    float _x1 = p1.x;
    float _x2 = p2.x;
    float _y1 = p1.y;
    float _y2 = p2.y;
    
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
            setPixel(y, x, color);
        }
        else
        {
            setPixel(x, y, color);
        }
        
        error -= dy;
        if(error < 0)
        {
            y += ystep;
            error += dx;
        }
    }
}

