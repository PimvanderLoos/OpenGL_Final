#ifndef MATLIB_H
#define MATLIB_H

#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include "../TextureData/textureData.h"

#include "../platformSpecificIncludes.ih"

struct Mtl
{
    QString mtlName;
    QString map_Kd;
    QString map_Bump;
    QString map_Ks;
    float ns;
    QVector3D ka;
    QVector3D kd;
    QVector3D ks;
    QVector3D ke;
    float ni;
    float  d;
    size_t illumModel;

    Mtl();
    Mtl(Material const &mat);
};

inline Mtl::Mtl()
:
//    map_Kd("NOTEXTURE"), map_Ks("NOTEXTURE"), map_Bump("NOTEXTURE"), ns(32),
    mtlName(""), map_Kd("NOTEXTURE"), map_Bump("NOTEXTURE"), map_Ks("NOTEXTURE"), ns(32),
    ka(QVector3D(1.0,1.0,1.0)), kd(QVector3D(1.0,1.0,1.0)),
    ks(QVector3D(1.0,1.0,1.0)), ke(QVector3D(1.0,1.0,1.0)), ni(0), d(0), illumModel(0)
{}

/**
 * @brief The MatLib class
 *
 * Loads all data from a Wavefront .mtl file
 */
class MatLib
{
    private:
        Mtl       *d_MTLs;
        size_t  d_MTLSize;
        size_t d_MTLCount;
        QString d_directory = "";

        QString readNewMtl(QTextStream &in, size_t idx);

    public:
        MatLib();

        void readFromFile(QString fileName);

        Mtl *matLibFromName(QString &name);
        Mtl *matLibAtPos(size_t pos);
        size_t getMatLibCount() const;
        void reserve(size_t addSpace);
        void addMtl();
        void setDirectory(QString &dir);
        void setDirectory(QString &&dir);
        ~MatLib();
};

#endif // MATLIB_H
