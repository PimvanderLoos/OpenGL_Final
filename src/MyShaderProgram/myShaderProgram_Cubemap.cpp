#include "myShaderProgram_Cubemap.h"

MyShaderProgram_Cubemap::MyShaderProgram_Cubemap(QOpenGLFunctions_3_3_Core *const gl,
                                                        QString &fragmentShader, QString &vertexShader)
:
    MyShaderProgram(gl, fragmentShader, vertexShader)
{}

MyShaderProgram_Cubemap::MyShaderProgram_Cubemap(QOpenGLFunctions_3_3_Core *const gl,
                                                        QString &&fragmentShader, QString &&vertexShader)
:
    MyShaderProgram(gl, fragmentShader, vertexShader)
{}

void MyShaderProgram_Cubemap::paint(RenderData &renderData)
{
    d_gl->glDepthMask(GL_FALSE);

    QMatrix4x4 PMV = *renderData.projectionTransform;
    PMV.rotate(QQuaternion::fromEulerAngles(renderData.camera->getRotation()));
    d_gl->glUniformMatrix4fv(d_uniformPMVTransform, 1, GL_FALSE, PMV.data());
    d_gl->glBindVertexArray(renderData.skyBoxVAO);
    d_gl->glActiveTexture(GL_TEXTURE0);
    d_gl->glBindTexture(GL_TEXTURE_CUBE_MAP, renderData.skyBoxTexturePtr);
    d_gl->glDrawArrays(GL_TRIANGLES, 0, 36);
    d_gl->glBindVertexArray(0);

    d_gl->glDepthMask(GL_TRUE);
}