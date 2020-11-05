//
//  RAES2TriMesh.cpp
//  Ossa
//
//  Created by Rinat Abdrashitov on 2015-01-14.
//  Copyright (c) 2015 Rinat Abdrashitov. All rights reserved.
//

#include "RAES2TriMesh.h"
#include "RAVertexBufferAccessor.h"
#include <Eigen/Core>

namespace tyro
{
    void ES2TriMesh::Init()
    {
        ES2Renderable::Init(PT_TRIANGLES);
    }
    
//    ES2TriMeshSPtr ES2TriMesh::Create()
//    {
//        ES2TriMeshSPtr ptr = std::make_shared<ES2TriMesh>();
//        ptr->Init();
//        return ptr;
//    }
    
    int ES2TriMesh::GetNumTriangles() const
    {
        if (GetIndexBuffer())
        {
            return GetIndexBuffer()->GetNumIndexes()/3;
        }
        else
        {
            return GetVertexBuffer()->GetNumOfVerticies()/3;
        }
    }
    
    // void ES2TriMesh::SetLocalBoundBoxFromVertexBuffer()
    // {
    //     if (GetVertexBuffer() == nullptr ||
    //         GetVisualEffect() == nullptr ||
    //         GetVisualEffect()->GetVertexFormat() == nullptr)
    //     {
    //         RA_LOG_ERROR_ASSERT("Vertex Buffer must not be null");
    //         return;
    //     }
        
    //     VertexBufferAccessor vba(this);
    //     vba.MapRead();
    //     if (!vba.HasPosition())
    //     {
    //         vba.Unmap();
    //         RA_LOG_ERROR_ASSERT("Need position information");
    //         return;
    //     }
        
    //     Eigen::Vector3f vmin, vmax;
        
    //     if (GetIndexBuffer())
    //     {
    //         unsigned int* indexData = (unsigned int*) GetIndexBuffer()->MapRead();
            
    //         for (int i = 0; i < GetIndexBuffer()->GetNumIndexes(); ++i)
    //         {
    //             Eigen::Vector3f pos = vba.Position<Eigen::Vector3f>(indexData[i]);
                
    //             if (i == 0)
    //             {
    //                 vmin = pos;
    //                 vmax = vmin;
    //             }
                
    //             vmin = Eigen::Vector3f(fmin(vmin(0), pos(0)),fmin(vmin(1), pos(1)),fmin(vmin(2), pos(2)));
    //             vmax = Eigen::Vector3f(fmax(vmax(0), pos(0)),fmax(vmax(1), pos(1)),fmax(vmax(2), pos(2)));
    //         }
            
    //         GetIndexBuffer()->Unmap();
    //     }
    //     else
    //     {
    //         for (int i = 0; i < GetVertexBuffer()->GetNumOfVerticies(); ++i)
    //         {
    //             Eigen::Vector3f pos = vba.Position<Eigen::Vector3f>(i);
                
    //             if (i == 0)
    //             {
    //                 vmin = pos;
    //                 vmax = vmin;
    //             }
                
    //             vmin = Eigen::Vector3f(fmin(vmin(0), pos(0)),fmin(vmin(1), pos(1)),fmin(vmin(2), pos(2)));
    //             vmax = Eigen::Vector3f(fmax(vmax(0), pos(0)),fmax(vmax(1), pos(1)),fmax(vmax(2), pos(2)));
    //         }
    //     }
        
    //     vba.Unmap();
        
    //      Vector3f vmin1(vmin(0),vmin(1),vmin(2));
    //     Vector3f vmax1(vmax(0),vmax(1),vmax(2));
    //     LocalBoundBox = AxisAlignedBBox(vmin1, vmax1);
    // }
    
//    void ES2TriMesh::UpdateModelNormals()
//    {
//        // Calculate normals from vertices by weighted averages of facet planes
//        // that contain the vertices.
//        VertexBufferAccessor vba(GetVisualEffect()->GetVertexFormat(), GetVertexBuffer().get());
//        vba.Map(HardwareBuffer::BL_READ_WRITE);
//        const int numVertices = GetVertexBuffer()->GetNumOfVerticies();
//        int i;
//        for (i = 0; i < numVertices; ++i)
//        {
//            vba.Normal<Float3>(i) = Float3(0.0f, 0.0f, 0.0f);
//        }
//        
//        const int numTriangles = GetNumTriangles();
//        for (i = 0; i < numTriangles; ++i)
//        {
//            // Get the vertex indices for the triangle.
//            int v0, v1, v2;
//            if (!GetTriangle(i, v0, v1, v2))
//            {
//                continue;
//            }
//            
//            // Get the vertex positions.
//            APoint pos0 = vba.Position<Float3>(v0);
//            APoint pos1 = vba.Position<Float3>(v1);Eigne
//            APoint pos2 = vba.Position<Float3>(v2);
//            
//            // Compute the triangle normal.  The length of this normal is used in
//            // the weighted sum of normals.
//            AVector triEdge1 = pos1 - pos0;
//            AVector triEdge2 = pos2 - pos0;
//            AVector triNormal = triEdge1.Cross(triEdge2);
//            
//            // Add the triangle normal to the vertices' normal sums.
//            vba.Normal<AVector>(v0) += triNormal;
//            vba.Normal<AVector>(v1) += triNormal;
//            vba.Normal<AVector>(v2) += triNormal;
//        }
//        
//        // The vertex normals must be unit-length vectors.
//        for (i = 0; i < numVertices; ++i)
//        {
//            vba.Normal<AVector>(i).Normalize();
//        }
//        vba.Unmap();
//    }
}