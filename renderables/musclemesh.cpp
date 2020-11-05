//
//  RAMuscleMesh.h
//  Ossa
//
//  Created by Rinat Abdrashitov on 2015-04-09.
//  Copyright (c) 2015 Rinat Abdrashitov. All rights reserved.
//

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
#include "musclemesh.h"
#include <igl/Timer.h>

namespace tyro
{
   
    ES2VisualEffectSPtr 
    MuscleMesh::VisualEffect()
    {
        igl::Timer timer;
        timer.start();
        ES2ShaderProgram* shader = new ES2ShaderProgram();
        shader->LoadProgram(GetFilePath("Muscle", "vs"), GetFilePath("Muscle", "fs"));
        
        ES2VertexFormat* vertexFormat = new ES2VertexFormat(2);
        vertexFormat->SetAttribute(0, shader->GetAttributeLocation("aPosition"), 0, ES2VertexFormat::AT_FLOAT3, ES2VertexFormat::AU_POSITION);
        // vertexFormat->SetAttribute(1, shader->GetAttributeLocation("aNormal"), vertexFormat->GetOffsetForNextAttrib(0), ES2VertexFormat::AT_FLOAT3, ES2VertexFormat::AU_NORMAL);
        // vertexFormat->SetAttribute(2, shader->GetAttributeLocation("aDiffusion"), vertexFormat->GetOffsetForNextAttrib(1), ES2VertexFormat::AT_FLOAT1, ES2VertexFormat::AU_BLENDWEIGHT_1);
        vertexFormat->SetAttribute(1, shader->GetAttributeLocation("aTetId"), vertexFormat->GetOffsetForNextAttrib(0), ES2VertexFormat::AT_INT1, ES2VertexFormat::AU_JOINT_INDEX_1);
        
        ES2ShaderUniforms* uniforms = new ES2ShaderUniforms(8);
        uniforms->SetUniform(0, shader->GetUniformLocation("uMVPMatrix"), 1, "uMVPMatrix", ES2ShaderUniforms::UniformMatrix4fv);
        uniforms->SetUniform(1, shader->GetUniformLocation("uNMatrix"), 1, "uNMatrix", ES2ShaderUniforms::UniformMatrix3fv);
        // uniforms->SetUniform(2, shader->GetUniformLocation("uColor"), 1, "uColor", ES2ShaderUniforms::Uniform4fv);
        uniforms->SetUniform(2, shader->GetUniformLocation("uMVMatrix"), 1, "uMVMatrix", ES2ShaderUniforms::UniformMatrix4fv);
        uniforms->SetUniform(3, shader->GetUniformLocation("uVerSampler"), 1, "uVerSampler", ES2ShaderUniforms::Uniform1iv);
        uniforms->SetUniform(4, shader->GetUniformLocation("uNormSampler"), 1, "uNormSampler", ES2ShaderUniforms::Uniform1iv);
        // uniforms->SetUniform(5, shader->GetUniformLocation("uViewportCustom"), 1, "uViewportCustom", ES2ShaderUniforms::Uniform4fv);
        // uniforms->SetUniform(5, shader->GetUniformLocation("uPMatrix"), 1, "uPMatrix", ES2ShaderUniforms::UniformMatrix4fv);
        uniforms->SetUniform(5, shader->GetUniformLocation("uDiffusedValuesSampler"), 1, "uDiffusedValuesSampler", ES2ShaderUniforms::Uniform1iv);
        uniforms->SetUniform(6, shader->GetUniformLocation("uMuscleSampler"), 1, "uMuscleSampler", ES2ShaderUniforms::Uniform1iv);
        uniforms->SetUniform(7, shader->GetUniformLocation("uMuscleColorSampler"), 1, "uMuscleColorSampler", ES2ShaderUniforms::Uniform1iv);

        ES2VisualEffectSPtr effect(std::make_shared<ES2VisualEffect>(shader, vertexFormat, uniforms));

        // RA_LOG_INFO("SHADER TIME %f", timer.getElapsedTime());
        return effect;
    }


