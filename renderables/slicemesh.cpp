//
//  RAMuscleMesh.h
//  Ossa
//
//  Created by Rinat Abdrashitov on 2015-04-09.
//  Copyright (c) 2015 Rinat Abdrashitov. All rights reserved.
//

#include "slicemesh.h"
#include "RAEnginePrerequisites.h"
#include "RAES2TriMesh.h"
#include "RAES2CoreVisualEffects.h"
#include "RACamera.h"
#include <Eigen/Core>
#include "RAVertexHardwareBuffer.h"
#include "RAVertexBufferAccessor.h"
#include "RAFileManager.h"
#include "RAES2VisualEffect.h"
#include "RAES2BufferTexture.h"
#include <igl/marching_tets.h>

namespace tyro
{
    
    ES2VisualEffectSPtr 
    MeshSlice::VisualEffect()
    {
        ES2ShaderProgram* shader = new ES2ShaderProgram();
        shader->LoadProgram(GetFilePath("mesh_slice", "vs"), GetFilePath("mesh_slice", "fs"));
                    
        ES2VertexFormat* vertexFormat = new ES2VertexFormat(2);
        vertexFormat->SetAttribute(0, shader->GetAttributeLocation("aPosition"), 0, ES2VertexFormat::AT_FLOAT3, ES2VertexFormat::AU_POSITION);
        vertexFormat->SetAttribute(1, shader->GetAttributeLocation("aValue"), vertexFormat->GetOffsetForNextAttrib(0), ES2VertexFormat::AT_FLOAT1, ES2VertexFormat::AU_JOINT_INDEX_1);
        
        ES2ShaderUniforms* uniforms = new ES2ShaderUniforms(1);
        uniforms->SetUniform(0, shader->GetUniformLocation("uMVPMatrix"), 1, "uMVPMatrix", ES2ShaderUniforms::UniformMatrix4fv);
        
        ES2VisualEffectSPtr effect(std::make_shared<ES2VisualEffect>(shader, vertexFormat, uniforms));

        return effect;
    }


    void MeshSlice::Init(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F, const Eigen::VectorXd& D) 
    {
        assert(V.rows()==D.size());

        ES2TriMesh::Init();
        SetVisualEffect(MeshSlice::VisualEffect());
 
        int numVertices = V.rows();
        int numTriangles = F.rows();
        int numIndices = 3*numTriangles;
 
        struct Vertex
        {
            GLfloat position[3];
            GLfloat value;
        };

        int stride = sizeof(Vertex);

        //vertex buffer data
        auto vbuffer = std::make_shared<tyro::ES2VertexHardwareBuffer>(stride, numIndices, nullptr, HardwareBuffer::BU_STATIC);
        SetVertexBuffer(vbuffer);

        //vertex buffer data
        VertexBufferAccessor vba(GetVisualEffect()->GetVertexFormat(), GetVertexBuffer().get());
        vba.MapWrite();
        int vIndex = 0;

        for (int fid = 0; fid < numTriangles; ++fid) 
        {   
            for (int j = 0; j < 3; ++j) 
            {   
                int vid = F(fid,j);
                vba.Position<Eigen::Vector3f>(vIndex) = Eigen::Vector3f(V(vid,0), V(vid,1), V(vid,2));
                vba.JointIndex1<float>(vIndex) = D(vid);
                vIndex++;
            }                
        }
        vba.Unmap();        
        
        //compute bounding box
        // LocalBoundBox.ComputeExtremes(vbuffer->GetNumOfVerticies(), vbuffer->GetVertexSize(), vbuffer->MapRead());
        ComputeExtremesL(vbuffer->GetNumOfVerticies(), vbuffer->GetVertexSize(), vbuffer->MapRead());
        vbuffer->Unmap();

	    ES2VertexArraySPtr varray = std::make_shared<ES2VertexArray>(this->GetVisualEffect(), vbuffer);
	    SetVertexArray(varray);        
        GetVisualEffect()->CullStateEnabled = false; 
    }
        
    MeshSliceSPtr MeshSlice::Create(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F, const Eigen::VectorXd& D) 
    {
        MeshSliceSPtr ptr = std::make_shared<MeshSlice>();
        ptr->Init(V,F,D);
        return ptr;
    }

    //Create this mesh by slicing a tet mesh defined by TV, TT and D. Slice with a plane
    MeshSliceSPtr MeshSlice::CreateSliceTet(const Eigen::MatrixXd& TV, 
                                            const Eigen::MatrixXi& TT, 
                                            const Eigen::VectorXd& D, 
                                            double isovalue)
    {   
        Eigen::MatrixXd SV;
        Eigen::MatrixXi SF;
        Eigen::VectorXi J;
        Eigen::SparseMatrix<double> BC;
        Eigen::VectorXd tvcol = TV.col(0);
        igl::marching_tets(TV,TT,tvcol,isovalue,SV,SF,J,BC);
        
        if (SV.size() == 0)
            return nullptr;
        
        if (SF.size() == 0)
            return nullptr;

        Eigen::VectorXd DV = BC*D;

        MeshSliceSPtr ptr = std::make_shared<MeshSlice>();
        ptr->Init(SV,SF,DV);
        return ptr;
    }

    void MeshSlice::UpdateUniformsWithCamera(const tyro::Camera* camera)
    {
        Eigen::Matrix4f viewMatrix = camera->GetEigenViewMatrix();
        Eigen::Matrix4f projectionMatrix = camera->GetEigenProjMatrix();
        Eigen::Matrix4f modelViewMatrix = viewMatrix;// * WorldTransform.Matrix();
        Eigen::Matrix4f modelViewProjectionMatrix =  projectionMatrix * modelViewMatrix;

        GetVisualEffect()->GetUniforms()->UpdateFloatUniform(0, modelViewProjectionMatrix.data());
    }
}