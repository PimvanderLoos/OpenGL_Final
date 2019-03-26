#ifndef GAME_MYSHADERPROGRAM_WATER_H
#define GAME_MYSHADERPROGRAM_WATER_H

#include "myShaderProgram.h"

class MyShaderProgram_Water : public MyShaderProgram
{
    private:
        void init();

    protected:
        GLint           d_uniformTime;
        GLint        d_uniformDuDvMap;
        GLint       d_uniformLightPos;
        GLint       d_uniformLightCol;
        GLint       d_uniformDepthMap;
        GLint      d_uniformClipPlane;
        GLint      d_uniformNormalMap;
        GLint  d_uniformProjTransform;
        GLint  d_uniformViewTransform;
        GLint  d_uniformreflectiontex;
        GLint  d_uniformrefractiontex;
        GLint d_uniformCameraPosition;
        GLint d_uniformModelTransform;

    public:
        MyShaderProgram_Water(QOpenGLFunctions_3_3_Core *const gl, QString &fragmentShader,  QString &vertexShader);
        MyShaderProgram_Water(QOpenGLFunctions_3_3_Core *const gl, QString &&fragmentShader, QString &&vertexShader);

        virtual void paint(RenderData &renderData);
};

#endif
