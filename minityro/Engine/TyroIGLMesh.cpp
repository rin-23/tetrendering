#include "TyroIGLMesh.h"
// #include "GLStructures.h"
#include "RAVertexHardwareBuffer.h"
#include "RAIndexHardwareBuffer.h"
#include "RAES2CoreVisualEffects.h"
#include "RAVertexBufferAccessor.h"
#include <igl/edges.h>
#include "RACamera.h"
#include "RAFileManager.h"

namespace tyro 
{   
    

    ES2VisualEffectSPtr IGLMeshWireframe::VisualEffect() 
    {
        ES2ShaderProgram* shader = new ES2ShaderProgram();
        shader->LoadProgram(GetFilePath("WireframeSlice", "vs"), GetFilePath("WireframeSlice", "fs"));
       
        ES2VertexFormat* vertexFormat = new ES2VertexFormat(2);
        vertexFormat->SetAttribute(0, shader->GetAttributeLocation("aPosition"), 0, ES2VertexFormat::AT_FLOAT3, ES2VertexFormat::AU_POSITION);
        vertexFormat->SetAttribute(1, shader->GetAttributeLocation("aColor"), vertexFormat->GetOffsetForNextAttrib(0), ES2VertexFormat::AT_FLOAT3, ES2VertexFormat::AU_COLOR);
        
        ES2ShaderUniforms* uniforms = new ES2ShaderUniforms(2);
        uniforms->SetUniform(0, shader->GetUniformLocation("uMVPMatrix"), 1, "uMVPMatrix", ES2ShaderUniforms::UniformMatrix4fv);
        uniforms->SetUniform(1, shader->GetUniformLocation("planepoint"), 1, "planepoint", ES2ShaderUniforms::Uniform3fv);
            
        ES2VisualEffectSPtr effect(std::make_shared<ES2VisualEffect>(shader, vertexFormat, uniforms));

        return effect;
    }

    void IGLMeshWireframe::UpdateData(const Eigen::MatrixXd& V, const Eigen::MatrixXi& uE, const Eigen::MatrixXd& uC) 
    {
        //int numVertices = V.rows();
        int numEdges = uE.rows();
        int numPoints = V.rows();
        int stride = 2*sizeof(Eigen::Vector3f);
    
        //vertex buffer data        
        VertexBufferAccessor vba(GetVisualEffect()->GetVertexFormat(), GetVertexBuffer().get());
        vba.MapWrite();        
        for (int i = 0; i < numPoints; ++i) 
        {   
            vba.Position<Eigen::Vector3f>(i) = Eigen::Vector3f(V(i,0), V(i,1), V(i,2));
            vba.Color<Eigen::Vector3f>(i) = Eigen::Vector3f(uC(i,0), uC(i,1), uC(i,2));
        }                
        vba.Unmap();

        //vertex index data       
        unsigned int* indexData = (unsigned int*) GetIndexBuffer()->MapWrite();
        for (int i = 0; i < numEdges; ++i) 
        {   
            indexData[2*i] =  uE(i,0);
            indexData[2*i+1] = uE(i,1);
        }                
        GetIndexBuffer()->Unmap();
    }

        
    void IGLMeshWireframe::Init(const Eigen::MatrixXd& V, const Eigen::MatrixXi& uE, const Eigen::MatrixXd& uC)
    {   
        ES2Polyline::Init(false);
        SetVisualEffect(IGLMeshWireframe::VisualEffect());

        //Calculate unique edges
        
        //int numVertices = V.rows();
        int numEdges = uE.rows();
        int numPoints = V.rows();
        int stride = 2*sizeof(Eigen::Vector3f);
    
        //vertex buffer data
        auto vbuffer = std::make_shared<tyro::ES2VertexHardwareBuffer>(stride, numPoints, nullptr, HardwareBuffer::BU_STATIC);
        SetVertexBuffer(vbuffer);

        auto ibuffer = std::make_shared<tyro::ES2IndexHardwareBuffer>(2*numEdges, nullptr, HardwareBuffer::BU_STATIC);
        SetIndexBuffer(ibuffer);

        this->UpdateData(V, uE, uC);
        
        //compute bounding box
        // LocalBoundBox.ComputeExtremesL(vbuffer->GetNumOfVerticies(), vbuffer->GetVertexSize(), vbuffer->MapRead());
        ComputeExtremesL(vbuffer->GetNumOfVerticies(), vbuffer->GetVertexSize(), vbuffer->MapRead());

        vbuffer->Unmap();
       
	    ES2VertexArraySPtr varray = std::make_shared<ES2VertexArray>(this->GetVisualEffect(), vbuffer);
	    SetVertexArray(varray);

        GetVisualEffect()->CullStateEnabled = true;    
        GetVisualEffect()->PolygonOffsetEnabled = true;
        GetVisualEffect()->PolygonOffsetFactor = 0;
        GetVisualEffect()->PolygonOffsetUnits = -5;
        GetVisualEffect()->PolygonOffsetIsSolid = false;
        
    }

