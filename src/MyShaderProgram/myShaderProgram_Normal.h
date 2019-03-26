#ifndef GAME_MYSHADERPROGRAM_NORMAL_H
#define GAME_MYSHADERPROGRAM_NORMAL_H

#include "myShaderProgram.h"

class MyShaderProgram_Normal : public MyShaderProgram
{
    private:
        void init();

    protected:
        GLint d_uniformModelTransform;
        GLint      d_uniformClipPlane;

    public:
        MyShaderProgram_Normal(QOpenGLFunctions_3_3_Core *const gl, QString &fragmentShader,  QString &vertexShader);
        MyShaderProgram_Normal(QOpenGLFunctions_3_3_Core *const gl, QString &&fragmentShader, QString &&vertexShader);

        virtual void paint(RenderData &renderData);
};

#endif
