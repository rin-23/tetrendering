#pragma once

#include <Eigen/Core>


// split tets in TT into triangles so you can visualize them
// output triangular mesh with colors per vertex or per face
void 
triangulate_tets(const Eigen::MatrixXi& TT,  // tets #T by 4
                 const Eigen::MatrixXd& TV,  // tet verticies #TV by 3
                 const Eigen::MatrixXd& TC,  // (optional) per vertex or per-tet colors #T|#TV by 3
                 const Eigen::MatrixXi& TI,  // (optional) only use subset of TT. If 1 te
                 const Eigen::MatrixXd& TD,  // #TV by 3 diffusion values per vertex
                 Eigen::MatrixXd& V,         // resulting verticies #V by 3
                 Eigen::MatrixXi& F,         // resulting triangular faces #F by 3
                 Eigen::MatrixXd& C,         // (optional if TC is not empty) resulting per-vertex or per face colors #F|#V by 3
                 Eigen::MatrixXd& D,         // #V by 3 diffusion values per vertex
                 Eigen::VectorXi& FtoT,      // Face to tet ID match
                 bool perfacecolor)  ;        // (ignored if TC is empty) if true then TC and C will contain per face colors, otherwise per-vertex


// split tets in TT into triangles so you can visualize them
// output triangular mesh with colors per vertex or per face
void    triangulate_tets(const Eigen::MatrixXi& TT,  // tets #T by 4
                         const Eigen::MatrixXd& TV,  // tet verticies #TV by 3
                         Eigen::MatrixXd& V,         // resulting verticies #V by 3
                         Eigen::MatrixXi& F,         // resulting triangular faces #F by 3
                         Eigen::VectorXi& FtoT);      // Face to tet ID match
