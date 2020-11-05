#pragma once 

#include <Eigen/Core>

inline void 
color_matrix(int rows, const Eigen::Vector3d& cv, Eigen::MatrixXd& uC)
{
    uC.resize(rows, 3);
    for (int e = 0; e<uC.rows(); ++e) 
    {
        uC.row(e) = cv;
    }
}