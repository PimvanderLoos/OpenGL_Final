
#ifndef GAME_MYSHADERPROGRAM_AABB_H
#define GAME_MYSHADERPROGRAM_AABB_H

#include "myShaderProgram.h"

class MyShaderProgram_AABB : public MyShaderProgram
{
    private:
        void init();

    protected:
        GLint d_uniformMatCol;

    public:
        MyShaderProgram_AABB(QOpenGLFunctions_3_3_Core *const gl, QString &fragmentShader,  QString &vertexShader);
        MyShaderProgram_AABB(QOpenGLFunctions_3_3_Core *const gl, QString &&fragmentShader, QString &&vertexShader);
        ~MyShaderProgram_AABB() = default;

        virtual void paint(RenderData &renderData);
};

#endif
