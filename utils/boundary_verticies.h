#pragma once

#include <Eigen/Core>

void boundary_verticies(const Eigen::MatrixXi& T, // tet mesh
                              Eigen::MatrixXi& F, // boundary faces
                              Eigen::VectorXi& b); // boundary vertex indicies

void boundary_verticies(const Eigen::MatrixXi& T, // tet mesh
                              Eigen::VectorXi& b); // boundary vertex indicies

