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
    class MuscleMesh : public ES2TriMesh
    {
    public:
        MuscleMesh() {}
        
        virtual ~MuscleMesh() {}
        
        static MuscleMeshSPtr Create(const Eigen::MatrixXd& V, 
                                     const Eigen::MatrixXi& F, 
                                     const Eigen::MatrixXd& N, 
                                     const Eigen::MatrixXd& MV, 
                                     const Eigen::MatrixXi& MI,
                                     const Eigen::MatrixXd& TV, 
                                     const Eigen::MatrixXi& TT,
                                     const Eigen::VectorXi& FtoT,
                                     const Eigen::MatrixXd& G,
                                     bool isnew);
        
        virtual void SetColor(const Eigen::Vector4f& color);
        void SetEffect(int effect);

        virtual void UpdateDiffusion(const Eigen::MatrixXd& MV, const Eigen::MatrixXi& MI);
        
        virtual void UpdateUniformsWithCamera(const Camera* camera) override;

        ES2VisualEffectSPtr VisualEffect();
      
     
    protected:
        Eigen::Vector4f mColor;
        int mNumTets;   

    
        void Init(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F, const Eigen::MatrixXd& N, 
                  const Eigen::MatrixXd& MV, const Eigen::MatrixXi& MI, const Eigen::MatrixXd& TV, 
                  const Eigen::MatrixXi& TT, const Eigen::VectorXi& FtoT, const Eigen::MatrixXd& G, bool isnew);
    };
}