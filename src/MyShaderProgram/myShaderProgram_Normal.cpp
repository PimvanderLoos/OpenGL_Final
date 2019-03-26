#include "myShaderProgram_Normal.h"

void MyShaderProgram_Normal::init()
{
    d_uniformClipPlane      = d_shaderProgram.uniformLocation("clipPlane");
    d_uniformModelTransform = d_shaderProgram.uniformLocation("modelTransform");
}

MyShaderProgram_Normal::MyShaderProgram_Normal(QOpenGLFunctions_3_3_Core *const gl,
                                                      QString &fragmentShader, QString &vertexShader)
:
    MyShaderProgram(gl, fragmentShader, vertexShader)
{
    init();
}

MyShaderProgram_Normal::MyShaderProgram_Normal(QOpenGLFunctions_3_3_Core *const gl,
                                                      QString &&fragmentShader, QString &&vertexShader)
:
    MyShaderProgram(gl, fragmentShader, vertexShader)
{
    init();
}

void MyShaderProgram_Normal::paint(RenderData &renderData)
{
    // Set the projection matrix
    d_gl->glUniformMatrix4fv(d_uniformPMVTransform,    1, GL_FALSE, renderData.PMV->data());
    d_gl->glUniformMatrix4fv(d_uniformModelTransform,  1, GL_FALSE, renderData.PMV->data());
    d_gl->glUniformMatrix3fv(d_uniformNormalTransform, 1, GL_FALSE, renderData.normalTransform->data());
    d_gl->glUniform4f(d_uniformClipPlane, renderData.clipPlane->x(), renderData.clipPlane->y(),
                      renderData.clipPlane->z(), renderData.clipPlane->w());
    for (size_t idx = 0; idx < renderData.entity->getModel()->getSubModelCount(); ++idx)
    {
        SubModel const *sub = &renderData.entity->getModel()->getSubModel(idx);
        d_gl->glBindVertexArray(sub->getMeshVAO());
        d_gl->glDrawArrays(GL_TRIANGLES, 0, sub->getMeshSize());
    }
}
