//
//  Geometry.cpp
//  Simple_2D_Drawing_System
//
//  Created by Dimitar Vasilev on 1/31/17.
//
//

#include "Geometry.hpp"

// convert vertices vector into matrix (vector of vectors)
void Geometry::convertToMatrix(){
    std::vector<double>x_row;
    std::vector<double>y_row;
    for(auto vertex : vertices){
        x_row.push_back(static_cast<double>(vertex->x));
        y_row.push_back(static_cast<double>(vertex->y));
    }
    
    std::vector<double>bottom_row;
    for(int i = 0; i < n; i++){
        bottom_row.push_back(1);
    }
    
    matrix.push_back(x_row);
    matrix.push_back(y_row);
    matrix.push_back(bottom_row);
}

void Geometry::convertToVector(){
    int numPoints = (int)matrix[0].size();
    vertices.clear();
    for(int i = 0; i < numPoints; i++){
        Point *pt = new Point();
        pt->x = matrix[0][i];
        pt->y = matrix[1][i];
        vertices.push_back(pt);
    }
}

// find centroid of polygon
// http://stackoverflow.com/questions/2792443/finding-the-centroid-of-a-polygon
void Geometry::findCentroid()
{
    centroid = {0, 0};
    float signedArea = 0.0;
    float x0 = 0.0; // Current vertex X
    float y0 = 0.0; // Current vertex Y
    float x1 = 0.0; // Next vertex X
    float y1 = 0.0; // Next vertex Y
    float a = 0.0;  // Partial signed area
    
    // For all vertices
    for (int i = 0; i < vertices.size(); ++i)
    {
        x0 = vertices[i]->x;
        y0 = vertices[i]->y;
        x1 = vertices[(i+1) % n]->x;
        y1 = vertices[(i+1) % n]->y;
        a = x0*y1 - x1*y0;
        signedArea += a;
        centroid.x += (x0 + x1)*a;
        centroid.y += (y0 + y1)*a;
    }
    
    signedArea *= 0.5;
    centroid.x /= (6.0*signedArea);
    centroid.y /= (6.0*signedArea);
    
}

// multiply two matrices together
std::vector<std::vector<double>> Geometry::matrixMultiply(std::vector<std::vector<double>> matrixA, std::vector<std::vector<double>> matrixB){
    
    std::vector<std::vector<double>> matrixC;
    int r1 = (int)matrixA.size();       // 3
    int c1 = (int)matrixA[0].size();
    int c2 = n;
    
    std::vector<double>row;
    for(int i = 0; i < n; i++){
        row.push_back(0);
    }
    for(int i = 0; i < matrixA.size(); i++){
        matrixC.push_back(row);
    }
    
    
    for(int i = 0; i < r1; i++){
        for (int j = 0; j < c2; j++){
            for(int k = 0; k < c1; k++){
                matrixC.at(i).at(j) += matrixA.at(i).at(k) * matrixB.at(k).at(j);
            }
        }
    }
    
    return matrixC;
}


// return a matrix with given rotate parameters
std::vector<std::vector<double>> Geometry::create_rot_matrix(double alpha){
    // rot matrix in the form of:
    //        |   cost(alpha)   -sin(alpha)     0   |
    //        |   sin(alpha)    cos(alpha)beta  0   |
    //        |   0                 0           1   |
    std::vector<std::vector<double>> rot_matrix;
    std::vector<double>row = {cos(alpha * M_PI/180), -sin(alpha * M_PI/180), 0.0};
    rot_matrix.push_back(row);
    
    row = {sin(alpha * M_PI/180), cos(alpha * M_PI/180), 0.0};
    rot_matrix.push_back(row);
    
    row = {0.0, 0.0, 1.0};
    rot_matrix.push_back(row);
    
    return rot_matrix;
    
}

