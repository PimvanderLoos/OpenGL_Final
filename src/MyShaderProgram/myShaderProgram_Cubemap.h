#ifndef GAME_MYSHADERPROGRAM_CUBEMAP_H
#define GAME_MYSHADERPROGRAM_CUBEMAP_H

#include "myShaderProgram.h"

class MyShaderProgram_Cubemap : public MyShaderProgram
{
    public:
        MyShaderProgram_Cubemap(QOpenGLFunctions_3_3_Core *const gl, QString &fragmentShader,  QString &vertexShader);
        MyShaderProgram_Cubemap(QOpenGLFunctions_3_3_Core *const gl, QString &&fragmentShader, QString &&vertexShader);

        virtual void paint(RenderData &renderData);
};

#endif
