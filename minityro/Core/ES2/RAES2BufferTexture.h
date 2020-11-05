//
//  RAES2BufferTexture.h
//
//  Created by Rinat Abdrashitov on 2020-07-30.
//

#pragma once

#include <iostream>
#include <memory>
#include <GL/glew.h>
#include "RAHardwareBuffer.h"
#include "RAEnginePrerequisites.h"

namespace tyro
{
    class ES2BufferTexture 
    {
    public:

        enum Format 
        {   
            TY_RGB32F,
            TY_RGBA32F,
            TY_RGB32I,
            TY_RGBA32I
        };
        
        ES2BufferTexture();
       
        ES2BufferTexture(Format format, 
                         int numElements, // this is not size in byts but how many tformat sized entries there are
                         const GLvoid* dataPtr, 
                         HardwareBuffer::Usage usage);

        ~ES2BufferTexture();
       
        GLuint GetName() const;
        
        void Bind() const;
        
        void Unbind() const;

        GLuint GetTextureID() const
        {
            return mTex;
        }

        GLuint GetBufferID() const
        {
            return mBuf;
        }

        // virtual void* Map(LockOptions options) override;
        
        // virtual void* MapRead() const override;
        
        // virtual void* MapWrite() override;

        // virtual void Unmap() const override;
        
        // fully update the content of the data
        // pSource is assume to have the same length as original
        void WriteDataFull(const GLvoid* dataPtr);
        
        // void PrepareToDraw(GLuint index, GLint count, GLsizeiptr offset, GLenum type, GLboolean normalized) const;
        
    private:
        GLuint mTex;
        GLuint mBuf;
        int mBytesize;

        GLenum GetGLFormat(Format usage) const;
        GLenum GetGLUsage(HardwareBuffer::Usage usage) const;
        int GetGLByteSizes(Format format, int numElements) const;
        
        // void _SerializeInit();
    };
    
}
