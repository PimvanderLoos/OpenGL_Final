#include "myShaderProgram_AABB.h"

void MyShaderProgram_AABB::init()
{
    d_uniformMatCol = d_shaderProgram.uniformLocation("matCol");
}

MyShaderProgram_AABB::MyShaderProgram_AABB(QOpenGLFunctions_3_3_Core *const gl,
                                                  QString &fragmentShader, QString &vertexShader)
:
    MyShaderProgram(gl, fragmentShader, vertexShader)
{
    init();
}

MyShaderProgram_AABB::MyShaderProgram_AABB(QOpenGLFunctions_3_3_Core *const gl,
                                                  QString &&fragmentShader, QString &&vertexShader)
:
    MyShaderProgram(gl, fragmentShader, vertexShader)
{
    init();
}

void MyShaderProgram_AABB::paint(RenderData &renderData)
{
    // Temporarily disable culling and switch to GL_Line mode to draw the wireframe.
    d_gl->glDisable(GL_CULL_FACE);
    d_gl->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Set the projection matrix
    QMatrix4x4 PMV = *renderData.projectionTransform * *renderData.viewTransform * *renderData.modelTransform;

    d_gl->glUniform3f(d_uniformMatCol, 1, 1, 1);

    d_gl->glUniformMatrix4fv(d_uniformPMVTransform, 1, GL_FALSE, PMV.data());

    d_gl->glBindVertexArray(renderData.model->getSubModel(0).getMeshVAO());
    d_gl->glDrawArrays(GL_TRIANGLES, 0, renderData.model->getSubModel(0).getMeshSize());

    d_gl->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    d_gl->glEnable(GL_CULL_FACE);
}