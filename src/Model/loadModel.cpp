#include "model.ih"

void Model::loadModel()
{
    loadAllMeshes();
    loadAllTextures();
    d_initStage = InitStage::DONE;
}

void Model::loadAllMeshes()
{
    MeshData md;
    for (size_t idx = 0; idx != getSubModelCount(); ++idx)
        generateVAOVBO(getSubModel(idx));

    for (size_t idx = 0; idx != getSubModelCount(); ++idx)
        getSubModel(idx).getMeshData().cleanup();
}

void Model::generateVAOVBO(SubModel &sm)
{
    // Generate VAO
    GLuint tmp;
    d_gl->glGenVertexArrays(1, &tmp);
    sm.setMeshVAO(tmp);
    d_gl->glBindVertexArray(sm.getMeshVAO());

    // Generate VBO
    d_gl->glGenBuffers(1, &tmp);
    sm.setMeshVBO(tmp);
    d_gl->glBindBuffer(GL_ARRAY_BUFFER, sm.getMeshVBO());

    // Write the data to the buffer
    d_gl->glBufferData(GL_ARRAY_BUFFER, sm.getMeshData().d_meshData->size() * sizeof(float),
                                        sm.getMeshData().d_meshData->data(), GL_STATIC_DRAW);

    // Set vertex coordinates to location 0
    d_gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    d_gl->glEnableVertexAttribArray(0);

    // Set colour coordinates to location 1
    d_gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    d_gl->glEnableVertexAttribArray(1);

    // Set texture coordinates to location 2
    d_gl->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    d_gl->glEnableVertexAttribArray(2);
    d_gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
    d_gl->glBindVertexArray(0);
}

void Model::loadAllTextures()
{
    for (size_t idx = 0; idx != getSubModelCount(); ++idx)
        loadTextures(getSubModel(idx));
}

void Model::loadTextures(SubModel &sm)
{
    loadTexture(sm.getMaterial().map_Kd);
    loadTexture(sm.getMaterial().map_Ks);
    loadTexture(sm.getMaterial().map_Bump);
}

void Model::loadTexture(TextureData &texData)
{
    if (texData.d_file == "NOTEXTURE" || texData.d_file == "")
        return;

    if (d_loadedTextures.find(texData.d_file.toStdString()) != d_loadedTextures.end())
    {
        texData.d_ptr = d_loadedTextures.find(texData.d_file.toStdString())->second;
        return;
    }

    GLfloat f;
    d_gl->glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&f);
    d_gl->glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,f);
    d_gl->glGenTextures(1, &texData.d_ptr);
    d_gl->glBindTexture(GL_TEXTURE_2D, texData.d_ptr);
    d_gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texData.d_imageWidth, texData.d_imageHeight, 0,
                       GL_RGBA, GL_UNSIGNED_BYTE, texData.d_textureData->data());
    d_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // use linear instead of mipmap

    d_loadedTextures.insert({texData.d_file.toStdString(), texData.d_ptr});

    texData.cleanup();
}
