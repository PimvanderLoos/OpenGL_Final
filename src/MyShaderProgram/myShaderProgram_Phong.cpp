#include "myShaderProgram_Phong.h"

void MyShaderProgram_Phong::init()
{
    d_uniformClipPlane      = d_shaderProgram.uniformLocation("clipPlane");
    d_uniformModelTransform = d_shaderProgram.uniformLocation("modelTransform");
    d_uniformLightPos       = d_shaderProgram.uniformLocation("lightPos");
    d_uniformLightCol       = d_shaderProgram.uniformLocation("lightCol");
    d_uniformTextured       = d_shaderProgram.uniformLocation("textured");
    d_uniformUnderWater     = d_shaderProgram.uniformLocation("underWater");
    d_uniformNS             = d_shaderProgram.uniformLocation("ns");
    d_uniformKA             = d_shaderProgram.uniformLocation("ka");
    d_uniformKD             = d_shaderProgram.uniformLocation("kd");
    d_uniformKS             = d_shaderProgram.uniformLocation("ks");
    d_uniformKE             = d_shaderProgram.uniformLocation("ke");
    d_uniformNI             = d_shaderProgram.uniformLocation("ne");
    d_uniformD              = d_shaderProgram.uniformLocation("d");
    d_uniformMap_Kd         = d_shaderProgram.uniformLocation("map_Kd");
    d_uniformMap_Ks         = d_shaderProgram.uniformLocation("map_Ks");
    d_uniformMap_Bump       = d_shaderProgram.uniformLocation("map_Bump");
    d_uniformIllumModel     = d_shaderProgram.uniformLocation("illumModel");
    d_uniformCameraPosition = d_shaderProgram.uniformLocation("cameraPosition");
    d_debug                 = d_shaderProgram.uniformLocation("debug");
}

MyShaderProgram_Phong::MyShaderProgram_Phong(QOpenGLFunctions_3_3_Core *const gl,
                                                    QString &fragmentShader, QString &vertexShader)
:
    MyShaderProgram(gl, fragmentShader, vertexShader)
{
    init();
}

MyShaderProgram_Phong::MyShaderProgram_Phong(QOpenGLFunctions_3_3_Core *const gl,
                                                    QString &&fragmentShader, QString &&vertexShader)
:
    MyShaderProgram(gl, fragmentShader, vertexShader)
{
    init();
}

void MyShaderProgram_Phong::paint(RenderData &renderData)
{
    // Set the projection matrix
    d_gl->glUniformMatrix4fv(d_uniformPMVTransform,    1, GL_FALSE, renderData.PMV->data());
    d_gl->glUniformMatrix4fv(d_uniformModelTransform,  1, GL_FALSE, renderData.modelTransform->data());
    d_gl->glUniformMatrix3fv(d_uniformNormalTransform, 1, GL_FALSE, renderData.normalTransform->data());

    d_gl->glUniform1ui(d_debug, 0);

    d_gl->glUniform3f(d_uniformLightCol, renderData.light->col.x(), renderData.light->col.y(),
                      renderData.light->col.z());

    d_gl->glUniform3f(d_uniformLightPos, renderData.light->pos.x(), renderData.light->pos.y(),
                      renderData.light->pos.z());

    d_gl->glUniform3f(d_uniformCameraPosition, renderData.camPos->x(), renderData.camPos->y(),
                      renderData.camPos->z());

    d_gl->glUniform4f(d_uniformClipPlane, renderData.clipPlane->x(), renderData.clipPlane->y(),
                      renderData.clipPlane->z(), renderData.clipPlane->w());

    for (size_t idx = 0; idx != renderData.entity->getModel()->getSubModelCount(); ++idx)
    {
        SubModel const &sm = renderData.entity->getModel()->getSubModel(idx);
        Material const &mat = sm.getMaterial();

        d_gl->glUniform1f(d_uniformNS, mat.ns);
        d_gl->glUniform3f(d_uniformKA, mat.ka.x(), mat.ka.y(), mat.ka.z());
        d_gl->glUniform3f(d_uniformKD, mat.kd.x(), mat.kd.y(), mat.kd.z());
        d_gl->glUniform3f(d_uniformKS, mat.ks.x(), mat.ks.y(), mat.ks.z());
        d_gl->glUniform3f(d_uniformKE, mat.ke.x(), mat.ke.y(), mat.ke.z());
        d_gl->glUniform1f(d_uniformNI, mat.ni);
        d_gl->glUniform1f(d_uniformD,  mat.d);
        d_gl->glUniform1ui(d_uniformIllumModel, mat.illumModel);

        d_gl->glUniform1i(d_uniformMap_Kd, 0);
        d_gl->glUniform1i(d_uniformMap_Ks, 1);
        d_gl->glUniform1i(d_uniformMap_Bump, 2);

        unsigned int newflag = 0;
        if (renderData.diffMap && (sm.d_texturedflag & TEXTURED_KD))
            newflag |= TEXTURED_KD;
        if (renderData.specMap && (sm.d_texturedflag & TEXTURED_KS))
            newflag |= TEXTURED_KS;
        if (renderData.bumpMap && (sm.d_texturedflag & TEXTURED_BUMP))
            newflag |= TEXTURED_BUMP;

        d_gl->glUniform1ui(d_uniformTextured, newflag);

        if (sm.d_texturedflag & TEXTURED_KD && renderData.diffMap)
        {
            d_gl->glActiveTexture(GL_TEXTURE0);
            d_gl->glBindTexture(GL_TEXTURE_2D, sm.getMaterial().map_Kd.d_ptr);
        }
        if (sm.d_texturedflag & TEXTURED_KS && renderData.specMap)
        {
            d_gl->glActiveTexture(GL_TEXTURE1);
            d_gl->glBindTexture(GL_TEXTURE_2D, sm.getMaterial().map_Ks.d_ptr);
        }
        if (sm.d_texturedflag & TEXTURED_BUMP && renderData.bumpMap)
        {
            d_gl->glActiveTexture(GL_TEXTURE2);
            d_gl->glBindTexture(GL_TEXTURE_2D, sm.getMaterial().map_Bump.d_ptr);
        }

        d_gl->glUniform1i(d_uniformUnderWater, 0);

        d_gl->glBindVertexArray(sm.getMeshVAO());
        d_gl->glDrawArrays(GL_TRIANGLES, 0, sm.getMeshSize());
    }

    // Reset selector
    d_gl->glActiveTexture(GL_TEXTURE0);
}