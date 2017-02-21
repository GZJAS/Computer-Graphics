//
//  Color.h
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 2/18/17.
//
//

#ifndef Color_h
#define Color_h
struct Color {
    float r;
    float g;
    float b;
    
    Color(float r, float g, float b): r(r), g(g), b(b) {}
    
    Color(float f): r(f), g(f), b(f){}
    
    Color(){}
    
    float getAverage(Color color){
        return color.r + color.g + color.b / 3;
    }
};


#endif /* Color_h */
