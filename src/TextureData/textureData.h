#ifndef GAME_TEXTUREDATA_H
#define GAME_TEXTUREDATA_H

#include "../platformSpecificIncludes.ih"

#include <QVector3D>
#include <QDebug>

#include <cstdint>

class TextureData
{
    public:
        QString d_file = "";
        QVector<quint8> *d_textureData; // (Temporarily) store texture data.
        std::vector<uint8_t> d_textureDataVec;
        int d_imageWidth;
        int d_imageHeight;
        GLuint d_ptr = 0;

        void cleanup();
};

inline void TextureData::cleanup()
{
    delete d_textureData;
    d_textureDataVec.clear();
}

struct Material
{
    QString mtlName;
    TextureData map_Kd;
    TextureData map_Ks;
    TextureData map_Bump;
    QVector3D col;
    float ns;
    QVector3D ka;
    QVector3D kd;
    QVector3D ks;
    QVector3D ke;
    float ni;
    float  d;
    size_t illumModel;
};

#endif
