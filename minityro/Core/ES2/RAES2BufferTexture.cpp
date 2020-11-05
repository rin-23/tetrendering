#include "RAES2BufferTexture.h"
#include "RALogManager.h"
namespace tyro 
{
        ES2BufferTexture::ES2BufferTexture() 
        :        
        mTex(-1),
        mBuf(-1)
        {}

        ES2BufferTexture::~ES2BufferTexture() 
        {   
            if (mTex>=0) 
            {
                glDeleteTextures(1, &mTex);
                GL_CHECK_ERROR; 
            }

            if (mBuf >=0)  
            {
                glDeleteBuffers(1,&mBuf);
                GL_CHECK_ERROR; 
            }
        }
       
        ES2BufferTexture::ES2BufferTexture(Format format, 
                                           int numElements,  
                                           const GLvoid* dataPtr, 
                                           HardwareBuffer::Usage usage) 
        {
            glGenBuffers(1, &mBuf);
            GL_CHECK_ERROR;
            glBindBuffer(GL_TEXTURE_BUFFER, mBuf);
            GL_CHECK_ERROR;
            mBytesize = GetGLByteSizes(format, numElements);
            glBufferData(GL_TEXTURE_BUFFER, mBytesize, dataPtr, GetGLUsage(usage));
            GL_CHECK_ERROR;

            glGenTextures(1, &mTex);
            GL_CHECK_ERROR;
            glBindTexture(GL_TEXTURE_BUFFER, mTex);
            GL_CHECK_ERROR;
            glTexBuffer(GL_TEXTURE_BUFFER, GetGLFormat(format), mBuf);
            GL_CHECK_ERROR;
        }

        void 
        ES2BufferTexture::WriteDataFull(const GLvoid* dataPtr) 
        {
            glBindBuffer(GL_TEXTURE_BUFFER, mBuf);
            GL_CHECK_ERROR;
            glBufferSubData(GL_TEXTURE_BUFFER, 0, mBytesize, dataPtr);
            GL_CHECK_ERROR;
        }

        GLenum ES2BufferTexture::GetGLUsage(HardwareBuffer::Usage usage) const
        {
            switch(usage)
            {
                case HardwareBuffer::BU_STATIC:
                    return GL_STATIC_DRAW;
                case HardwareBuffer::BU_DYNAMIC:
                    return GL_DYNAMIC_DRAW;
                default:
                    return GL_STATIC_DRAW;
            };
        }

        GLenum ES2BufferTexture::GetGLFormat(Format format) const 
        {
            if (format ==      TY_RGBA32F) 
                return  GL_RGBA32F;
            else if (format == TY_RGB32F) 
                return GL_RGB32F;

            else if (format == TY_RGBA32I) 
                return GL_RGBA32I;
            else if (format == TY_RGB32I) 
                return GL_RGB32I;            
            else 
            {
                RA_LOG_ERROR_ASSERT("unsupported format");
                return 0;
            }            
        }

        int ES2BufferTexture::GetGLByteSizes(Format format, int numElements) const 
        {   
            int numComponents=0;
            int typeSize;
            if (format == TY_RGBA32F) 
            {
                numComponents = 4;
                typeSize = sizeof(GLfloat);
            }
            else if (format == TY_RGB32F)  
            {
                numComponents = 3;
                typeSize = sizeof(GLfloat);
            }
            else if (format == TY_RGBA32I) 
            {
                numComponents = 4;
                typeSize = sizeof(GLint);
            }   
            else if (format == TY_RGB32I)  
            {
                numComponents = 3;
                typeSize = sizeof(GLint); 
            }

            int totalsize = numComponents * typeSize * numElements;
            return totalsize;
        }
}