    //  void IGLMeshWireframe::Init(const Eigen::MatrixXd& V, const Eigen::MatrixXi& uE, const Eigen::MatrixXd& uC, const std::vector<int>& eid_list)
    // {   
    //     ES2Polyline::Init(false);
    //     SetVisualEffect(ES2CoreVisualEffects::WireframeColor());

    //     //Calculate unique edges
        
    //     //int numVertices = V.rows();
    //     int numEdges = eid_list.size();
    //     int numPoints = numEdges * 2;
    //      struct Wire
    //     {
    //         Eigen::Vector3f position;
    //         Eigen::Vector3f color;
    //     };

    //     int stride = sizeof(Wire);
    
    //     //vertex buffer data
    //     auto vbuffer = std::make_shared<tyro::ES2VertexHardwareBuffer>(stride, numPoints, nullptr, HardwareBuffer::BU_DYNAMIC);
    //     SetVertexBuffer(vbuffer);
    //     //this->UpdateData(V, uE, uC);
        
    //     //vertex buffer data        
    //     VertexBufferAccessor vba(GetVisualEffect()->GetVertexFormat(), GetVertexBuffer().get());
    //     int vIndex = 0;
    //     vba.MapWrite();        
    //     for (int j = 0; j < numEdges; ++j) 
    //     {   
    //         //RA_LOG_INFO("V: %f %f %f", V(i, 0), V(i, 1), V(i, 2));
    //         int i = eid_list[j];
    //         int v1 = uE(i,0);
    //         int v2 = uE(i,1);
            
    //         vba.Position<Eigen::Vector3f>(vIndex) = Eigen::Vector3f(V(v1,0), V(v1,1), V(v1,2));
    //         vba.Color<Eigen::Vector3f>(vIndex++) = Eigen::Vector3f(0, 0.8, 0);
    //         vba.Position<Eigen::Vector3f>(vIndex) = Eigen::Vector3f(V(v2,0), V(v2,1), V(v2,2));
    //         vba.Color<Eigen::Vector3f>(vIndex++) = Eigen::Vector3f(0, 0.8, 0);
    //     }                
    //     vba.Unmap();

    //     //compute bounding box
    //     //LocalBoundBox.ComputeExtremes(vbuffer->GetNumOfVerticies(), vbuffer->GetVertexSize(), vbuffer->MapRead());
    //     ComputeExtremesL(vbuffer->GetNumOfVerticies(), vbuffer->GetVertexSize(), vbuffer->MapRead());
    //     vbuffer->Unmap();
       
	//     ES2VertexArraySPtr varray = std::make_shared<ES2VertexArray>(this->GetVisualEffect(), vbuffer);
	//     SetVertexArray(varray);

    //     GetVisualEffect()->CullStateEnabled = false;    
    //     GetVisualEffect()->PolygonOffsetEnabled = true;
    //     GetVisualEffect()->PolygonOffsetFactor = 0;
    //     GetVisualEffect()->PolygonOffsetUnits = -5;
    //     GetVisualEffect()->PolygonOffsetIsSolid = false;
        
    // }

    IGLMeshWireframeSPtr IGLMeshWireframe::Create(const Eigen::MatrixXd& V, const Eigen::MatrixXi& uE, const Eigen::MatrixXd& uC)
    {
        IGLMeshWireframeSPtr sptr = std::make_shared<IGLMeshWireframe>();
        sptr->Init(V,uE,uC);
        return sptr;
    }
    
    // IGLMeshWireframeSPtr IGLMeshWireframe::Create(const Eigen::MatrixXd& V, const Eigen::MatrixXi& uE, 
    //                                               const Eigen::MatrixXd& uC, const std::vector<int>& eid_list) 
    // {
    //     IGLMeshWireframeSPtr sptr = std::make_shared<IGLMeshWireframe>();
    //     sptr->Init(V, uE, uC, eid_list);
    //     return sptr;
    // }

    /*
    IGLMeshWireframeSPtr IGLMeshWireframe::Create(Eigen::MatrixXd& V, Eigen::MatrixXi& F, Eigen::Vector3d& color)
    {
        Eigen::MatrixXd C;
        C.resize(V.rows(), V.cols());
        for (int i =0 ; i <  V.rows(); ++i) 
        {
            C.row(i) = color;
        }

        IGLMeshWireframeSPtr sptr = std::make_shared<IGLMeshWireframe>();
        sptr->Init(V,F,C);
        return sptr;
    }
    */
 
    void IGLMeshWireframe::UpdateUniformsWithCamera(const Camera* camera)
    {
            
        Eigen::Matrix4f MVPMatrix = camera->GetEigenProjMatrix() * camera->GetEigenViewMatrix(); // * WorldTransform.Matrix();
        GetVisualEffect()->GetUniforms()->UpdateFloatUniform(0, MVPMatrix.data());
        GetVisualEffect()->GetUniforms()->UpdateFloatUniform(1, mClip.data());
    }

  
    
}