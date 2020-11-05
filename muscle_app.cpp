#include <stdio.h>
#include <functional>
#include <random>
#include <ctime>
#include <stdlib.h>

#include "muscle_app.h"
#include "RAVisibleSet.h"
#include "TyroIGLMesh.h"
#include "musclemesh.h"
#include "tetmesh.h"
#include "RAFileManager.h"
#include "RALogManager.h"
#include "slicemesh.h"
#include "color_matrix.h"
#include "boundary_verticies.h"
#include "triangulate_tets.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <igl/Timer.h>
#include <igl/unproject_in_mesh.h>
#include <igl/readOBJ.h>
#include <igl/per_vertex_normals.h>
#include <igl/unique_edge_map.h>
#include <igl/unproject_onto_mesh.h>
#include <igl/project.h>
#include <igl/cotmatrix.h>
#include <igl/matlab_format.h>
#include <igl/boundary_facets.h>
#include <igl/readMESH.h>
#include <igl/project_to_line.h>
#include <igl/unproject_on_plane.h>
#include <igl/tet_tet_adjacency.h>
#include <igl/project.h>
#include <igl/unproject.h>
#include <igl/PI.h>
#include <igl/unique.h>
#include <igl/cat.h>
#include <igl/read_triangle_mesh.h>
#include <igl/avg_edge_length.h>
#include <igl/unproject_ray.h>
#include <igl/bounding_box_diagonal.h>
#include <igl/per_face_normals.h>
#include <igl/barycenter.h>
#include <igl/sort.h>
#include <igl/slice.h>
#include <igl/list_to_matrix.h>
#include <igl/readCSV.h>

#include <Eigen/Sparse>
#include <Eigen/Geometry>

using namespace std;
using namespace Eigen;

using VecXd=Eigen::VectorXd;
using VecXi=Eigen::VectorXi;
using MatXi=Eigen::MatrixXi;
using MatXd=Eigen::MatrixXd;
using SMatXd=Eigen::SparseMatrix<double>;

// mouse buttons
#define LMB 0 //left
#define RMB 1 //right
#define MMB 2 //middle

namespace tyro
{   
    App::App()
    {
        m_tyro_window =nullptr;
        m_gl_rend=nullptr;
        m_camera=nullptr;
        mouse_is_down=false;
        gesture_state=0;
        m_state=App::State::None;
        m_update_camera=false;
        
        UIDATA.clip = Eigen::Vector3f(1,1,1);
        UIDATA.showwireframe = true;
        UIDATA.showtetmesh = true;
        UIDATA.showmuscles = false;
        UIDATA.showslice = 1;
        UIDATA.pause = 0;
        UIDATA.IsMouseHoveringAnyWindow = false;

        RENDER.template_mesh_wire = nullptr;
        RENDER.tetmesh = nullptr;
        RENDER.tetmesh_bndry = nullptr;
    }

    App::~App() 
    {
        if (m_tyro_window){m_tyro_window->Terminate();delete m_tyro_window; }
        if (m_gl_rend) delete m_gl_rend;
        if (m_camera) delete m_camera;
    }


