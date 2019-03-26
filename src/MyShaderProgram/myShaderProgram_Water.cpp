#include "myShaderProgram_Water.h"

void MyShaderProgram_Water::init()
{
    d_uniformClipPlane         = d_shaderProgram.uniformLocation("clipPlane");
    d_uniformModelTransform    = d_shaderProgram.uniformLocation("modelTransform");
    d_uniformModelTransform    = d_shaderProgram.uniformLocation("modelTransform");
    d_uniformLightPos          = d_shaderProgram.uniformLocation("lightPos");
    d_uniformLightCol          = d_shaderProgram.uniformLocation("lightCol");
    d_uniformClipPlane         = d_shaderProgram.uniformLocation("clipPlane");
    d_uniformModelTransform    = d_shaderProgram.uniformLocation("transformationMatrix");
    d_uniformProjTransform     = d_shaderProgram.uniformLocation("projectionMatrix");
    d_uniformViewTransform     = d_shaderProgram.uniformLocation("viewMatrix");
    d_uniformreflectiontex     = d_shaderProgram.uniformLocation("reflectionTexture");
    d_uniformrefractiontex     = d_shaderProgram.uniformLocation("refractionTexture");
    d_uniformDuDvMap           = d_shaderProgram.uniformLocation("dudvMap");
    d_uniformNormalMap         = d_shaderProgram.uniformLocation("normalMap");
    d_uniformDepthMap          = d_shaderProgram.uniformLocation("depthMap");
    d_uniformTime              = d_shaderProgram.uniformLocation("time");
    d_uniformCameraPosition    = d_shaderProgram.uniformLocation("cameraPosition");
}

MyShaderProgram_Water::MyShaderProgram_Water(QOpenGLFunctions_3_3_Core *const gl,
                                                    QString &fragmentShader, QString &vertexShader)
:
    MyShaderProgram(gl, fragmentShader, vertexShader)
{
    init();
}

MyShaderProgram_Water::MyShaderProgram_Water(QOpenGLFunctions_3_3_Core *const gl,
                                                    QString &&fragmentShader, QString &&vertexShader)
:
    MyShaderProgram(gl, fragmentShader, vertexShader)
{
    init();
}

void MyShaderProgram_Water::paint(RenderData &renderData)
{
    // Set the projection matrix
    d_gl->glUniformMatrix4fv(d_uniformPMVTransform,    1, GL_FALSE, renderData.PMV->data());
    d_gl->glUniformMatrix3fv(d_uniformNormalTransform, 1, GL_FALSE, renderData.normalTransform->data());
    d_gl->glUniformMatrix4fv(d_uniformModelTransform,  1, GL_FALSE, renderData.modelTransform->data());
    d_gl->glUniformMatrix4fv(d_uniformViewTransform,   1, GL_FALSE, renderData.viewTransform->data());
    d_gl->glUniformMatrix4fv(d_uniformProjTransform,   1, GL_FALSE, renderData.projectionTransform->data());

    // Phong stuff
    d_gl->glUniform3f(d_uniformLightPos, renderData.light->pos.x(), renderData.light->pos.y(),
                      renderData.light->pos.z());
    d_gl->glUniform3f(d_uniformLightCol, renderData.light->col.x(), renderData.light->col.y(),
                      renderData.light->col.z());
    d_gl->glUniform3f(d_uniformCameraPosition, renderData.camPos->x(), renderData.camPos->y(), renderData.camPos->z());

    d_gl->glUniform1f(d_uniformTime, renderData.time);

    d_gl->glUniform4f(d_uniformClipPlane, renderData.clipPlane->x(), renderData.clipPlane->y(),
                      renderData.clipPlane->z(), renderData.clipPlane->w());

    d_gl->glUniform1i(d_uniformrefractiontex, 0);
    d_gl->glUniform1i(d_uniformreflectiontex, 1);
    d_gl->glUniform1i(d_uniformDuDvMap, 2);
    d_gl->glUniform1i(d_uniformNormalMap, 3);
    d_gl->glUniform1i(d_uniformDepthMap, 4);

    d_gl->glActiveTexture(GL_TEXTURE0);
    d_gl->glBindTexture(GL_TEXTURE_2D, renderData.refractBufferTex);

    d_gl->glActiveTexture(GL_TEXTURE1);
    d_gl->glBindTexture(GL_TEXTURE_2D, renderData.reflectBufferTex);

    for (size_t idx = 0; idx != renderData.entity->getModel()->getSubModelCount(); ++idx)
    {
        d_gl->glActiveTexture(GL_TEXTURE2);
        d_gl->glBindTexture(GL_TEXTURE_2D, renderData.entity->getModel()->getSubModel(idx).getMaterial().map_Kd.d_ptr);

        d_gl->glActiveTexture(GL_TEXTURE3);
        d_gl->glBindTexture(GL_TEXTURE_2D,
                            renderData.entity->getModel()->getSubModel(idx).getMaterial().map_Bump.d_ptr);
    }

    d_gl->glActiveTexture(GL_TEXTURE4);
    d_gl->glBindTexture(GL_TEXTURE_2D, renderData.refractDepthTex);

    // Reset texture selector.
    d_gl->glActiveTexture(GL_TEXTURE0);

    d_gl->glEnable(GL_BLEND);
    d_gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (size_t idx = 0; idx != renderData.entity->getModel()->getSubModelCount(); ++idx)
    {
        d_gl->glBindVertexArray(renderData.entity->getModel()->getSubModel(idx).getMeshVAO());
        d_gl->glDrawArrays(GL_TRIANGLES, 0, renderData.entity->getModel()->getSubModel(idx).getMeshData().d_meshSize);
    }

    d_gl->glDisable(GL_BLEND);
}
