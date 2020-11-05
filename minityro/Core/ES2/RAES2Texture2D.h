//
//  RAES2Texture.h
//  Ossa
//
//  Created by Rinat Abdrashitov on 2015-04-30.
//  Copyright (c) 2015 Rinat Abdrashitov. All rights reserved.
//

#pragma once

#include "RATexture.h"
//#include <OpenGLES/gltypes.h>
#include <GL/glew.h>
#include <memory>
#include "RAEnginePrerequisites.h"
#include <Eigen/Core>

namespace tyro
{
    class ES2Texture2D : public Texture
    {
    public:
        ES2Texture2D(TextureFormat format, int width, int height);
        ES2Texture2D(const std::string& imagepath, bool flip=0);
        ES2Texture2D(const Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic>& R,
                     const Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic>& G,
                     const Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic>& B,
                     const Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic>& A);

        virtual ~ES2Texture2D();
        
        void LoadData(const void* data);
        
        void LoadSubData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, const void* data);

        GLuint GetTextureID() const;
    private:
        GLuint mTextureID;
        static GLenum GetGLFormat(TextureFormat format);
        static GLenum GetGLDataType(TextureFormat format);
    };
}