    int App::Launch()
    {       
        m_tyro_window = new Window();
        m_tyro_window->Init(1280,960);
                
        //setup renderer
        m_gl_rend = new ES2Renderer(m_tyro_window->GetGLContext());
        m_gl_rend->SetClearColor(173.0/255, 216/255.0, 230/255.0, 1.0f);

        //which graphics card are you using
        printf("%s %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(m_tyro_window->GetGLFWWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 150");

        //set up window callbacks
        m_tyro_window->callback_mouse_down = [&](Window& window, int button, int modifier)->bool 
        {return this->mouse_down(window, button, modifier);};
        m_tyro_window->callback_mouse_up = [&](Window& window, int button, int modifier)->bool 
        {return this->mouse_up(window, button, modifier);};
        m_tyro_window->callback_mouse_move = [&](Window& window, int mouse_x, int mouse_y)->bool 
        {return this->mouse_move(window, mouse_x, mouse_y);};
        m_tyro_window->callback_window_resize = [&](Window& window, unsigned int w, unsigned int h)->bool 
        {return this->window_resize(window, w, h);};
        m_tyro_window->callback_key_pressed = [&](Window& window, unsigned int key, int modifiers)->bool 
        {return this->key_pressed(window, key, modifiers);};
        m_tyro_window->callback_key_down = [&](Window& window, unsigned int key, int modifiers)->bool 
        {return this->key_down(window, key, modifiers);};
        m_tyro_window->callback_mouse_scroll = [&](Window& window, float ydelta)->bool 
        {return this->mouse_scroll(window, ydelta);};

        m_update_camera = true;        

        this->reset_meshes();

        m_state = App::State::LoadedModel;
   
        double currentTime= glfwGetTime();
        double lastTime=currentTime;
        #ifdef ENABLE_MKL
        double framerate = 60.0;
        #else
        double framerate = 30.0;
        #endif
        while (!m_tyro_window->ShouldClose())
        {   
            currentTime = glfwGetTime(); 
            m_tyro_window->ProcessUserEvents();
            if(currentTime - lastTime >= 1.0 / framerate) 
            {
                if (m_state == App::State::LoadedModel) 
                    draw_meshes();

                draw_ui();

                m_tyro_window->GetGLContext()->swapBuffers();
                lastTime = currentTime;  
            }          
        }

        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
	    return 0;
    }

    void App::draw_ui()
    {
         // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        UIDATA.IsMouseHoveringAnyWindow = ImGui::IsMouseHoveringAnyWindow();
        
        ImGui::SetNextWindowSize(ImVec2(430,600), ImGuiCond_FirstUseEver);
        bool flag = true;
        ImGui::Begin("Options", &flag);

        //-----   
        ImGui::Text("Clipping");
        ImGui::SliderFloat("X Clip", &UIDATA.clip(0), -1.f, 1.0f, "%.4f");
        ImGui::SliderFloat("Y Clip", &UIDATA.clip(1), -1.f, 1.0f, "%.4f");
        ImGui::SliderFloat("Z Clip", &UIDATA.clip(2), -1.f, 1.0f, "%.4f");
        
        if (RENDER.tetmesh)
            RENDER.tetmesh->setClip(UIDATA.clip);
        if (RENDER.template_mesh_wire)
            RENDER.template_mesh_wire->setClip(UIDATA.clip);
        if (RENDER.tetmesh_bndry)
            RENDER.tetmesh_bndry->setClip(UIDATA.clip);

        //-----
        ImGui::Text("Visibility"); 
        if (RENDER.template_mesh_wire)
            ImGui::Checkbox("Wireframe", &RENDER.template_mesh_wire->Visible); ImGui::SameLine();
        
        if (RENDER.tetmesh)
            ImGui::Checkbox("Tets",  &RENDER.tetmesh->Visible); ImGui::SameLine();

        if (RENDER.tetmesh_bndry)
            ImGui::Checkbox("Bndry", &RENDER.tetmesh_bndry->Visible); ImGui::SameLine();
        
        if (RENDER.template_mesh)
            ImGui::Checkbox("Render Isosurface", &RENDER.template_mesh->Visible);;
        
        if (RENDER.mesh_slice)
            ImGui::Checkbox("Slice", &UIDATA.showslice);

        // if (ImGui::Button("Reset mesh")) 
        // {
        //     this->reset_meshes(); 
        //     m_update_camera = true;        
        // }

        if (GEOMETRY.allDT.size()>0) 
        {   
            double eps = 0.01; // prevents wireframe overlapping rendering artifacts
            // if (mSelCurve.pid == 0 || mSelCurve.pid == 3)
            //     RENDER.mesh_slice = MeshSlice::CreateSliceTet(GEOMETRY.tet.V, GEOMETRY.tet.T, GEOMETRY.allDT.col(0),UIDATA.clip(0)+eps);
            // else
            
            RENDER.mesh_slice = MeshSlice::CreateSliceTet(GEOMETRY.tet.V, GEOMETRY.tet.T, GEOMETRY.allDT.col(1), UIDATA.clip(0)+eps);

            if (RENDER.mesh_slice)
                RENDER.mesh_slice->Visible = UIDATA.showslice;
        }

        ImGui::End();       
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        GL_CHECK_ERROR
    }

