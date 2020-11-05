//
//  RATextureBuffer.cpp
//  Ossa
//
//  Created by Rinat Abdrashitov on 2015-03-17.
//  Copyright (c) 2015 Rinat Abdrashitov. All rights reserved.
//

#include "RATextureBuffer.h"
#include <cstdlib>
#include "RALogManager.h"
//#include <easyppm/easyppm.h>

namespace tyro
{
    TextureBuffer::TextureBuffer(int format, int type, void* data, int width, int height)
    {
        mFormat = format;
        mType = type;
        mData = data;
        mWidth = width;
        mHeight = height;
        mNumOfBytes = calculateNumOfBytes();
    }
    
    TextureBuffer::~TextureBuffer()
    {
        if (mData) {
            free(mData);
        }
    };

    Eigen::Matrix<unsigned char,4,1> TextureBuffer::GetColor(const Eigen::Vector2i& touchPoint) const
    {
        if (mType != TB_UNSIGNED_BYTE || mFormat != TF_RGBA) 
        {
            RA_LOG_WARN("Unsuported texture format");
            return Eigen::Matrix<unsigned char,4,1>(0, 0, 0, 0);
        }
        
        int x = touchPoint(0);
        int y = touchPoint(1);
    
        int index = y * mWidth + x;
        int index4 = index * 4;
        
        uint8* pixels = (uint8*)mData;
        if (index4 + 3 < mNumOfBytes && index4 > 0) {
            
            uint8 r = pixels[index4];
            uint8 g = pixels[index4 + 1];
            uint8 b = pixels[index4 + 2];
            uint8 a = pixels[index4 + 3];
            
            return Eigen::Matrix<unsigned char,4,1>(r, g, b, a);
        }
        RA_LOG_WARN("color index is out of bounds");
        return Eigen::Matrix<unsigned char,4,1>(0, 0, 0, 0);
    }
    
    
    int TextureBuffer::calculateNumOfBytes()
    {
        int typeSize = 0;
        if (mType == TB_UNSIGNED_BYTE) {
            typeSize = 1;
        }
        
        int formatSize = 0;
        if (mFormat == TF_RGBA) {
            formatSize = 4;
        }
        
        return typeSize * formatSize * mWidth * mHeight;
    }
}