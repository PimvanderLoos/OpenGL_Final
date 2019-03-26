#ifndef GAME_TEXTUREDATA_H
#define GAME_TEXTUREDATA_H

#include "../platformSpecificIncludes.ih"

#include <QVector3D>
#include <QDebug>

#include <cstdint>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

class TextureData
{
    private:
        friend class boost::serialization::access;

        template<class Archive>
        void save(Archive &ar, const unsigned int version) const;
        template<class Archive>
        void load(Archive & ar, const unsigned int version);
        BOOST_SERIALIZATION_SPLIT_MEMBER()

    public:
        QString d_file = "";
        QVector<quint8> *d_textureData; // (Temporarily) store texture data.
        std::vector<uint8_t> d_textureDataVec;
        int d_imageWidth;
        int d_imageHeight;
        GLuint d_ptr = 0;

        void cleanup();
};

template<class Archive>
inline void TextureData::save(Archive &ar, const unsigned int version) const
{
    ar &d_textureDataVec;
    ar &d_imageWidth;
    ar &d_imageHeight;
}

template<class Archive>
inline void TextureData::load(Archive &ar, const unsigned int version)
{
    ar &d_textureDataVec;
    ar &d_imageWidth;
    ar &d_imageHeight;
}

inline void TextureData::cleanup()
{
//    delete d_imageHeight;
//    delete d_imageWidth;
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