// return a matrix with given scaling parameters
std::vector<std::vector<double>> Geometry::create_scale_matrix(double alpha, double beta){
    // scale matrix in the form of:
    //        |    alpha   0       0   |
    //        |    0       beta    0   |
    //        |    0       0       1   |
    std::vector<std::vector<double>> scale_matrix;
    std::vector<double>row = {alpha, 0.0, 0.0};
    scale_matrix.push_back(row);
    
    row = {0.0, beta, 0.0};
    scale_matrix.push_back(row);
    
    row = {0.0, 0.0, 1.0};
    scale_matrix.push_back(row);
    
    return scale_matrix;
    
}

// return a matrix with given tranlate parameters
std::vector<std::vector<double>> Geometry::create_trans_matrix(double x, double y){
    // translate matrix in the form of:
    //        |     1     0      x    |
    //        |     0     1      y    |
    //        |     0     0      1    |
    std::vector<std::vector<double>> trans_matrix;
    std::vector<double>row = {1.0, 0.0, x};
    trans_matrix.push_back(row);
    
    row = {0.0, 1.0, y};
    trans_matrix.push_back(row);
    
    row = {0.0, 0.0, 1.0};
    trans_matrix.push_back(row);
    
    return trans_matrix;
    
}


// translate a polygon or line by (x, y) !!! NOT TO (x, y)
void Geometry::translate(int x, int y){
    
    std::vector<std::vector<double>> trans_matrix;
    std::vector<std::vector<double>> cpymatrix;
    std::vector<std::vector<double>> result_matrix;
    
    // get matrix for translating
    trans_matrix = create_trans_matrix(x, y);
    
    // copy matrix
    cpymatrix = matrix;
    
    //multiply original matrix by translation matrix
    result_matrix = matrixMultiply(trans_matrix, cpymatrix);
    
    // copy back into matrix
    matrix = result_matrix;
    
    
}

// rotate polygon or line by alpha
void Geometry::rotate(double alpha){
    std::vector<std::vector<double>> rot_matrix;
    std::vector<std::vector<double>> cpymatrix;
    std::vector<std::vector<double>> trans_matrix;
    std::vector<std::vector<double>> result_matrix;
    std::vector<std::vector<double>> rev_trans_matrix;
    
    // find centroid and set coordinates
    findCentroid();
    
    // matrix to translate polygon/line's centroid by (-cx, -cy)
    rev_trans_matrix = create_trans_matrix(-centroid.x, -centroid.y);
    
    // get matrix for rotating polygon
    rot_matrix = create_rot_matrix(alpha);
    
    // matrix to translate polygon/line's centroid back to (cx, cy)
    trans_matrix = create_trans_matrix(centroid.x, centroid.y);
    
    
    // copy matrix
    cpymatrix = matrix;
    
    // carry out operations for rotating: translate, rotate, and translate back
    result_matrix = matrixMultiply(rev_trans_matrix, cpymatrix);
    result_matrix = matrixMultiply(rot_matrix, result_matrix);
    result_matrix = matrixMultiply(trans_matrix, result_matrix);
    
    // copy back into matrix
    matrix = result_matrix;
    
}

void Geometry::scale(double alpha, double beta){
    std::vector<std::vector<double>> scale_matrix;
    std::vector<std::vector<double>> cpymatrix;
    std::vector<std::vector<double>> trans_matrix;
    std::vector<std::vector<double>> result_matrix;
    std::vector<std::vector<double>> rev_trans_matrix;
    
    // find centroid and set coordinates
    findCentroid();
    
    // matrix to translate polygon/line's centroid by (-cx, -cy)
    rev_trans_matrix = create_trans_matrix(-centroid.x, -centroid.y);
    
    // get matrix for rotating polygon
    scale_matrix = create_scale_matrix(alpha, beta);
    
    // matrix to translate polygon/line's centroid back to (cx, cy)
    trans_matrix = create_trans_matrix(centroid.x, centroid.y);
    
    
    // copy matrix
    cpymatrix = matrix;
    
    // carry out operations for rotating: translate, rotate, and translate back
    result_matrix = matrixMultiply(rev_trans_matrix, cpymatrix);
    result_matrix = matrixMultiply(scale_matrix, result_matrix);
    result_matrix = matrixMultiply(trans_matrix, result_matrix);
    
    // copy back into matrix
    matrix = result_matrix;
    
}

