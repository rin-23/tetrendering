//
//  RAES2Texture.cpp
//  Ossa
//
//  Created by Rinat Abdrashitov on 2015-04-30.
//  Copyright (c) 2015 Rinat Abdrashitov. All rights reserved.
//

#include "RAES2Texture2D.h"
//#include <OpenGLES/ES2/gl.h>
//#include <OpenGLES/ES2/glext.h>
#include <GL/glew.h>
#include "RALogManager.h"
#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace tyro
{

ES2Texture2D::ES2Texture2D(TextureFormat format, int width, int height)
:
Texture(format, TextureType::TT_2D)
{
    mWidth = width;
    mHeight = height;
}

ES2Texture2D::ES2Texture2D(const std::string& imagepath, bool flip)
:
Texture()
{   
    mType = TextureType::TT_2D;
    int nrChannels;
    if (flip)
        stbi_set_flip_vertically_on_load(1);  
    else
        stbi_set_flip_vertically_on_load(0); 

    unsigned char *data = stbi_load(imagepath.c_str(), &mWidth, &mHeight, &nrChannels, 0); 
    
    if (nrChannels==3) 
    {
       mFormat = TextureFormat::TF_R8G8B8;     
    }
    else if (nrChannels==4) 
    {
       mFormat = TextureFormat::TF_R8G8B8A8;     
    }
    else 
    {
        RA_LOG_ERROR_ASSERT("Only supporting 3 or 4 channels")
    }

    LoadData(data);
    stbi_image_free(data);
}

ES2Texture2D::ES2Texture2D(const Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic>& R,
                const Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic>& G,
                const Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic>& B,
                const Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic>& A) 
{

    mWidth = R.rows();
    mHeight = R.cols();

    unsigned char *data;
    mFormat = TextureFormat::TF_R8G8B8A8;  
    data = (unsigned char *) malloc(R.size()*4);
    for (unsigned i=0;i<R.size();++i)
    {
      data[i*4+0] = R(i);
      data[i*4+1] = G(i);
      data[i*4+2] = B(i);
      data[i*4+3] = A(i);
    }
    LoadData(data);
    free(data);
}

ES2Texture2D::~ES2Texture2D()
{
    glDeleteTextures(1, &mTextureID);
    GL_CHECK_ERROR;
}

void ES2Texture2D::LoadData(const void* data)
{
    glGenTextures(1, &mTextureID);
    GL_CHECK_ERROR;

    glActiveTexture(GL_TEXTURE0);
    GL_CHECK_ERROR;
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    GL_CHECK_ERROR;

    // Set the filtering mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GL_CHECK_ERROR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GL_CHECK_ERROR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    GL_CHECK_ERROR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GL_CHECK_ERROR;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GL_CHECK_ERROR;

    GLenum glFormat = GetGLFormat(mFormat);
    GLenum glType = GetGLDataType(mFormat);
    glTexImage2D(GL_TEXTURE_2D, 0, glFormat, mWidth, mHeight, 0, glFormat, glType, data); 
    GL_CHECK_ERROR;

 
    
    glBindTexture(GL_TEXTURE_2D, 0);
    GL_CHECK_ERROR;
}

void ES2Texture2D::LoadSubData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, const void* data)
{
    glActiveTexture(GL_TEXTURE0);
    GL_CHECK_ERROR;
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    GL_CHECK_ERROR;

    GLenum glFormat = GetGLFormat(mFormat);
    GLenum glType = GetGLDataType(mFormat);
    
    glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, width, height, glFormat, glType, data);
    GL_CHECK_ERROR;
}



GLuint ES2Texture2D::GetTextureID() const
{
    return mTextureID;
}

GLenum ES2Texture2D::GetGLFormat(TextureFormat format)
{
    switch (format)
    {
        case Texture::TF_R8:
            return GL_RED;
        case Texture::TF_A8:
            return GL_ALPHA;
        case Texture::TF_L8:
            return GL_LUMINANCE;
        case Texture::TF_A8L8:
            return GL_LUMINANCE_ALPHA;
        case Texture::TF_R8G8B8:
            return GL_RGB;
        case Texture::TF_R8G8B8A8:
            return GL_RGBA;
        default:
            assert(false);
            return 0;
    }
}

GLenum ES2Texture2D::GetGLDataType(TextureFormat format)
{
    switch (format)
    {
        case Texture::TF_R8:
            return GL_UNSIGNED_BYTE;
        case Texture::TF_A8:
            return GL_UNSIGNED_BYTE;
        case Texture::TF_L8:
            return GL_UNSIGNED_BYTE;
        case Texture::TF_A8L8:
            return GL_UNSIGNED_BYTE;
        case Texture::TF_R8G8B8:
            return GL_UNSIGNED_BYTE;
        case Texture::TF_R8G8B8A8:
            return GL_UNSIGNED_BYTE;
        default:
            assert(false);
            return 0;
    }
}
}