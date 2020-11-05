//
//  RAMuscleMesh.h
//  Ossa
//
//  Created by Rinat Abdrashitov on 2015-04-09.
//  Copyright (c) 2015 Rinat Abdrashitov. All rights reserved.
//

#pragma once

#include "RAEnginePrerequisites.h"
#include "RAES2TriMesh.h"
#include <Eigen/Core>

namespace tyro
{
    class TetMesh : public ES2TriMesh
    {
    public:
        TetMesh() 
        {
            mClip = Eigen::Vector3f(0,0,0);
        }
        
        virtual ~TetMesh() {}
        
        static TetMeshSPtr Create(const Eigen::MatrixXd& V, 
                                     const Eigen::MatrixXi& F, 
                                     const Eigen::MatrixXd& N);

        static TetMeshSPtr Create(const Eigen::MatrixXd& V, 
                                     const Eigen::MatrixXi& F);
        
        virtual void SetColor(const Eigen::Vector4f& color);

        virtual void UpdateUniformsWithCamera(const Camera* camera) override;

        void setClip(const Eigen::Vector3f& clip) { mClip = clip;}
        
        ES2VisualEffectSPtr VisualEffect();
    protected:
        Eigen::Vector4f mColor;
        Eigen::Vector3f mClip;
        int mNumTets;   

    
        void Init(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F, const Eigen::MatrixXd& N);
    };
}