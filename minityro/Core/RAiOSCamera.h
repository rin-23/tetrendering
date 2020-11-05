//
//  RAArcBallCamera.h
//  Ossa
//
//  Created by Rinat Abdrashitov on 2015-01-20.
//  Copyright (c) 2015 Rinat Abdrashitov. All rights reserved.
//

#pragma once

#include "RACamera.h"
#include "RAEnginePrerequisites.h"
#include <Eigen/Core>

namespace tyro
{
    class iOSCamera : public Camera
    {
    public:
        iOSCamera(const Eigen::Vector3f& worldCenter,
                  float radius,
                  float aspect,
                  float scale,
                  const Eigen::Vector4i& viewport,
                  bool isOrtho);
        
		virtual ~iOSCamera();

        void SetWorldCenter(const Eigen::Vector3f& worldCenter);
        void SetRadius(float radius);
        float GetRadius() const;
        void Reset();

        void HandleOneFingerPanGesture(int state, Eigen::Vector2i glTouch);
        void HandleTwoFingerPanGesture(int state, Eigen::Vector2i glTouch);
        //void HandleRotationGesture(UIRotationGestureRecognizer* sender);
        void HandlePinchGesture(int state, Eigen::Vector2i glTouch, double offset);
        void Translate(const Eigen::Vector2i& offset);
       
        //Eigen::Vector4f GetEigenViewport();
        //int WorldToScreen(UIGestureRecognizer* sender, float depth, Wm5::Vector3f& win);
        //int CameraToScreen(UIGestureRecognizer* sender, float depth, Wm5::Vector3f& win);
        //Wm5::Vector3f ScreenGestureToWorld(UIGestureRecognizer* sender, float depth) const;
        //Wm5::Vector3f ScreenGestureToCamera(UIGestureRecognizer* sender, float depth) const;
        
        //static GestureState GestureStateForRecognizer(UIGestureRecognizer* sender);
        //static Wm5::Vector2i TouchPointForGesture(UIGestureRecognizer* sender);

    protected:
        virtual void UpdateProjectionMatrix() override;
        virtual void UpdateViewMatrix() override;
        
        //static CGPoint ScaleTouchPoint(CGPoint touchPoint, UIView* view);
        
        Eigen::Vector3f mInitialPosition;
        Eigen::Vector3f mWorldCenter;
        float mInitialScale;
        float mRadius;

        //Rotation
        Eigen::Vector2i mLastLoc; //x,y axes rotaiton
        float mLastRot; //z axis rotation
        
        //Scale
        float mCurScale;
        float mScale;
        float mFov;
        Eigen::Vector3f mCentroid;
        // Wm5::Vector2f mInitialTouch;
        
        //Translation
        Eigen::Vector3f mStartPoint;
        Wm5::HMatrix mTranslationMatrix;
        Wm5::HMatrix mAccumulatedTranslation;
    };

    /*
    template<class Archive>
    void save(Archive & archive) const
    {
        archive(mInitialPosition);
        archive(mWorldCenter);

        archive(mInitialScale);
        archive(mRadius);
        
        archive(mLastLoc);
        archive(mLastRot);
        
        archive(mCurScale);
        archive(mScale);

        archive(mCentroid);
        archive(mInitialTouch);

        archive(mStartPoint);
        archive(mTranslationMatrix);
        archive(mAccumulatedTranslation);
    }
    
    template<class Archive>
    void load(Archive & archive)
    {
        archive(mInitialPosition);
        archive(mWorldCenter);

        archive(mInitialScale);
        archive(mRadius);
        
        archive(mLastLoc);
        archive(mLastRot);
        
        archive(mCurScale);
        archive(mScale);

        archive(mCentroid);
        archive(mInitialTouch);

        archive(mStartPoint);
        archive(mTranslationMatrix);
        archive(mAccumulatedTranslation);
    }
    */
}

