#ifndef GAME_MYSHADERPROGRAM_PHONG_H
#define GAME_MYSHADERPROGRAM_PHONG_H

#include "myShaderProgram.h"

class MyShaderProgram_Phong : public MyShaderProgram
{
    private:
        void init();

    protected:
        GLint d_uniformModelTransform;
        GLint      d_uniformClipPlane;
        GLint       d_uniformLightPos;
        GLint       d_uniformLightCol;
        GLint       d_uniformTextured;
        GLint     d_uniformUnderWater;
        GLint             d_uniformNS;
        GLint             d_uniformKA;
        GLint             d_uniformKD;
        GLint             d_uniformKS;
        GLint             d_uniformKE;
        GLint             d_uniformNI;
        GLint              d_uniformD;
        GLint         d_uniformMap_Kd;
        GLint         d_uniformMap_Ks;
        GLint       d_uniformMap_Bump;
        GLint     d_uniformIllumModel;
        GLint d_uniformCameraPosition;
        GLint                 d_debug;

    public:
        MyShaderProgram_Phong(QOpenGLFunctions_3_3_Core *const gl, QString &fragmentShader,  QString &vertexShader);
        MyShaderProgram_Phong(QOpenGLFunctions_3_3_Core *const gl, QString &&fragmentShader, QString &&vertexShader);

        virtual void paint(RenderData &renderData);
};

#endif
