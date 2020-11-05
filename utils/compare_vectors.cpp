#include "compare_vectors.h"
#include <algorithm>

bool 
compare_vectors(const Eigen::VectorXi& b1, 
                const Eigen::VectorXi& b2,
                bool sort) 
{   
    if (b1.size() != b2.size()) 
        return false;

    if (sort) 
    {
        Eigen::VectorXi _b1=b1; 
        Eigen::VectorXi _b2=b2;

        std::sort(_b1.data(),_b1.data()+_b1.size());
        std::sort(_b2.data(),_b2.data()+_b2.size());

        return _b1 == _b2;
    }
    else 
    {
        return b1 == b2;
    }
}