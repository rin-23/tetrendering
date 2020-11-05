#pragma once

#include <Eigen/Core>

bool 
compare_vectors(const Eigen::VectorXi& b1, 
                const Eigen::VectorXi& b2,
                bool sort=true);