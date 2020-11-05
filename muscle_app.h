#pragma once

#include "TyroWindow.h"
#include "RAES2Renderer.h"
#include "RAiOSCamera.h"
#include "RAEnginePrerequisites.h"

#include <vector>
#include <atomic>

#include <Eigen/Core>
#include <Eigen/Sparse>

#include <igl/AABB.h>


namespace tyro
{   
    class App 
    {
    public:

        enum State {None, Launched, LoadedModel, PlayingAnimation};

        App();
        ~App();
        int Launch();
   
        void update_camera();
        void draw_meshes();
        void draw_ui();
        void update_meshes();
        int reset_meshes();

        //UI
        bool mouse_down(Window& window, int button, int modifier);
        bool mouse_up(Window& window, int button, int modifier);
        bool mouse_move(Window& window, int mouse_x, int mouse_y);
        bool mouse_scroll(Window& window, float ydelta); 
        bool window_resize(Window& window, unsigned int w, unsigned int h);    
        bool key_pressed(Window& window, unsigned int key, int modifiers); 
        bool key_down(Window& window, unsigned int key, int modifiers); 

        bool mouse_is_down;
        int gesture_state; 
        int current_mouse_x;
        int current_mouse_y;
        int m_modifier;
        int m_mouse_btn_clicked;
        bool m_update_camera;

        State m_state;
        Window* m_tyro_window;
        ES2Renderer* m_gl_rend;
        iOSCamera* m_camera;

        struct MGeometry 
        {
            // Tetrahedral mesh            
            struct Tet
            {
                Eigen::MatrixXd V;  // |T|*4 matrix of tet indicies
                Eigen::MatrixXi T;  // |T|*4 matrix of tet indicies
                Eigen::MatrixXd DT; // |V|x3 by num_curves mattrix of diffusion values per vertex
                Eigen::MatrixXi DI; // |V|x1 vector of indicies of max muscle at that point
                Eigen::MatrixXi Tadj; // Tet tet adjacency
                igl::AABB<Eigen::MatrixXd,3> tree; // AABD tree for distance computations
                double avg_edge_length;
                Eigen::SparseMatrix<double> L; //laplacian matrix . Its already negated (so its PSD)
                Eigen::VectorXi bndry; // indicies into VT of the boundary verticies
            };

            // triangulate tet mesh
            // break down tets into triangles for rendering
            struct TriTet
            {
                Eigen::MatrixXd V; // |V|x3 array of verticies
                Eigen::MatrixXi F; // |F|x3 or 4*|T|*3 array of face indcies (each face belong to a tet)
                Eigen::MatrixXd N; // |F|*3 array of per face normals   
                Eigen::VectorXi FtoT; // map F id to tet id it came from    
            };

            
            TriTet tritet;
            Tet tet;

            Eigen::MatrixXd allDT; //gloabl matrix of diffusion
        };
        MGeometry GEOMETRY;        

        struct MRenderData 
        {
            MeshSliceSPtr mesh_slice;
            MuscleMeshSPtr template_mesh;
            IGLMeshWireframeSPtr template_mesh_wire;
            TetMeshSPtr tetmesh;
            TetMeshSPtr tetmesh_bndry;
            void Reset() {}
        };
        MRenderData RENDER;

        struct UIData 
        {
            Eigen::Vector3f clip;
            bool showwireframe;
            bool showtetmesh;
            bool showmuscles;
            bool showslice;
            bool pause;
            double diag_scale;
            Eigen::Vector3d center;
            bool IsMouseHoveringAnyWindow;
        };
        UIData UIDATA;
    };
}