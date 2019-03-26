#include "myShaderProgram_Splash.h"

void MyShaderProgram_Splash::init()
{
    d_uniformLightPos = d_shaderProgram.uniformLocation("lightPos");
}

MyShaderProgram_Splash::MyShaderProgram_Splash(QOpenGLFunctions_3_3_Core *const gl,
                                                      QString &fragmentShader, QString &vertexShader)
:
    MyShaderProgram(gl, fragmentShader, vertexShader)
{
    init();
}

MyShaderProgram_Splash::MyShaderProgram_Splash(QOpenGLFunctions_3_3_Core *const gl,
                                                      QString &&fragmentShader, QString &&vertexShader)
:
    MyShaderProgram(gl, fragmentShader, vertexShader)
{
    init();
}

void MyShaderProgram_Splash::paint(RenderData &renderData)
{
    // Set the projection matrix
    d_gl->glUniformMatrix4fv(d_uniformPMVTransform,    1, GL_FALSE, renderData.PMV->data());
    d_gl->glUniformMatrix4fv(d_uniformNormalTransform, 1, GL_FALSE, renderData.normalTransform->data());

    d_gl->glUniform3f(d_uniformLightPos, renderData.light->pos.x(), renderData.light->pos.y(),
                      renderData.light->pos.z());

    d_gl->glActiveTexture(GL_TEXTURE0);
    d_gl->glBindTexture(GL_TEXTURE_2D, renderData.entity->getModel()->getSubModel(0).getMaterial().map_Kd.d_ptr);

    d_gl->glBindVertexArray(renderData.entity->getModel()->getSubModel(0).getMeshVAO());
    d_gl->glDrawArrays(GL_TRIANGLES, 0, renderData.entity->getModel()->getSubModel(0).getMeshSize());
}