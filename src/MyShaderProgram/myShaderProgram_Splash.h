#ifndef GAME_MYSHADERPROGRAM_SPLASH_H
#define GAME_MYSHADERPROGRAM_SPLASH_H

#include "myShaderProgram.h"

class MyShaderProgram_Splash : public MyShaderProgram
{
    private:
        void init();

    protected:
        GLint d_uniformLightPos;

    public:
        MyShaderProgram_Splash(QOpenGLFunctions_3_3_Core *const gl, QString &fragmentShader,  QString &vertexShader);
        MyShaderProgram_Splash(QOpenGLFunctions_3_3_Core *const gl, QString &&fragmentShader, QString &&vertexShader);

        virtual void paint(RenderData &renderData);
};

#endif
