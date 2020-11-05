
#include "triangulate_tets.h"

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
                 bool perfacecolor)          // (ignored if TC is empty) if true then TC and C will contain per face colors, otherwise per-vertex
{
    using namespace std;
    using namespace Eigen;
    assert(TI.size()==0);

    if (TI.size()==0) 
    {
        int numtets = TT.rows();
        V.resize(numtets*4,3);

        if (TD.size()>0) 
            D.resize(V.rows(), TD.cols());
        
        // if (perfacecolor)
        if (TC.size() > 0)
            C.resize(numtets*4,3);
        // else
            // C.resize(numtets*4,3);
        
        F.resize(numtets*4,3);
        FtoT.resize(F.rows());
            
        for (unsigned i=0; i<numtets;++i)
        {
            V.row(i*4+0) = TV.row(TT(i,0));
            V.row(i*4+1) = TV.row(TT(i,1));
            V.row(i*4+2) = TV.row(TT(i,2));
            V.row(i*4+3) = TV.row(TT(i,3));

            F.row(i*4+0) << (i*4)+1, (i*4)+2, (i*4)+3; 
            F.row(i*4+1) << (i*4)+3, (i*4)+2, (i*4)+0;
            F.row(i*4+2) << (i*4)+0, (i*4)+1, (i*4)+3;
            F.row(i*4+3) << (i*4)+0, (i*4)+2, (i*4)+1;

            // bottom trianlge with 2 on the top
            //       2
            //      / \  
            //     /   \
            //    /     \
            //   /       \
            //  0---------1

            FtoT(i*4+0) = i;
            FtoT(i*4+1) = i;
            FtoT(i*4+2) = i;
            FtoT(i*4+3) = i;
            
            if (TD.size()>0) 
            {
                D.row(i*4+0) = TD.row(TT(i,0));
                D.row(i*4+1) = TD.row(TT(i,1));
                D.row(i*4+2) = TD.row(TT(i,2));
                D.row(i*4+3) = TD.row(TT(i,3)); 
            }

            if (TC.size() > 0 && i < TC.rows()) 
            {
                if (perfacecolor) 
                {
                    C.row(i*4+0) = TC.row(i);
                    C.row(i*4+1) = TC.row(i);
                    C.row(i*4+2) = TC.row(i);
                    C.row(i*4+3) = TC.row(i);        
                }
                else 
                {
                    C.row(i*4+0) = TC.row(TT(i,0));
                    C.row(i*4+1) = TC.row(TT(i,1));
                    C.row(i*4+2) = TC.row(TT(i,2));
                    C.row(i*4+3) = TC.row(TT(i,3));
                }
            }

            // F.row(i*4+0) << (i*4)+0, (i*4)+1, (i*4)+3;
            // F.row(i*4+1) << (i*4)+0, (i*4)+2, (i*4)+1;
            // F.row(i*4+2) << (i*4)+3, (i*4)+2, (i*4)+0;
            // F.row(i*4+3) << (i*4)+1, (i*4)+2, (i*4)+3;

            

        }  
    }
    else 
    {   
        assert(false);
        int numtets = TI.rows();
        V.resize(numtets*4,3);
        // if (perfacecolor)
        if (TC.size() > 0)
            C.resize(numtets*4,3);
        // else
            // C.resize(numtets*4,3);
        
        F.resize(numtets*4,3);
            
        for (unsigned i=0; i<numtets;++i)
        {   
            auto rowidx = TI(i);
            auto v = TT.row(rowidx);
            V.row(i*4+0) = TV.row(v(0));
            V.row(i*4+1) = TV.row(v(1));
            V.row(i*4+2) = TV.row(v(2));
            V.row(i*4+3) = TV.row(v(3));

            D.row(i*4+0) = TD.row(TT(i,0));
            D.row(i*4+1) = TD.row(TT(i,1));
            D.row(i*4+2) = TD.row(TT(i,2));
            D.row(i*4+3) = TD.row(TT(i,3));

            if (TC.size() > 0 && rowidx < TC.rows()) 
            {
                if (perfacecolor) 
                {
                    C.row(i*4+0) = TC.row(rowidx);
                    C.row(i*4+1) = TC.row(rowidx);
                    C.row(i*4+2) = TC.row(rowidx);
                    C.row(i*4+3) = TC.row(rowidx);        
                }
                else 
                {
                    C.row(i*4+0) = TC.row(v(0));
                    C.row(i*4+1) = TC.row(v(1));
                    C.row(i*4+2) = TC.row(v(2));
                    C.row(i*4+3) = TC.row(v(3));
                }
            }

            F.row(i*4+0) << (i*4)+0, (i*4)+1, (i*4)+3;
            F.row(i*4+1) << (i*4)+0, (i*4)+2, (i*4)+1;
            F.row(i*4+2) << (i*4)+3, (i*4)+2, (i*4)+0;
            F.row(i*4+3) << (i*4)+1, (i*4)+2, (i*4)+3;
        }  
    }
}

// split tets in TT into triangles so you can visualize them
// output triangular mesh with colors per vertex or per face
void    triangulate_tets(const Eigen::MatrixXi& TT,  // tets #T by 4
                         const Eigen::MatrixXd& TV,  // tet verticies #TV by 3
                         Eigen::MatrixXd& V,         // resulting verticies #V by 3
                         Eigen::MatrixXi& F,         // resulting triangular faces #F by 3
                         Eigen::VectorXi& FtoT)      // Face to tet ID match
{    

    Eigen::MatrixXd TC;  // (optional) per vertex or per-tet colors #T|#TV by 3
    Eigen::MatrixXi TI;  // (optional) only use subset of TT. If 1 te
    Eigen::MatrixXd TD;  // #TV by 3 diffusion values per vertex
    Eigen::MatrixXd C;         // (optional if TC is not empty) resulting per-vertex or per face colors #F|#V by 3
    Eigen::MatrixXd D;         // #V by 3 diffusion values per vertex

    triangulate_tets(TT,  // tets #T by 4
                     TV,  // tet verticies #TV by 3  
                     TC,
                     TI,
                     TD,                  
                     V,         // resulting verticies #V by 3
                     F,         // resulting triangular faces #F by 3
                     C,
                     D,
                     FtoT,      // Face to tet ID match
                     true);          // (ignored if TC is empty) if true then TC and C will contain per face colors, otherwise per-vertex
   
}
