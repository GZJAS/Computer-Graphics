//
//  Polygon.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/31/17.
//
//

#include "Polygon.hpp"
extern int global_id;
extern int resolution;


// constructor
Polygon::Polygon(std::vector<Point > points){
    control_pts = points;
    id = global_id++;
}

// calculator points to draw lines between
void Polygon::Setup(){
    if(type == Bezier){
        deCasteljau();
    }
    else if(type == Spline){
        deBoor();
    }
}

// draw lines
void Polygon::Draw(){
    if(type == Bezier){
        for(int i = 0; i < bezier_pts.size() - 1; i++){
            Line line(bezier_pts.at(i), bezier_pts.at(i+1));
            line.color = color;
            line.lineDDA();
        }

    }
    if(type == Spline){
        for(int i = 0; i < boor_pts.size() - 1; i++){
            Line line(boor_pts.at(i), boor_pts.at(i+1));
            line.color = color;
            line.lineDDA();
        }
    }
}


int Polygon::WhichInterval(double u, int start){

    for(int i=start;i<=knots.size()-1;i++)
    {
        if(u >= knots.at(i) && knots.at(i+1) >= u)
            return i;
    }
    return -1;
}


Point Polygon::getBoorPt(float u, int I){
    std::vector<std::vector<Point>>tmp;
    tmp.push_back(control_pts);
    for(int j = 1; j <= k-1; j++){
        std::vector<Point>gen;
        for(int i = I-(k-1); i <= (I-j); i++){
            while(gen.size()!= i){
                gen.push_back(Point());
            }
            double a = (knots.at(i + k) - u) / (knots.at(i + k) - knots.at(i + j));
            double b = (u - knots.at(i + j)) / (knots.at(i + k) - knots.at(i + j));
            Point newpt = tmp.at(j-1).at(i) * a + tmp.at(j-1).at(i+1) * b;
            gen.push_back(newpt);
        }
        
        tmp.push_back(gen);

    }
    
    return tmp.at(tmp.size()-1).at(tmp.at(tmp.size()-1).size()-1);

}



// create B-spline curve
void Polygon::deBoor(){
    
    boor_pts.clear();
    
    float n = control_pts.size() - 1;
    double  domain_start = knots.at(k - 1);
    double domain_end = knots.at(n + 1);
    
    double step = ( domain_end - domain_start) / (resolution - 1);
    
    for(double u = domain_start; u <= domain_end; u+= step){
        int I = WhichInterval(u, floor(domain_start));
        Point pt = getBoorPt(u, I);
        boor_pts.push_back(pt);
    }
 }
    

void Polygon::createKnotVector(){
    int knotSize = (int)(k + control_pts.size());
    knots.clear();
    
    for(int count = 0; count < knotSize; count++){
        knots.push_back((float)count);
    }
    
}

// calculate Bezier point given t
Point Polygon::getBzrPt(float t){
    Point pt;
    std::vector<Point >tmp = control_pts;
    int j = (int)control_pts.size() - 1;
    while(j > 0){
        for(int i = 0; i < j; i++){
            tmp.at(i) = tmp.at(i) * (1 - t) + tmp.at(i + 1) * t;
        }
        j--;
    }

    
    return tmp.at(0);
}

// create Bezier curve
void Polygon::deCasteljau(){
    bezier_pts.clear();
    
    double step = ((double)1)/(resolution - 1);
    
    // calculate all Bezier points
    for(float i = 0; i <= 1; i += step ){

        Point pt = getBzrPt(i);
        bezier_pts.push_back(pt);
    }
    
}

void Polygon::updateCurve(){
    int n = (int)control_pts.size() - 1;
    if (knots.size() == k + n + 1){
        return;
    }
    
    while(knots.size() != k + n + 1){
        if(knots.size() > k + n + 1){
            knots.pop_back();
        }
        else if(knots.size() < k + n + 1){
            float f = knots.at(knots.size() - 1);
            knots.push_back(f + 1.0);
        }
    }
    
}

void Polygon::setKValue(int newk){
    this->k = newk;
    updateCurve();
}

bool Polygon::modifyKnot(int i, float f){
    if(knots.at(i - 1) <= f && knots.at(i+1) >= f){
        knots.at(i) = f;
        return true;
    }
    return false;
}

void Polygon::insertControlPt(int i, Point pt){
    control_pts.insert(control_pts.begin() + i, pt);
    if(type == Spline){
        updateCurve();
    }
}

void Polygon::modifyControlPt(int i, Point pt){
    control_pts.at(i).x = pt.x;
    control_pts.at(i).y = pt.y;
    
}
void Polygon::deleteControlPt(int i){
    control_pts.erase(control_pts.begin() + i);
    if(type == Spline){
        updateCurve();
    }
}


