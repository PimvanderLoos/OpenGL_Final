#include "camera.h"

#include <cmath>

Camera::Camera(QVector3D const &pos)
:
    d_pos(pos),
    d_offset(QVector3D(0.0,0.0,0.0))
{}

Camera::Camera(QVector3D &&pos)
:
    Camera(pos)
{}

Camera::Camera(Camera const &other)
{
    copy(this, other);
}

void Camera::setOffset(QVector3D offset)
{
    d_offset = offset;
}

void Camera::move(QVector3D const &moveVec)
{
    move(moveVec.x(), moveVec.y(), moveVec.z());
}

void Camera::move(double x, double y, double z)
{
    if (x == 0 && y == 0 && z == 0)
        return;
    d_pos.setX(d_pos.x() + x);
    d_pos.setY(d_pos.y() + y);
    d_pos.setZ(d_pos.z() + z);
}

void Camera::teleport(QVector3D newPos)
{
    d_pos = newPos;
}

QMatrix4x4 const &Camera::getMatrix() const
{
    return d_rot;
}

void Camera::rotate(QMatrix4x4 &&rot)
{
    d_rot = QMatrix4x4(rot);
}

void Camera::rotate(QMatrix4x4 const &rot)
{
    d_rot = QMatrix4x4(rot);
}

void Camera::rotate(QVector3D const &angles)
{
    QMatrix4x4 rot;
    rot.rotate(QQuaternion::fromEulerAngles(angles));
    rotate(rot);
}

QVector3D const &Camera::getPosition() const
{
    return d_pos;
}

QVector3D const &Camera::getOffset() const
{
    return d_offset;
}

CameraType Camera::getCameraType() const
{
    return d_type;
}

void Camera::setType(CameraType type)
{
    d_type = type;
}

// Invert pitch and roll
// QQuaternion::fromEulerAngles uses the following order: pitch, yaw, roll.
void Camera::invertPitch()
{
    setRotation(-d_rotVec.x(), d_rotVec.y(), -d_rotVec.z());
}

void Camera::copy(Camera *to, Camera const &from)
{
    to->d_pos       = from.d_pos;
    to->d_rot       = from.d_rot;
    to->d_offset    = from.d_offset;
    to->d_rotVec    = from.d_rotVec;
    to->d_type      = from.d_type;
}

Camera &Camera::operator=(Camera const &other)
{
    Camera::copy(this, other);
    return *this;
}

QVector3D const &Camera::getRotation() const
{
    return d_rotVec;
}

void Camera::addRotation(QVector3D const &rotVec)
{
    addRotation(rotVec.x(), rotVec.y(), rotVec.z());
}

void Camera::focusOnEntity()
{
    d_pos = d_entity->getPosition()     +
            d_entity->getLookVector()   * d_entity->getCameraOffsetTotal().x() +
            d_entity->getHeightVector() * d_entity->getCameraOffsetTotal().y() +
            d_entity->getSideVector()   * d_entity->getCameraOffsetTotal().z();
    setRotation(d_entity->getRotation());
}

void Camera::setEntity(Entity *ent)
{
    d_entity = ent;
}

Entity &Camera::getEntity()
{
    return *d_entity;
}

Entity const &Camera::getEntity() const
{
    return *d_entity;
}

void Camera::addRotation(float x, float y, float z)
{
    setRotation(fmod(d_rotVec.x() + x, 360),
                fmod(d_rotVec.y() + y, 360),
                fmod(d_rotVec.z() + z, 360));
}

void Camera::setRotation(QVector3D const &rotVec)
{
    setRotation(rotVec.x(), rotVec.y(), rotVec.z());
}

void Camera::setRotation(float x, float y, float z)
{
    d_rotVec.setX(x);
    d_rotVec.setY(y);
    d_rotVec.setZ(z);
    rotate(d_rotVec);
}