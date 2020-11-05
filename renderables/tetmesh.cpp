//
//  RAMuscleMesh.h
//  Ossa
//
//  Created by Rinat Abdrashitov on 2015-04-09.
//  Copyright (c) 2015 Rinat Abdrashitov. All rights reserved.
//

#include "tetmesh.h"
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
#include <igl/per_face_normals.h>

namespace tyro
{
    
    ES2VisualEffectSPtr 
    TetMesh::VisualEffect()
    {
        ES2ShaderProgram* shader = new ES2ShaderProgram();
        shader->LoadProgram(GetFilePath("TetMeshSlice", "vs"), GetFilePath("TetMeshSlice", "fs"));
                    
        ES2VertexFormat* vertexFormat = new ES2VertexFormat(3);
        vertexFormat->SetAttribute(0, shader->GetAttributeLocation("aPosition"), 0, ES2VertexFormat::AT_FLOAT3, ES2VertexFormat::AU_POSITION);
        vertexFormat->SetAttribute(1, shader->GetAttributeLocation("aNormal"), vertexFormat->GetOffsetForNextAttrib(0), ES2VertexFormat::AT_FLOAT3, ES2VertexFormat::AU_NORMAL);
        vertexFormat->SetAttribute(2, shader->GetAttributeLocation("aColor"), vertexFormat->GetOffsetForNextAttrib(1), ES2VertexFormat::AT_FLOAT3, ES2VertexFormat::AU_COLOR);
        
        ES2ShaderUniforms* uniforms = new ES2ShaderUniforms(3);
        uniforms->SetUniform(0, shader->GetUniformLocation("uMVPMatrix"), 1, "uMVPMatrix", ES2ShaderUniforms::UniformMatrix4fv);
        uniforms->SetUniform(1, shader->GetUniformLocation("uNMatrix"), 1, "uNMatrix", ES2ShaderUniforms::UniformMatrix3fv);
        uniforms->SetUniform(2, shader->GetUniformLocation("planepoint"), 1, "planepoint", ES2ShaderUniforms::Uniform3fv);
        
        ES2VisualEffectSPtr effect(std::make_shared<ES2VisualEffect>(shader, vertexFormat, uniforms));

        return effect;
    }


    void TetMesh::Init(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F, const Eigen::MatrixXd& N) 
    {
        // ES2TriMesh::Init();
        // SetVisualEffect(ES2CoreVisualEffects::GourandDirectional());
        assert(N.rows() == F.rows());
        ES2TriMesh::Init();
        SetVisualEffect(TetMesh::VisualEffect());
 
        SetColor(Eigen::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
        
        int numVertices = V.rows();
        int numTriangles = F.rows();
        int numIndices = 3*numTriangles;
        int numNormals = N.rows();

        struct Vertex
        {
            GLfloat position[3];
            GLfloat normal[3];
            GLfloat color[3];
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
                vba.Normal<Eigen::Vector3f>(vIndex) = Eigen::Vector3f(N(fid, 0), N(fid, 1), N(fid, 2));
                vba.Color<Eigen::Vector3f>(vIndex) = Eigen::Vector3f(mColor(0), mColor(1), mColor(2));;
    
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
        
    TetMeshSPtr TetMesh::Create(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F,   
                                      const Eigen::MatrixXd& N) 
    {
        TetMeshSPtr ptr = std::make_shared<TetMesh>();
        ptr->Init(V,F,N);
        return ptr;
    }

    TetMeshSPtr TetMesh::Create(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F) 
    {
        Eigen::MatrixXd N;
        igl::per_face_normals(V, F, N); 

        TetMeshSPtr ptr = std::make_shared<TetMesh>();
        ptr->Init(V,F,N);
        return ptr;
    }

    void TetMesh::SetColor(const Eigen::Vector4f& color)
    {
        mColor = color;
    }
  
    void TetMesh::UpdateUniformsWithCamera(const tyro::Camera* camera)
    {
        Eigen::Matrix4f viewMatrix = camera->GetEigenViewMatrix();
        Eigen::Matrix4f projectionMatrix = camera->GetEigenProjMatrix();
        Eigen::Matrix4f modelViewMatrix = viewMatrix;// * WorldTransform.Matrix();
        Eigen::Matrix4f modelViewProjectionMatrix =  projectionMatrix * modelViewMatrix;
        Eigen::Matrix3f normalMatrix = camera->GetEigenNormalMatrix();

        Eigen::Vector4f vport_f = camera->GetEigenViewport();
        // Eigen::Vector4f vport_f(vport_i[0], vport_i[1], vport_i[2],vport_i[3]);

        
        GetVisualEffect()->GetUniforms()->UpdateFloatUniform(0, modelViewProjectionMatrix.data());
        GetVisualEffect()->GetUniforms()->UpdateFloatUniform(1, normalMatrix.data());
        GetVisualEffect()->GetUniforms()->UpdateFloatUniform(2, mClip.data());
    }
}