#include "AABB.h"

AABB::AABB()
{
    d_dims = QVector3D(1, 1, 1);

    d_min_start = QVector3D(-0.5f,-0.5f,-0.5f);
    d_max_start = QVector3D( 0.5f, 0.5f, 0.5f);

    d_min = QVector3D(d_min);
    d_max = QVector3D(d_max);
}


AABB::AABB(QVector3D const &pos, float lenX, float lenY, float lenZ)
{
    d_dims = QVector3D(lenX, lenY, lenZ);

    d_min_start = QVector3D(pos.x() - lenX / 2, pos.y() - lenY / 2, pos.z() - lenZ / 2);
    d_max_start = QVector3D(pos.x() + lenX / 2, pos.y() + lenY / 2, pos.z() + lenZ / 2);

    d_min = QVector3D(d_min_start);
    d_max = QVector3D(d_max_start);
}

AABB::AABB(QVector3D const &pos, float scale)
:
    AABB(pos, scale, scale, scale)
{}

AABB::AABB(QVector3D const &pos, QVector3D const &dimensions)
:
    AABB(pos, std::max(dimensions.x(), std::max(dimensions.y(), dimensions.z())),
              std::max(dimensions.x(), std::max(dimensions.y(), dimensions.z())),
              std::max(dimensions.x(), std::max(dimensions.y(), dimensions.z())))
{}

AABB::AABB(AABB const &other)
{
    d_min_start = QVector3D(other.getMinVecStart().x(), other.getMinVecStart().y(), other.getMinVecStart().z());
    d_max_start = QVector3D(other.getMaxVecStart().x(), other.getMaxVecStart().y(), other.getMaxVecStart().z());
    d_min = QVector3D(other.getMinVec().x(), other.getMinVec().y(), other.getMinVec().z());
    d_max = QVector3D(other.getMaxVec().x(), other.getMaxVec().y(), other.getMaxVec().z());
    d_dims = QVector3D(other.d_dims.x(), other.d_dims.y(), other.d_dims.z());
}

bool AABB::collide(AABB const &other) const
{
    return(maxX() > other.minX() &&
           minX() < other.maxX() &&
           maxY() > other.minY() &&
           minY() < other.maxY() &&
           maxZ() > other.minZ() &&
           minZ() < other.maxZ());
}

void AABB::shift(float x, float y, float z)
{
    d_min.setX(d_min.x() + x);
    d_min.setY(d_min.x() + y);
    d_min.setZ(d_min.x() + z);

    d_max.setX(d_max.x() + x);
    d_max.setY(d_max.x() + y);
    d_max.setZ(d_max.x() + z);

//    d_rot.translate(x, y, z);

    update();
}

void AABB::shift(QVector3D const &other)
{
    shift(other.x(), other.y(), other.z());
}

void AABB::grow(float x, float y, float z)
{
    d_min.setX(d_min.x() - x / 2);
    d_min.setY(d_min.x() - y / 2);
    d_min.setZ(d_min.x() - z / 2);

    d_max.setX(d_max.x() + x / 2);
    d_max.setY(d_max.x() + y / 2);
    d_max.setZ(d_max.x() + z / 2);

    d_dims.setX(d_dims.x() + x);
    d_dims.setY(d_dims.y() + y);
    d_dims.setZ(d_dims.z() + z);

    update();
}

QVector3D AABB::getCenter() const
{
    return QVector3D(d_min.x() + (d_max.x() - d_min.x()) / 2,
                     d_min.y() + (d_max.y() - d_min.y()) / 2,
                     d_min.z() + (d_max.z() - d_min.z()) / 2);
}

QVector3D AABB::getCenterBot() const
{
    return QVector3D(d_min.x() + (d_max.x() - d_min.x()) / 2,
                     d_min.y(),
                     d_min.z() + (d_max.z() - d_min.z()) / 2);
}

QVector3D AABB::getSize() const
{
    return d_dims;
}

void AABB::setPos(QVector3D const &pos)
{
    d_min = QVector3D(pos.x() - d_dims.x(), pos.y() - d_dims.y(), pos.z() - d_dims.z());
    d_max = QVector3D(pos.x() + d_dims.x(), pos.y() + d_dims.y(), pos.z() + d_dims.z());
}

void AABB::setDims(QVector3D dims)
{
    d_dims = dims;
    update();
}

void AABB::update()
{
    fixMinMax();
}

void AABB::fixMinMax()
{
    // Fix min and max, if needed.
    if (d_min.x() > d_max.x())
    {
        float tmp = d_min.x();
        d_min.setX(d_max.x());
        d_max.setX(tmp);
    }
    if (d_min.y() > d_max.y())
    {
        float tmp = d_min.y();
        d_min.setY(d_max.y());
        d_max.setY(tmp);
    }
    if (d_min.z() > d_max.z())
    {
        float tmp = d_min.z();
        d_min.setZ(d_max.z());
        d_max.setZ(tmp);
    }
}