    void App::draw_meshes() 
    {
        VisibleSet vis_set;
        auto rendermesh = [&](ES2RenderableSPtr a) { if (a && a->Visible) { a->Update(true); vis_set.Insert(a.get());} };

        rendermesh(RENDER.template_mesh);
        rendermesh(RENDER.tetmesh);
        rendermesh(RENDER.template_mesh_wire);
        rendermesh(RENDER.mesh_slice);
        
        if (m_update_camera) 
        {
            update_camera();
            m_update_camera = false;
        }
        
        m_gl_rend->RenderVisibleSet(&vis_set, m_camera);       
    }

    void App::update_camera() 
    {
        //setup camera
        RENDER.tetmesh->Update(true);
        auto WorldBoundBox = RENDER.tetmesh->WorldBoundBox;
        auto world_center = WorldBoundBox.center();
        float radius = std::abs(WorldBoundBox.diagonal().norm());
        int v_width, v_height;
        m_tyro_window->GetGLContext()->getFramebufferSize(&v_width, &v_height);
        Eigen::Vector4i viewport(0, 0, v_width, v_height);
        float aspect = (float)v_width/v_height;
        
        if (m_camera)
            delete m_camera;
        
        m_camera = new iOSCamera(world_center, radius, aspect, 1, viewport, false);                
    } 

    int App::reset_meshes() 
    {   
        igl::Timer timer;
        timer.start();

        RENDER.Reset();

        auto tet_dot_mesh = std::string(MY_RESOURCE_FILE) + std::string("/data/cylinder.mesh");
        MatXi _f;
        igl::readMESH(tet_dot_mesh, GEOMETRY.tet.V, GEOMETRY.tet.T, _f);

        VecXd center = GEOMETRY.tet.V.colwise().sum()/ GEOMETRY.tet.V.rows();
        GEOMETRY.tet.V.rowwise() -= center.transpose();
        double diag_scale = 1.0/(0.5*igl::bounding_box_diagonal(GEOMETRY.tet.V)); 
        GEOMETRY.tet.V = diag_scale*GEOMETRY.tet.V;
        center = diag_scale*center;

        VecXd Occup = (GEOMETRY.tet.V.rowwise() - center.transpose()).rowwise().squaredNorm(); // simulate the probability of occupancy

        // preallocate
        // GEOMETRY.tet.DI.resize(GEOMETRY.tet.T.rows(), 4); 
        GEOMETRY.tet.DT.resize(Occup.rows(), 1);  
        GEOMETRY.tet.DT.col(0) = Occup;
    
        // triangulate tets
        triangulate_tets(GEOMETRY.tet.T, GEOMETRY.tet.V, GEOMETRY.tritet.V, GEOMETRY.tritet.F, GEOMETRY.tritet.FtoT);
        
        igl::per_face_normals(GEOMETRY.tritet.V, GEOMETRY.tritet.F, GEOMETRY.tritet.N);  

        RENDER.tetmesh = TetMesh::Create(GEOMETRY.tritet.V,  GEOMETRY.tritet.F, GEOMETRY.tritet.N);
        RENDER.tetmesh->Visible = false;
    
        MatXd G2;        
        RENDER.template_mesh = MuscleMesh::Create(GEOMETRY.tritet.V, GEOMETRY.tritet.F, GEOMETRY.tritet.N, GEOMETRY.tet.DT, GEOMETRY.tet.DI, GEOMETRY.tet.V, GEOMETRY.tet.T, GEOMETRY.tritet.FtoT, G2, true);
        RENDER.template_mesh->Visible = true;
        
        MatXd UEC_T;
        MatXi TBF, E_T, UE_T; // Face data. 
        VecXi EMAP_T; // Map directed edges to unique edges. 
        std::vector<std::vector<int> > uE2E_T;

        igl::unique_edge_map(GEOMETRY.tritet.F,E_T,UE_T,EMAP_T,uE2E_T);
        color_matrix(UE_T.rows(), Eigen::Vector3d(0.1,0.1,0.1), UEC_T);
        
        bool prev_visible = false;
        if (RENDER.template_mesh_wire)
            prev_visible = RENDER.template_mesh_wire->Visible;

        RENDER.template_mesh_wire = IGLMeshWireframe::Create(GEOMETRY.tritet.V, UE_T, UEC_T);
        RENDER.template_mesh_wire->Visible = prev_visible;

        m_update_camera = true;   
        update_camera();

        return 1;
    }

