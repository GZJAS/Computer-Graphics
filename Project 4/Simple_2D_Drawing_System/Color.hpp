//
//  Color.h
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 3/4/17.
//
//

#ifndef Color_h
#define Color_h
#include <iostream>
#include <cstdlib>

struct Color {
    float r;
    float g;
    float b;
    
    Color(float r, float g, float b): r(r), g(g), b(b) {}
    
    Color(float f): r(f), g(f), b(f){}
    
    Color(){}
    
    void print(){
        std::cout << "color = " << r << "," << g << "," << b << std::endl;
    }
    
    
    void setColor(float c){
        this->r = c;
        this->g = c;
        this->b = c;
    }
    
    void setColor(float r, float g, float b){
        this->r = r;
        this->g = g;
        this->b = b;
    }
    
    float getAverage(){
        return r + g + b / 3;
    }
    
    
    Color operator*(const float &c){
        Color newColor;
        newColor.r = this->r * c;
        newColor.g = this->g * c;
        newColor.b = this->b * c;
        
        return newColor;
    }
    
    Color operator*(const Color &c){
        Color newColor;
        newColor.r = this->r * c.r;
        newColor.g = this->g * c.g;
        newColor.b = this->b * c.b;
        
        return newColor;
    }
    
    Color operator/(const float &c){
        Color newColor;
        newColor.r = this->r / c;
        newColor.g = this->g / c;
        newColor.b = this->b / c;
        return newColor;
    }
    
    Color operator+(const Color &color){
        Color newColor;
        newColor.r = this->r + color.r;
        newColor.g = this->g + color.g;
        newColor.b = this->b + color.b;
        return newColor;
    }
    
    bool operator<(const float &c){
        if (this->r < c){
            return true;
        }
        if (this->g < c){
            return true;
        }
        if (this->b < c){
            return true;
        }
        return false;
    }
    
    
};


#endif /* Color_h */
