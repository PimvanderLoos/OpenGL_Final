#include "mainview.ih"

void MainView::LoadCubeMap()
{
    float points[] =
    {
        -10.0f,  10.0f, -10.0f,
        -10.0f, -10.0f, -10.0f,
         10.0f, -10.0f, -10.0f,
         10.0f, -10.0f, -10.0f,
         10.0f,  10.0f, -10.0f,
        -10.0f,  10.0f, -10.0f,

        -10.0f, -10.0f,  10.0f,
        -10.0f, -10.0f, -10.0f,
        -10.0f,  10.0f, -10.0f,
        -10.0f,  10.0f, -10.0f,
        -10.0f,  10.0f,  10.0f,
        -10.0f, -10.0f,  10.0f,

         10.0f, -10.0f, -10.0f,
         10.0f, -10.0f,  10.0f,
         10.0f,  10.0f,  10.0f,
         10.0f,  10.0f,  10.0f,
         10.0f,  10.0f, -10.0f,
         10.0f, -10.0f, -10.0f,

        -10.0f, -10.0f,  10.0f,
        -10.0f,  10.0f,  10.0f,
         10.0f,  10.0f,  10.0f,
         10.0f,  10.0f,  10.0f,
         10.0f, -10.0f,  10.0f,
        -10.0f, -10.0f,  10.0f,

        -10.0f,  10.0f, -10.0f,
         10.0f,  10.0f, -10.0f,
         10.0f,  10.0f,  10.0f,
         10.0f,  10.0f,  10.0f,
        -10.0f,  10.0f,  10.0f,
        -10.0f,  10.0f, -10.0f,

        -10.0f, -10.0f, -10.0f,
        -10.0f, -10.0f,  10.0f,
         10.0f, -10.0f, -10.0f,
         10.0f, -10.0f, -10.0f,
        -10.0f, -10.0f,  10.0f,
         10.0f, -10.0f,  10.0f,
    };

    glGenBuffers(1, &d_meshVBO);
    glBindBuffer(GL_ARRAY_BUFFER, d_meshVBO);
    glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float), &points, GL_STATIC_DRAW);
    glGenVertexArrays(1, &d_skyBoxVAO);
    glBindVertexArray(d_skyBoxVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    string faces[6]
    {
        string(d_resourcesDir.toStdString() + "skybox/skybox_right.jpg"),
        string(d_resourcesDir.toStdString() + "skybox/skybox_left.jpg"),
        string(d_resourcesDir.toStdString() + "skybox/skybox_bottom.jpg"),
        string(d_resourcesDir.toStdString() + "skybox/skybox_top.jpg"),
        string(d_resourcesDir.toStdString() + "skybox/skybox_front.jpg"),
        string(d_resourcesDir.toStdString() + "skybox/skybox_back.jpg")
    };

    loadCubemapTexture(faces);
}

void MainView::loadCubemapTexture(string *faces)
{
    glGenTextures(1, &d_skyBoxTexturePtr);
    glBindTexture(GL_TEXTURE_CUBE_MAP, d_skyBoxTexturePtr);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

    for (size_t idx = 0; idx < 6; ++idx)
    {
        QImage textureImage(faces[idx].c_str());
        QVector<quint8> *textureData = imageToBytes(textureImage);

        int imageWidth = textureImage.width();
        int imageHeight = textureImage.height();
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + idx, 0, GL_RGBA8, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData->data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // use linear instead of mipmap
        free(textureData);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}