    // void App::compute_muscles() 
    // {   

    //     this->update_meshes();
     
    //     // find_max_muscles(GEOMETRY.tet.V, GEOMETRY.tet.T, GEOMETRY.allDT, GEOMETRY.tet.DT, GEOMETRY.tet.DI);
    //     // RENDER.template_mesh->UpdateDiffusion(GEOMETRY.tet.DT, GEOMETRY.tet.DI);
    // }
    
    bool App::mouse_down(Window& window,  int button, int modifier) 
    {   
        if (m_state != App::State::LoadedModel || !m_camera || UIDATA.IsMouseHoveringAnyWindow) 
            return true; 
        
        double x = current_mouse_x;
        double y = m_camera->GetEigenViewport()(3)- current_mouse_y;

        // RA_LOG_INFO("%i, %i", modifier, modifier == TYRO_MOD_NONE);
        if (modifier == TYRO_MOD_CONTROL)   // camera rotation
        {   
            mouse_is_down = true;
            m_modifier = modifier;
            m_mouse_btn_clicked = button;
        } 
       
        return true;
    }

    bool App::mouse_move(Window& window, int mouse_x, int mouse_y) 
    {   
        if (m_state != App::State::LoadedModel || !m_camera) 
            return true; 
        
        current_mouse_x = mouse_x;
        current_mouse_y = mouse_y;

        double x = current_mouse_x;
        double y = m_camera->GetEigenViewport()(3)- current_mouse_y;

       
        if (mouse_is_down && m_modifier == TYRO_MOD_CONTROL)  // camera
        {
            if (m_mouse_btn_clicked == LMB) 
                m_camera->HandleOneFingerPanGesture(gesture_state, Eigen::Vector2i(mouse_x, mouse_y));
            else if (m_mouse_btn_clicked == MMB) 
                m_camera->HandleTwoFingerPanGesture(gesture_state, Eigen::Vector2i(mouse_x, -mouse_y));

            gesture_state = 1;
        }

        return true;
        
    }

    bool App::mouse_up(Window& window, int button, int modifier) 
    {   
        if (m_state != App::State::LoadedModel || !m_camera ) 
            return true; 

        if (mouse_is_down && m_modifier == TYRO_MOD_CONTROL) 
        {   
            gesture_state = 2;
            
            if (m_mouse_btn_clicked == LMB) 
                m_camera->HandleOneFingerPanGesture(gesture_state, Eigen::Vector2i(current_mouse_x, current_mouse_y));
            else if (m_mouse_btn_clicked == MMB) 
                m_camera->HandleTwoFingerPanGesture(gesture_state, Eigen::Vector2i(current_mouse_x, -current_mouse_y));
        } 
       
        mouse_is_down = false;
        gesture_state = 0;
    
        return true;
    }

    bool App::mouse_scroll(Window& window, float ydelta) 
    {
        //RA_LOG_INFO("mouse scroll delta %f", ydelta);
        if (m_state != App::State::LoadedModel || UIDATA.IsMouseHoveringAnyWindow) return false;
        
        m_camera->HandlePinchGesture(gesture_state, Eigen::Vector2i(current_mouse_x, current_mouse_y), ydelta);

        return true;
    } 

    bool App::window_resize(Window& window, unsigned int w, unsigned int h)
    {
        update_camera();
        return true;
    }

    bool App::key_pressed(Window& window, unsigned int key, int modifiers) 
    {   
        return true;
    }
    
    bool App::key_down(Window& window, unsigned int key, int modifiers) 
    {   
        #define DEBUG_key_down 0
        
        #if DEBUG_key_down
        std::cout<<"key: "<<key<<std::endl;
        #endif
        
        return true;
    }
  
}