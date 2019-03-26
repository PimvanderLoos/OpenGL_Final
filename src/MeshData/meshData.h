#ifndef GAME_MESHDATA_H
#define GAME_MESHDATA_H

#include <QVector>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

class MeshData
{
    private:
        friend class boost::serialization::access;

        template<class Archive>
        void save(Archive &ar, const unsigned int version) const;
        template<class Archive>
        void load(Archive & ar, const unsigned int version);
        BOOST_SERIALIZATION_SPLIT_MEMBER()

    public:
        QVector<float> *d_meshData;
        std::vector<float> d_meshDataVec;
        int d_meshSize;
        std::string d_usedMTLName;
        std::string d_meshName;

        void cleanup();
};

template<class Archive>
inline void MeshData::save(Archive &ar, const unsigned int version) const
{
    ar &d_meshSize;
    ar &d_meshDataVec;
    ar &d_usedMTLName;
//    ar &d_meshName;
}

template<class Archive>
inline void MeshData::load(Archive &ar, const unsigned int version)
{
    ar &d_meshSize;
    ar &d_meshDataVec;
    ar &d_usedMTLName;
//    ar &d_meshName;
}

inline void MeshData::cleanup()
{
    delete d_meshData;
    d_meshDataVec.clear();
}

#endif
