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
    class MeshSlice : public ES2TriMesh
    {
    public:
        MeshSlice() 
        {
            mClip = Eigen::Vector3f(0,0,0);
        }
        
        virtual ~MeshSlice() {}
        
        static MeshSliceSPtr Create(const Eigen::MatrixXd& V,  //triangular mesh vert
                                    const Eigen::MatrixXi& F,  //faces
                                    const Eigen::VectorXd& D); // scalar value for each vertex

        //Create this mesh by slicing a tet mesh defined by TV, TT and D. Slice with a plane
        //assume thie is along x-axis
        static MeshSliceSPtr CreateSliceTet(const Eigen::MatrixXd& TV, 
                                            const Eigen::MatrixXi& TT, 
                                            const Eigen::VectorXd& D,
                                            double isovalue);

        virtual void UpdateUniformsWithCamera(const Camera* camera) override;

        void setClip(const Eigen::Vector3f& clip) { mClip = clip;}
        
        ES2VisualEffectSPtr VisualEffect();
    protected:
        Eigen::Vector3f mClip;
        void Init(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F, const Eigen::VectorXd& D);
    };
}