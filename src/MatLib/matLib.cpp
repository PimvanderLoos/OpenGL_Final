#include "matLib.h"

#include <QDebug>
#include <QFile>
#include <limits>

MatLib::MatLib()
{
    d_MTLs     = new Mtl[1];
    d_MTLCount = 0;
    d_MTLSize  = 1;
}

void MatLib::readFromFile(QString fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);

        QString line;
        QStringList tokens;

        size_t mtlCount = 0;

        // Read all mtls from file
        while(!in.atEnd())
        {
            line = in.readLine();
            if (line.startsWith("#") || line == "")
                continue; // skip comments and blank lines

            tokens = line.split(" ", QString::SkipEmptyParts);
            // 'o' means a new object was encountered in the wavefront .obj d_file.
            if (tokens[0] == "newmtl")
            {
                // Get the name of the first object to be added
                QString subObjectName = tokens[1];
                // Then, for as long as there are new objects, read them.
                while (subObjectName != nullptr)
                {
                    addMtl();
                    d_MTLs[d_MTLCount-1].mtlName = subObjectName;
                    subObjectName = readNewMtl(in, d_MTLCount-1);
                    ++mtlCount;
                }
            }
        }
        file.close();
    }
    else
        qDebug() << "Failed to open d_file " << fileName << " because: " << file.errorString();
}

void MatLib::addMtl()
{
    // Make more space if needed (doubling available space).
    if (d_MTLCount == d_MTLSize)
        reserve(d_MTLSize);
    ++d_MTLCount;
}

void MatLib::setDirectory(QString &dir)
{
    d_directory = dir;
}

void MatLib::setDirectory(QString &&dir)
{
    d_directory = QString(dir);
}

void MatLib::reserve(size_t addSpace)
{
    if (addSpace == 0)
        return;
    Mtl *newObjects = new Mtl[d_MTLSize + addSpace];
    for (size_t idx = 0; idx != d_MTLCount; ++idx)
        newObjects[idx] = d_MTLs[idx];

    delete[] d_MTLs;
    d_MTLs = newObjects;
    d_MTLSize += addSpace;
}

size_t MatLib::getMatLibCount() const
{
    return d_MTLCount;
}

/*
 * Reads an objects from a QTextStream and stores its data in
 * d_subModels[idx].
 * Returns the name of the next object (if it exists).
 */
QString MatLib::readNewMtl(QTextStream &in, size_t idx)
{
    QString line;
    QStringList tokens;
    while (!in.atEnd())
    {
        line = in.readLine();
        if (line.startsWith("#") || line == "")
            continue; // skip comments and blank lines

        tokens = line.split(" ", QString::SkipEmptyParts);

        // When the token 'newmtl' has been encountered, it means a new mtl will be defined from here on out.
        // So, return the name of the new mtl.
        if (tokens[0] == "newmtl")
            return tokens[1];

        if (tokens[0] == "Ns")
            d_MTLs[idx].ns = tokens[1].toFloat();

        if (tokens[0] == "Ka")
        {
            float x, y, z;
            x = tokens[1].toFloat();
            y = tokens[2].toFloat();
            z = tokens[3].toFloat();
            d_MTLs[idx].ka = QVector3D(x, y, z);
        }

        if (tokens[0] == "Kd")
        {
            float x, y, z;
            x = tokens[1].toFloat();
            y = tokens[2].toFloat();
            z = tokens[3].toFloat();
            d_MTLs[idx].kd = QVector3D(x, y, z);
        }

        if (tokens[0] == "Ks")
        {
            float x, y, z;
            x = tokens[1].toFloat();
            y = tokens[2].toFloat();
            z = tokens[3].toFloat();
            d_MTLs[idx].ks = QVector3D(x, y, z);
        }

        if (tokens[0] == "Ke")
        {
            float x, y, z;
            x = tokens[1].toFloat();
            y = tokens[2].toFloat();
            z = tokens[3].toFloat();
            d_MTLs[idx].ke = QVector3D(x, y, z);
        }

        if (tokens[0] == "Ni")
            d_MTLs[idx].ni = tokens[1].toFloat();

        if (tokens[0] == "d")
            d_MTLs[idx].d = tokens[1].toFloat();

        if (line.startsWith("illum"))
            d_MTLs[idx].illumModel = tokens[1].toULong();

        if (line.startsWith("map_Kd"))
            d_MTLs[idx].map_Kd = d_directory + tokens[1];

        if (line.startsWith("map_Bump"))
            d_MTLs[idx].map_Bump = d_directory + tokens[1];

        if (line.startsWith("map_Ks"))
            d_MTLs[idx].map_Ks = d_directory + tokens[1];
    }
    return nullptr;
}

Mtl *MatLib::matLibFromName(QString &name)
{
    for (size_t idx = 0; idx != d_MTLCount; ++idx)
        if (d_MTLs[idx].mtlName == name)
            return &d_MTLs[idx];
    return nullptr;
}

MatLib::~MatLib()
{
    delete[] d_MTLs;
}

Mtl *MatLib::matLibAtPos(size_t pos)
{
    if (pos >= d_MTLCount)
        return nullptr;
    return &d_MTLs[pos];
}
