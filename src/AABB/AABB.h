#ifndef GAME_AABB_H
#define GAME_AABB_H

#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

class AABB
{
    QVector3D d_min_start;
    QVector3D d_max_start;

    QVector3D d_min;
    QVector3D d_max;

    QVector3D d_dims;

    QMatrix4x4 d_rot;

    void fixMinMax();
    void updateRot();

    public:
        AABB();
        AABB(QVector3D const &pos, float lenX, float lenY, float lenZ);
        AABB(QVector3D const &pos, QVector3D const &dimensions);
        AABB(QVector3D const &pos, float scale);
        AABB(AABB const &other);

        bool collide(AABB const &other) const;
        void shift(float x, float y, float z);
        void shift(QVector3D const &other);
        void grow (float x, float y, float z);
        QVector3D getCenter() const;
        QVector3D getCenterBot() const;
        QVector3D getSize() const;

        float minX() const;
        float minY() const;
        float minZ() const;
        float maxX() const;
        float maxY() const;
        float maxZ() const;

        QVector3D const &getMinVec() const;
        QVector3D const &getMaxVec() const;
        QVector3D const &getMinVecStart() const;
        QVector3D const &getMaxVecStart() const;

        void setPos(QVector3D const &pos);

        void setDims(QVector3D dims);

        void update();
};

inline float AABB::minX() const
{
    return d_min.x();
}

inline float AABB::minY() const
{
    return d_min.y();
}

inline float AABB::minZ() const
{
    return d_min.z();
}

inline float AABB::maxX() const
{
    return d_max.x();
}

inline float AABB::maxY() const
{
    return d_max.y();
}

inline float AABB::maxZ() const
{
    return d_max.z();
}

inline QVector3D const &AABB::getMinVec() const
{
    return d_min;
}

inline QVector3D const &AABB::getMaxVec() const
{
    return d_max;
}

inline QVector3D const &AABB::getMinVecStart() const
{
    return d_min_start;
}

inline QVector3D const &AABB::getMaxVecStart() const
{
    return d_max_start;
}

#endif
