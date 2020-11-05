#include "boundary_verticies.h"
#include <igl/boundary_facets.h>
#include <igl/unique.h>

void boundary_verticies(const Eigen::MatrixXi& T, // tet mesh
                              Eigen::MatrixXi& F, // boundary faces
                              Eigen::VectorXi& b) // boundary vertex indicies
{
    igl::boundary_facets(T, F);
    igl::unique(F, b);  
}

void boundary_verticies(const Eigen::MatrixXi& T, // tet mesh
                              Eigen::VectorXi& b) // boundary vertex indicies
{   
    Eigen::MatrixXi F;
    boundary_verticies(T, F, b);
}

