#ifndef GAME_MYSHADERPROGRAM_H
#define GAME_MYSHADERPROGRAM_H

#include <QString>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_3_3_Core>
#include <QMatrix4x4>
#include <QMatrix3x3>
#include <QVector3D>
#include <QVector4D>

#include <cstdarg>

#include "../TextureData/textureData.h"
#include "../Entity/entity.h"
#include "../Camera/camera.h"

struct Light
{
    QVector3D pos;
    QVector3D col;
};

struct RenderData
{
    QMatrix4x4 const *PMV;
    QMatrix3x3 const *normalTransform;
    QMatrix4x4 const *projectionTransform;
    Entity     const *entity;
    QVector4D  const *clipPlane;
    QMatrix4x4 const *modelTransform;
    QMatrix4x4 const *viewTransform;
    QVector3D  const *camPos;
    Light      const *light;
    Camera     const *camera;
    Model      const *model;
    unsigned int refractBufferTex;
    unsigned int refractDepthTex;
    unsigned int reflectBufferTex;
    unsigned int textured;
    float time;

    unsigned int skyBoxVAO;
    unsigned int skyBoxTexturePtr;

    bool diffMap = true;
    bool specMap = true;
    bool bumpMap = true;
};

class MyShaderProgram
{
    private:
        void init();

    protected:
        QOpenGLShaderProgram d_shaderProgram;
        QOpenGLFunctions_3_3_Core *d_gl = nullptr;

        GLint d_uniformPMVTransform;
        GLint d_uniformNormalTransform;

    public:
        MyShaderProgram(QOpenGLFunctions_3_3_Core *const gl, QString &&fragmentShader, QString &&vertexShader);
        MyShaderProgram(QOpenGLFunctions_3_3_Core *const gl, QString &fragmentShader,  QString &vertexShader);

        virtual void paint(RenderData &renderData) = 0;
        virtual ~MyShaderProgram();

        void bind();
        void release();
};

inline MyShaderProgram::MyShaderProgram(QOpenGLFunctions_3_3_Core *const gl,
                                        QString &&fragmentShader, QString &&vertexShader)
:
    d_gl(gl)
{
    d_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,   vertexShader);
    d_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShader);
    d_shaderProgram.link();

    init();
}

inline MyShaderProgram::MyShaderProgram(QOpenGLFunctions_3_3_Core *const gl,
                                        QString &fragmentShader, QString &vertexShader)
:
    d_gl(gl)
{

    qDebug() << "Loading vertex shader:" << vertexShader << "and fragmentShader:" << fragmentShader;

    d_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,   vertexShader);
    d_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShader);
    d_shaderProgram.link();

    init();
}

inline void MyShaderProgram::init()
{
    d_uniformPMVTransform    = d_shaderProgram.uniformLocation("pmvTransform");
    d_uniformNormalTransform = d_shaderProgram.uniformLocation("normalTransform");
}

inline void MyShaderProgram::bind()
{
    d_shaderProgram.bind();
}

inline void MyShaderProgram::release()
{
    d_shaderProgram.release();
}

inline MyShaderProgram::~MyShaderProgram()
{

}

#endif