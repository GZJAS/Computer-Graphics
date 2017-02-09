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
    std::vector<double >z_row;
    for(auto vertex : vertices){
        x_row.push_back(vertex->x);
        y_row.push_back(vertex->y);
        z_row.push_back(vertex->z);
    }
    
    std::vector<double >bottom_row;
    for(int i = 0; i < vertices.size(); i++){
        bottom_row.push_back(1);
    }
    
    matrix.push_back(x_row);
    matrix.push_back(y_row);
    matrix.push_back(z_row);
    matrix.push_back(bottom_row);
}

void Geometry::convertToVector(){
    int numPoints = (int)matrix[0].size();
    for(int i = 0; i < numPoints; i++){
        vertices[i]->x = matrix[0][i];
        vertices[i]->y = matrix[1][i];
        vertices[i]->z = matrix[2][i];
    }
}

void Geometry::findCentroid(){
    double sumx = 0, sumy = 0, sumz = 0;
    
    for (auto i : vertices){
        sumx += i->x;
        sumy += i->y;
        sumz += i->z;
    }
    
    centroid.x = sumx/vertices.size();
    centroid.y = sumy/vertices.size();
    centroid.z = sumz/vertices.size();
}


// multiply two matrices together
std::vector<std::vector<double>> Geometry::matrixMultiply(std::vector<std::vector<double>> matrixA, std::vector<std::vector<double>> matrixB){
    
    std::vector<std::vector<double>> matrixC;
    int r1 = (int)matrixA.size();
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
std::vector<std::vector<double>> Geometry::create_rot_matrix(Edge *rotaxis, double angle){
    //  |   1 - 2b^2 - 2c^2       2ab - 2sc         2ac + 2sb       0   |
    //  |   2ab + 2sc           1 - 2a^2 - 2c^2     2bc - 2sa       0   |
    //  |   2ac - 2sb             2bc + 2sa      1 - 2a^2 - 2b^2    0   |
    //  |       0                     0                 0           1   |
    
    std::vector<std::vector<double>> rot_matrix;
    double magnitude, x_squared, y_squared, z_squared;

    float x_diff = rotaxis->p2->x - rotaxis->p1->x;
    float y_diff = rotaxis->p2->y - rotaxis->p1->y;
    float z_diff = rotaxis->p2->z - rotaxis->p1->z;

    x_squared = pow(x_diff, 2);
    y_squared = pow(y_diff, 2);
    z_squared = pow(z_diff, 2);
    magnitude = sqrt(x_squared + y_squared +z_squared);
    
    float a = x_diff/magnitude;
    float b = y_diff/magnitude;
    float c = z_diff/magnitude;
    float s = cos(angle/2);
    
    double m11 = 1 - 2 * pow(b, 2) - 2 * pow(c, 2);
    double m12 = 2 * a * b - 2 * s * c;
    double m13 = 2 * a * c + 2 * s * b;
    double m14 = 0.0;
    double m21 = 2 * a * b + 2 * s * c;
    double m22 = 1 - 2 * pow(a, 2) - 2 * pow(c, 2);
    double m23 = 2 * b * c - 2 * s * a;
    double m24 = 0.0;
    double m31 = 2 * a * c - 2 * s * b;
    double m32 = 2 * b * c + 2 * s * a;
    double m33 = 1 - 2 * pow(a, 2) - 2 * pow(b, 2);
    double m34 = 0.0;
    double m41 = 0.0;
    double m42 = 0.0;
    double m43 = 0.0;
    double m44 = 1.0;
    
    rot_matrix = {{m11, m12, m13, m14},{m21, m22, m23, m24},{m31, m32, m33, m34},{m41, m42, m43, m44}};
    
    return rot_matrix;
    
}

// return a matrix with given scaling parameters
std::vector<std::vector<double>> Geometry::create_scale_matrix(double Sx, double Sy, double Sz){
    //  |   Sx      0       0       0   |
    //  |   0       Sy      0       0   |
    //  |   0       0       Sz      0   |
    //  |   0       0       0       1   |
    
    std::vector<std::vector<double>> scale_matrix;
    
    std::vector<double>row = {Sx, 0.0, 0.0, 0.0};
    scale_matrix.push_back(row);
    
    row = {0.0, Sy, 0.0, 0.0};
    scale_matrix.push_back(row);
    
    
    row = {0.0, 0.0, Sz, 0.0};
    scale_matrix.push_back(row);
    
    
    row = {0.0, 0.0, 0.0, 1.0};
    scale_matrix.push_back(row);
    
    return scale_matrix;
    
}

// return a matrix with given tranlate parameters
std::vector<std::vector<double>> Geometry::create_trans_matrix(double x, double y, double z){
    //  |   1       0       0       x   |
    //  |   0       1       0       y   |
    //  |   0       0       1       z   |
    //  |   0       0       0       1   |

    std::vector<std::vector<double>> trans_matrix;
    std::vector<double>row = {1.0, 0.0, 0.0, x};
    trans_matrix.push_back(row);
    
    row = {0.0, 1.0, 0.0, y};
    trans_matrix.push_back(row);
    
    row = {0.0, 0.0, 1.0, y};
    trans_matrix.push_back(row);
    
    row = {0.0, 0.0, 0.0, 1.0};
    trans_matrix.push_back(row);
    
    return trans_matrix;
    
}


// translate a polygon or line by (x, y, z) !!! NOT TO (x, y, z)
void Geometry::translate(double x, double y, double z){
    
    std::vector<std::vector<double>> trans_matrix;
    std::vector<std::vector<double>> cpymatrix;
    std::vector<std::vector<double>> result_matrix;
    
    // get matrix for translating
    trans_matrix = create_trans_matrix(x, y, z);
    
    // copy matrix
    cpymatrix = matrix;
    
    //multiply original matrix by translation matrix
    result_matrix = matrixMultiply(trans_matrix, cpymatrix);
    
    // copy back into matrix
    matrix = result_matrix;
    
    // update vertices
    convertToVector();
    
}

void Geometry::scale(double Sx, double Sy, double Sz){
    std::vector<std::vector<double>> scale_matrix;
    std::vector<std::vector<double>> cpymatrix;
    std::vector<std::vector<double>> trans_matrix;
    std::vector<std::vector<double>> result_matrix;
    std::vector<std::vector<double>> rev_trans_matrix;

    // find centroid and set coordinates
    findCentroid();

    // matrix to translate polygon/line's centroid by (-cx, -cy, -cz)
    rev_trans_matrix = create_trans_matrix(-centroid.x, -centroid.y, -centroid.z);

    // get matrix for rotating polygon
    scale_matrix = create_scale_matrix(Sx, Sy, Sz);

    // matrix to translate polygon/line's centroid back to (cx, cy, cz)
    trans_matrix = create_trans_matrix(centroid.x, centroid.y, centroid.z);


    // copy matrix
    cpymatrix = matrix;

    // carry out operations for rotating: translate, rotate, and translate back
    result_matrix = matrixMultiply(rev_trans_matrix, cpymatrix);
    result_matrix = matrixMultiply(scale_matrix, result_matrix);
    result_matrix = matrixMultiply(trans_matrix, result_matrix);

    // copy back into matrix
    matrix = result_matrix;
    
    // update vertices
    convertToVector();

}


// rotate polygon or line by alpha
void Geometry::rotate(Edge * rotaxis, double angle){
    std::vector<std::vector<double>> rot_matrix;
    std::vector<std::vector<double>> cpymatrix;
    std::vector<std::vector<double>> trans_matrix;
    std::vector<std::vector<double>> result_matrix;
    std::vector<std::vector<double>> rev_trans_matrix;
    
    // find centroid and set coordinates
    findCentroid();
    
    // matrix to translate polygon/line's centroid by (-cx, -cy)
    rev_trans_matrix = create_trans_matrix(-centroid.x, -centroid.y, -centroid.z);
    
    // get matrix for rotating polygon
    rot_matrix = create_rot_matrix(rotaxis, angle);
    
    // matrix to translate polygon/line's centroid back to (cx, cy)
    trans_matrix = create_trans_matrix(centroid.x, centroid.y, centroid.z);
    
    
    // copy matrix
    cpymatrix = matrix;
    
    // carry out operations for rotating: translate, rotate, and translate back
    result_matrix = matrixMultiply(rev_trans_matrix, cpymatrix);
    result_matrix = matrixMultiply(rot_matrix, result_matrix);
    result_matrix = matrixMultiply(trans_matrix, result_matrix);
    
    // copy back into matrix
    matrix = result_matrix;
    
    // update vertices
    convertToVector();
    
}



