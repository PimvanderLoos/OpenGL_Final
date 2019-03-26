#ifndef GAME_MESHDATA_H
#define GAME_MESHDATA_H

#include <QVector>


class MeshData
{

    public:
        QVector<float> *d_meshData;
        std::vector<float> d_meshDataVec;
        int d_meshSize;
        std::string d_usedMTLName;
        std::string d_meshName;

        void cleanup();
};

inline void MeshData::cleanup()
{
    delete d_meshData;
    d_meshDataVec.clear();
}

#endif
