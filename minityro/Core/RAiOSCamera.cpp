//
//  RAArcBallCamera.cpp
//  Ossa
//
//  Created by Rinat Abdrashitov on 2015-01-20.
//  Copyright (c) 2015 Rinat Abdrashitov. All rights reserved.
//

#include "RAiOSCamera.h"
#include "RAMath.h"
#include "RALogManager.h"
#include "Wm5HQuaternion.h"

using namespace Wm5;
#define EigenToAPoint(a) APoint(a(0),a(1),a(2))
#define EigenToWm5Vector(a) Wm5::Vector3f(a(0),a(1),a(2))
#define PointToEigen(a) Eigen::Vector3f(a[0],a[1],a[2])

namespace tyro
{
    //Orbiting camera araound worldCenter at radius
    iOSCamera::iOSCamera(const Eigen::Vector3f& worldCenter,
                         float radius,
                         float aspect,
                         float scale,
                         const Eigen::Vector4i& viewport,
                         bool isOrtho)
    :
    tyro::Camera(APoint(0,0,0), 
                 EigenToAPoint(worldCenter), 
                 viewport,
                 aspect, 
                 isOrtho)
     {
        // Wm5::APoint worldCenter(worldCenter1(0),worldCenter1(1),worldCenter1(2));

        mInitialPosition = worldCenter;
        mInitialPosition(2) -= (1.0 + radius);
        SetPosition(EigenToAPoint(mInitialPosition));
        LookAt(EigenToAPoint(worldCenter));
        mFov = 45.0f;

        mWorldCenter = worldCenter;
        mRadius = radius;
        mScale = scale;
        mInitialScale = scale;
        mTranslationMatrix.MakeIdentity();
        mAccumulatedTranslation.MakeIdentity();
        
        UpdateViewMatrix();
        UpdateProjectionMatrix();
    }
    
    iOSCamera::~iOSCamera() {}
    
    void iOSCamera::SetWorldCenter(const Eigen::Vector3f& worldCenter)
    {
        mWorldCenter = worldCenter;
        LookAt(EigenToAPoint(mWorldCenter));
    }
    
    void iOSCamera::SetRadius(float radius)
    {
        mRadius = radius;
        UpdateProjectionMatrix();
    }
    
    float iOSCamera::GetRadius() const
    {
        return mRadius;
    }

#pragma mark - Gesture handlers
	
    void iOSCamera::HandleOneFingerPanGesture(int state, Eigen::Vector2i glTouch)
    {    
        if (state == 0)
        {
            mLastLoc = glTouch;
        }
        else if (state == 1 || state == 2)
        {
            Eigen::Vector2i diff = glTouch - mLastLoc;
            float rotX =  Math::DegToRad(-1*diff[1]/3.0f); //because positive angle is clockwise
            float rotY =  Math::DegToRad(-1*diff[0]/3.0f);

            AVector xAxis = GetOrientation().Rotate(AVector::UNIT_X);
            AVector yAxis = GetOrientation().Rotate(AVector::UNIT_Y);

            HQuaternion xAxisRot(xAxis, rotX);
            HQuaternion yAxisRot(yAxis, rotY);
            AVector pivotToOrigin = APoint::ORIGIN - EigenToAPoint(mWorldCenter);
            HMatrix toOrigin(pivotToOrigin);
            HMatrix fromOrigin(-pivotToOrigin);

            Vector3f positiveX(1, 0, 0);
            Vector3f transVectorNormalized(diff(0), diff(1), 0);
            transVectorNormalized.Normalize();
            float angle = acosf(positiveX.Dot(transVectorNormalized));
            
            if (angle < Math::DegToRad(45.0) || angle > Math::DegToRad(135.0) )
            {
                HMatrix rot;
                yAxisRot.ToRotationMatrix(rot);
                APoint newPosition = fromOrigin * rot * toOrigin * GetPosition();
                Camera::SetPosition(newPosition);
                Camera::LookAt(EigenToAPoint(mWorldCenter));
            }
            else
            {
                HMatrix rot;
                xAxisRot.ToRotationMatrix(rot);
                APoint newPosition = fromOrigin * rot * toOrigin * GetPosition();
                Camera::SetPosition(newPosition);
                Camera::LookAt(EigenToAPoint(mWorldCenter));
            }
            
            mLastLoc = glTouch;
        } 
    }
    