    void MuscleMesh::Init(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F, const Eigen::MatrixXd& N, 
                          const Eigen::MatrixXd& MV, const Eigen::MatrixXi& MI, const Eigen::MatrixXd& TV, 
                          const Eigen::MatrixXi& TT, const Eigen::VectorXi& FtoT, const Eigen::MatrixXd& G, bool isnew) 
    {
        #define DEBUG_Init 0
        // ES2TriMesh::Init();
        // SetVisualEffect(ES2CoreVisualEffects::GourandDirectional());
        igl::Timer timer;
        timer.start();
        assert(N.rows() == F.rows());
        ES2TriMesh::Init();
        SetVisualEffect(MuscleMesh::VisualEffect());
    
        SetColor(Eigen::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
        
        mNumTets = TT.rows();
        int numVertices = V.rows();
        int numTriangles = F.rows();
        int numIndices = 3*numTriangles;
        int numNormals = N.rows();

        struct MuscleVertex
        {
            GLfloat position[3];
            // GLfloat normal[3];
            // GLfloat diffusion;
            GLint tetidx;
        };

        int stride = sizeof(MuscleVertex);

        if (numNormals != numVertices)
            RA_LOG_ERROR_ASSERT("Number of verticies and normals doesnt match");
        
        //vertex buffer data
        auto vbuffer = std::make_shared<tyro::ES2VertexHardwareBuffer>(stride, numIndices, nullptr, HardwareBuffer::BU_STATIC);
        SetVertexBuffer(vbuffer);
        #if DEBUG_Init
        RA_LOG_INFO("[MuscleMesh::Init]SetVertexBuffer %f", timer.getElapsedTime());
        #endif


        //vertex buffer data
        VertexBufferAccessor vba(GetVisualEffect()->GetVertexFormat(), GetVertexBuffer().get());
        vba.MapWrite();
        int vIndex = 0;

        for (int fid = 0; fid < numTriangles; ++fid) 
        {   
            //RA_LOG_INFO("V: %f %f %f", V(i, 0), V(i, 1), V(i, 2));
            //RA_LOG_INFO("N: %f %f %f", N(i, 0), N(i, 1), N(i, 2));
            for (int j = 0; j < 3; ++j) 
            {   
                int vid = F(fid,j);
                vba.Position<Eigen::Vector3f>(vIndex) = Eigen::Vector3f(V(vid,0), V(vid,1), V(vid,2));
                // vba.Normal<Eigen::Vector3f>(vIndex) = Eigen::Vector3f(N(fid, 0), N(fid, 1), N(fid, 2));
                // vba.BlendWeight1<float>(vIndex) = 0.0; //float(MV(vid));
                int tid = FtoT(fid);
                vba.JointIndex1<int>(vIndex) = tid;

                vIndex++;
            }                
        }
        vba.Unmap();  
        #if DEBUG_Init
        RA_LOG_INFO("[MuscleMesh::Init]VBA map %f", timer.getElapsedTime());
        #endif
      
        
        //compute bounding box
        // LocalBoundBox.ComputeExtremes(vbuffer->GetNumOfVerticies(), vbuffer->GetVertexSize(), vbuffer->MapRead());
        ComputeExtremesL(vbuffer->GetNumOfVerticies(), vbuffer->GetVertexSize(), vbuffer->MapRead());
        #if DEBUG_Init
        RA_LOG_INFO("[MuscleMesh::Init]ComputeExtremesL %f", timer.getElapsedTime());
        #endif

        vbuffer->Unmap();


	    ES2VertexArraySPtr varray = std::make_shared<ES2VertexArray>(this->GetVisualEffect(), vbuffer);
	    SetVertexArray(varray);        
        GetVisualEffect()->CullStateEnabled = true; 
        #if DEBUG_Init
        RA_LOG_INFO("[MuscleMesh::Init]SetVertexArray %f", timer.getElapsedTime());
        #endif



        // generate vertex texture buffer
        assert(TT.cols()==4);
        int numEntries = 4*TT.rows();
        Eigen::Vector3f* vertexDataPtr = new Eigen::Vector3f[numEntries]; 
        for (int tid = 0; tid < TT.rows(); ++tid) 
        {   
            int tid_offset = 4*tid;
            for (int j = 0; j < 4; ++j) 
            {   
                int vid = TT(tid,j);
                vertexDataPtr[tid_offset + j] = Eigen::Vector3f(TV(vid,0), TV(vid,1), TV(vid,2));
            }                
        }
        #if DEBUG_Init
        RA_LOG_INFO("[MuscleMesh::Init]vertexDataPtr %f", timer.getElapsedTime());
        #endif


        // generate normal texture buffer
        Eigen::Vector3f* normalDataPtr = new Eigen::Vector3f[numEntries]; 
        assert(N.rows() ==  4*TT.rows());

        for (int tid = 0; tid < TT.rows(); ++tid) 
        {   
            int tid_offset = 4*tid;
            for (int j = 0; j < TT.cols(); ++j) 
            {   
                int nid = 4*tid + j;
                normalDataPtr[tid_offset + j] = Eigen::Vector3f(N(nid,0), N(nid,1), N(nid,2));
            }                
        }
        #if DEBUG_Init
        RA_LOG_INFO("[MuscleMesh::Init]normalDataPtr %f", timer.getElapsedTime());
        #endif


        // generate diffused texture buffer
        // int numMusles = MV.cols(); // number of muscles plus skin and bones
        // assert(numMusles == 4);

        Eigen::Vector4f* diffusedDataPtr = new Eigen::Vector4f[4*TT.rows()]; 
        for (int tid = 0; tid < TT.rows(); ++tid) 
        {   
            // for (int j = 0; j < 4; ++j)
            // {
            //     Eigen::Vector4f tidval = Eigen::Vector4f(MV(TT(tid,0),j), 
            //                                              MV(TT(tid,1),j), 
            //                                              MV(TT(tid,2),j), 
            //                                              MV(TT(tid,3),j));            
            //     diffusedDataPtr[4*tid + j] = tidval;
            // }            

            // for (int j = 0; j < 16; ++j)
            // {
            //     Eigen::Vector4f tidval = Eigen::Vector4f(MV(tid,0),j), 
            //                                              MV(tid,1),j), 
            //                                              MV(tid,2),j), 
            //                                              MV(tid,3),j));    
                if (!isnew) 
                {        
                    diffusedDataPtr[4*tid + 0] = Eigen::Vector4f(MV(tid,0),MV(tid,1),MV(tid,2),MV(tid,3));
                    diffusedDataPtr[4*tid + 1] = Eigen::Vector4f(MV(tid,4),MV(tid,5),MV(tid,6),MV(tid,7));
                    diffusedDataPtr[4*tid + 2] = Eigen::Vector4f(MV(tid,8),MV(tid,9),MV(tid,10),MV(tid,11));
                    diffusedDataPtr[4*tid + 3] = Eigen::Vector4f(MV(tid,12),MV(tid,13),MV(tid,14),MV(tid,15)); 
                }
                else 
                {
                    diffusedDataPtr[4*tid + 0] = Eigen::Vector4f(1,1,1,1);
                    diffusedDataPtr[4*tid + 1] = Eigen::Vector4f(0,0,0,0);
                    diffusedDataPtr[4*tid + 2] = Eigen::Vector4f(0,0,0,0);
                    diffusedDataPtr[4*tid + 3] = Eigen::Vector4f(0,0,0,0); 
                }
            // }  
        }
        #if DEBUG_Init
        RA_LOG_INFO("[MuscleMesh::Init]diffusedDataPtr %f", timer.getElapsedTime());
        #endif

       
        // Eigen::Vector3f* gradDataPtr = new Eigen::Vector3f[TT.rows()]; 
        // for (int tid = 0; tid < TT.rows(); ++tid) 
        // {   
        //     // Eigen::Vector3f tidval = Eigen::Vector3f(G(tid,0), G(tid,1), G(tid,2));
        //     gradDataPtr[tid] = Eigen::Vector3f(0,0,0);//tidval;
        // }
        // assert(numEntries ==numMusles*TT.rows() );
    
        Eigen::Vector4f* uMuscleIDDataPtr = new Eigen::Vector4f[TT.rows()]; 
        for (int tid = 0; tid < TT.rows(); ++tid) 
        {   
            if (!isnew) 
            {
                Eigen::Vector4i tidval = MI.row(tid);
                uMuscleIDDataPtr[tid] = Eigen::Vector4f(tidval(0),tidval(1),tidval(2),tidval(3));//tidval; Eigen::Vector4f(5,5,5,5); // 
            }
            else 
            {
                uMuscleIDDataPtr[tid] = Eigen::Vector4f(0,-1,-1,-1);//tidval; Eigen::Vector4f(5,5,5,5); //             
            }
        }
        #if DEBUG_Init
        RA_LOG_INFO("[MuscleMesh::Init]uMuscleIDDataPtr %f", timer.getElapsedTime());
        #endif


        const int numColors = 20;
        float uMuscleColorDataPtr[numColors*3] = {
                0.75127,0.35166 , 0.16218,0.2551 ,  0.83083 , 0.79428,0.50596,  0.58526  ,  0.31122,0.69908,0.54972 , 0.52853,
                0.8909 ,0.91719 , 0.16565,0.95929,0.28584 , 0.60198,0.54722,0.7572  ,0.26297,0.13862,0.75373 , 0.65408,0.14929,0.38045 , 0.68921,0.25751,0.56782 , 0.74815,
                0.84072,0.075854,  0.45054,0.25428,0.05395 ,0.083821,0.81428,0.5308  ,0.22898,0.24352,0.77917 , 0.91334,0.92926,0.93401 , 0.15238,
                0.34998,0.12991 , 0.82582,0.1966 ,0.56882 , 0.53834,0.25108,0.46939 , 0.99613,0.61604,0.011902, 0.078176,0.47329,0.33712 , 0.44268};

        ES2BufferTextureSPtr vertexBufTex   = std::make_shared<ES2BufferTexture>(ES2BufferTexture::TY_RGB32F, numEntries, vertexDataPtr,   HardwareBuffer::Usage::BU_STATIC);  
        ES2BufferTextureSPtr normalBufTex   = std::make_shared<ES2BufferTexture>(ES2BufferTexture::TY_RGB32F, numEntries, normalDataPtr,   HardwareBuffer::Usage::BU_STATIC);  
        ES2BufferTextureSPtr diffusedBufTex = std::make_shared<ES2BufferTexture>(ES2BufferTexture::TY_RGBA32F, numEntries, diffusedDataPtr, HardwareBuffer::Usage::BU_STATIC);  
        // ES2BufferTextureSPtr gradBufTex     = std::make_shared<ES2BufferTexture>(ES2BufferTexture::TY_RGB32F, TT.rows(), gradDataPtr, HardwareBuffer::Usage::BU_STATIC);  
        ES2BufferTextureSPtr muscleBufTex   = std::make_shared<ES2BufferTexture>(ES2BufferTexture::TY_RGBA32F, TT.rows(), uMuscleIDDataPtr, HardwareBuffer::Usage::BU_STATIC);  
        ES2BufferTextureSPtr muscleColorBufTex   = std::make_shared<ES2BufferTexture>(ES2BufferTexture::TY_RGB32F, numColors, uMuscleColorDataPtr, HardwareBuffer::Usage::BU_STATIC);  


        GetVisualEffect()->AddBufferTexture(vertexBufTex);
        GetVisualEffect()->AddBufferTexture(normalBufTex);
        GetVisualEffect()->AddBufferTexture(diffusedBufTex);
        // GetVisualEffect()->AddBufferTexture(gradBufTex);
        GetVisualEffect()->AddBufferTexture(muscleBufTex);
        GetVisualEffect()->AddBufferTexture(muscleColorBufTex);


        delete[] vertexDataPtr;
        delete[] normalDataPtr;
        delete[] diffusedDataPtr;
        // delete[] gradDataPtr;
        delete[] uMuscleIDDataPtr;
        // delete[] uMuscleColorDataPtr;
        #if DEBUG_Init
        RA_LOG_INFO("[MuscleMesh::Init]total time %f", timer.getElapsedTime());
        #endif

    }
        
    MuscleMeshSPtr MuscleMesh::Create(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F,   
                                      const Eigen::MatrixXd& N, const Eigen::MatrixXd& MV,  
                                      const Eigen::MatrixXi& MI,
                                      const Eigen::MatrixXd& TV, const Eigen::MatrixXi& TT,
                                      const Eigen::VectorXi& FtoT,  const Eigen::MatrixXd& G, bool isnew) 
    {
        MuscleMeshSPtr ptr = std::make_shared<MuscleMesh>();
        ptr->Init(V,F,N,MV,MI,TV,TT,FtoT,G,isnew);
        return ptr;
    }

    void MuscleMesh::SetColor(const Eigen::Vector4f& color)
    {
        mColor = color;
    }

    void 
    MuscleMesh::UpdateDiffusion(const Eigen::MatrixXd& MV, const Eigen::MatrixXi& MI) 
    {   
        #define DEBUG_UpdateDiffusion 0
        #if DEBUG_UpdateDiffusion
        RA_LOG_INFO("UpdateDiffusion called");
        #endif
        assert(MV.size()>0);
        assert(MI.size()>0);
        Eigen::Vector4f* diffusedDataPtr = new Eigen::Vector4f[4*mNumTets]; 
        for (int tid = 0; tid < mNumTets; ++tid) 
        {   
            for (int j = 0; j < 4; ++j)
            {
                diffusedDataPtr[4*tid + 0] = Eigen::Vector4f(MV(tid,0),MV(tid,1),MV(tid,2),MV(tid,3));
                diffusedDataPtr[4*tid + 1] = Eigen::Vector4f(MV(tid,4),MV(tid,5),MV(tid,6),MV(tid,7));
                diffusedDataPtr[4*tid + 2] = Eigen::Vector4f(MV(tid,8),MV(tid,9),MV(tid,10),MV(tid,11));
                diffusedDataPtr[4*tid + 3] = Eigen::Vector4f(MV(tid,12),MV(tid,13),MV(tid,14),MV(tid,15));
            }            
        }
   
        Eigen::Vector4f* uMuscleIDDataPtr = new Eigen::Vector4f[mNumTets]; 
        for (int tid = 0; tid < mNumTets; ++tid) 
        {   
            Eigen::Vector4i tidval = MI.row(tid);
            uMuscleIDDataPtr[tid] = Eigen::Vector4f(tidval(0),tidval(1),tidval(2),tidval(3));//tidval; Eigen::Vector4f(5,5,5,5); //
        }
        // ES2BufferTextureSPtr diffusedBufTex = std::make_shared<ES2BufferTexture>(ES2BufferTexture::TY_RGBA32F, numEntries, diffusedDataPtr, HardwareBuffer::Usage::BU_STATIC);  
        GetVisualEffect()->GetBufferTexture(2)->WriteDataFull(diffusedDataPtr);
        GetVisualEffect()->GetBufferTexture(3)->WriteDataFull(uMuscleIDDataPtr);
        #if DEBUG_UpdateDiffusion
        RA_LOG_INFO("UpdateDiffusion exiting");
        #endif
    }


    void MuscleMesh::UpdateUniformsWithCamera(const tyro::Camera* camera)
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
        // GetVisualEffect()->GetUniforms()->UpdateFloatUniform(2, mColor.data());
        GetVisualEffect()->GetUniforms()->UpdateFloatUniform(2, modelViewMatrix.data());
        
        int vertexbuf = 0;
        GetVisualEffect()->GetUniforms()->UpdateIntUniform(3, &vertexbuf);
        
        int normtexbuf = 1;
        GetVisualEffect()->GetUniforms()->UpdateIntUniform(4, &normtexbuf);

        // GetVisualEffect()->GetUniforms()->UpdateFloatUniform(5, vport_f.data());
        // GetVisualEffect()->GetUniforms()->UpdateFloatUniform(5, projectionMatrix.data());

        int diffusedtexbuf = 2;
        GetVisualEffect()->GetUniforms()->UpdateIntUniform(5, &diffusedtexbuf);

        // int gradtexbuf = GetVisualEffect()->GetBufferTexture(3)->GetTextureID();
        // GetVisualEffect()->GetUniforms()->UpdateIntUniform(9, &gradtexbuf);

        int muscleIDbuf = 3;
        GetVisualEffect()->GetUniforms()->UpdateIntUniform(6, &muscleIDbuf);      


        int musclecolorIDbuf = 4;
        GetVisualEffect()->GetUniforms()->UpdateIntUniform(7, &musclecolorIDbuf);  

    }
}