//
//  RAVisual.cpp
//  Ossa
//
//  Created by Rinat Abdrashitov on 2015-01-13.
//  Copyright (c) 2015 Rinat Abdrashitov. All rights reserved.
//

#include "RAES2Renderable.h"
#include "RAVisibleSet.h"

namespace tyro
{
    void ES2Renderable::Init(ES2PrimitiveType type)
    {
        Spatial::Init();
        mPrimitiveType = type;
    }
    
//    ES2RenderableSPtr ES2Renderable::Create(ES2PrimitiveType type)
//    {
//        ES2RenderableSPtr sptr = std::make_shared<ES2Renderable>();
//        sptr->Init(type);
//        return sptr;
//    }
		
    void ES2Renderable::UpdateBounds()
    {
        // WorldBoundBox = AxisAlignedBBox::TransformAffine(WorldTransform, LocalBoundBox);
        // APoint t = WorldTransform.GetTranslate();
        // Eigen::Vector3f m(t[0],t[1],t[2]);                                          
        WorldBoundBox = LocalBoundBox;
        // WorldBoundBox = LocalBoundBox.translate(m);
        // auto v1= LocalBoundBox.GetMinimum();
        // auto v2= LocalBoundBox.GetMaximum();
        // WorldBoundBox = Eigen::AlignedBox<float,3>(Eigen::Vector3f(v1[0], v1[1],v1[2]), Eigen::Vector3f(v2[0], v2[1],v2[2]));
    }
    
    void ES2Renderable::GetVisibleSet (tyro::VisibleSet* visibleSet)
    {
        if (this->Visible)
        {
            Spatial::GetVisibleSet(visibleSet);
            visibleSet->Insert(this);
        }
    }
    
    void ES2Renderable::GetHitProxies(tyro::VisibleSet* visibleSet)
    {}
}