    void iOSCamera::HandleTwoFingerPanGesture(int state, Eigen::Vector2i translation)
    {
        Vector3f screen(translation(0), translation(1), 0.0f);
        Vector3f world = Camera::ScreenToCamera(screen);
        world.Z() = 0;
        
        if (state == 0)
        {
            mStartPoint = PointToEigen(world);
        }
        else if (state == 1 || state == 2)
        {
            AVector diff(world - EigenToWm5Vector(mStartPoint));
            HMatrix temp(diff);
            mTranslationMatrix = temp * mAccumulatedTranslation;
            UpdateViewMatrix();
            
            if (state == 2)
            {
                mAccumulatedTranslation = mTranslationMatrix;
            }
        }
    }

    void iOSCamera::Translate(const Eigen::Vector2i& translation) 
    {   
        Vector3f screen1(0, 0, 0.0f);
        Vector3f world1 = Camera::ScreenToCamera(screen1);
        world1.Z() = 0;

        Vector3f screen2(translation(0), translation(1), 0.0f);
        Vector3f world2 = Camera::ScreenToCamera(screen2);
        world2.Z() = 0;
        
        AVector diff(world2-world1);
        HMatrix temp(diff);
        mTranslationMatrix = temp;
        UpdateViewMatrix();
    }

    /*
    void iOSCamera::HandleRotationGesture(UIRotationGestureRecognizer* sender)
    {
        if (sender.numberOfTouches != 2) {
            return;
        }
        
        GestureState state = GestureStateForRecognizer(sender);
        float rotation = sender.rotation;
        
        if (state == GestureState::Began)
        {
            mLastRot = rotation;
        }
        else if (state == GestureState::Changed || state == GestureState::Ended)
        {
            float rotZ = (rotation - mLastRot);
            AVector zAxis = GetOrientation().Rotate(AVector::UNIT_NEGATIVE_Z);
            HQuaternion zAxisRot(zAxis, -1*rotZ);
            Camera::Rotate(zAxisRot);
            mLastRot = rotation;
        }
    }
    */

    void iOSCamera::HandlePinchGesture(int state, Eigen::Vector2i glTouch, double offset)
    {
        mFov -= (float)offset;
        if (mFov < 1)
            mFov = 1;
        if (mFov > 200.0f)
            mFov = 200.0f;
        UpdateProjectionMatrix();            
    }
    

    void iOSCamera::UpdateProjectionMatrix()
    {
        // if (mIsOrtho)
        // {
            float scale = 1/mScale;
            float left = 0 - mRadius * mAspect * scale;
            float right = 0 + mRadius * mAspect * scale;
            float bottom = 0 - mRadius * scale;
            float top = 0 + mRadius * scale;
            
            // Camera::SetOrthoProjection(left, right, bottom, top, 1.0f, 1.0f + 100.0f * mRadius);
        // } else {
            //perpcl
            // float scale = 1/mScale;
            // float left = 0 - mRadius * mAspect * scale;
            // float right = 0 + mRadius * mAspect * scale;
            // float bottom = 0 - mRadius * scale;
            // float top = 0 + mRadius * scale;

            // float left = -2;
            // float right = 2;
            // float bottom = -2;
            // float top = 2;
            
            // Camera::SetPerpProjection(left, right, bottom, top, 0.0f, -2.0f );
            float near = 1;
            Camera::SetPerpProjection(mFov, mAspect, near, near + 100.0f*mRadius);

        // }
    }
    
    void iOSCamera::UpdateViewMatrix()
    {
        Camera::UpdateViewMatrix();
        mViewMatrix = mTranslationMatrix * mViewMatrix;
    }
    
    void iOSCamera::Reset()
    {
        mPosition = EigenToAPoint(mInitialPosition);
        mOrientation = HQuaternion::IDENTITY;
        mScale = mInitialScale;
        LookAt(EigenToAPoint(mWorldCenter));
        
        mTranslationMatrix.MakeIdentity();
        mAccumulatedTranslation.MakeIdentity();
        
        UpdateViewMatrix();
        UpdateProjectionMatrix();
    }
}