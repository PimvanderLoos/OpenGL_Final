#ifndef GAME_CAMERA_H
#define GAME_CAMERA_H

#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

#include "../Entity/entity.h"

enum class CameraType
{
    MAIN, REFLECTION, REFRACTION, OTHER
};

class Camera
{
    private:
        QVector3D d_pos;
        QVector3D d_offset;
        QVector3D d_rotVec;

        CameraType d_type;

        QMatrix4x4 d_rot;

        Entity *d_entity;

        static void copy(Camera *to, Camera const &from);

        void rotate(QMatrix4x4 &&rot);
        void rotate(QMatrix4x4 const &rot);
        void rotate(QVector3D const &angles);

    public:
        Camera() = default;
        Camera(QVector3D const &pos);
        Camera(QVector3D &&pos);
        Camera(Camera const &other);

        void setOffset(QVector3D offset);
        void move(QVector3D const &moveVec);
        void move(double x, double y, double z);
        void teleport(QVector3D newPos);

        QMatrix4x4 const &getMatrix() const;

        QVector3D const &getPosition() const;
        QVector3D const &getOffset() const;

        CameraType getCameraType() const;
        void setType(CameraType type);

        void invertPitch();

        Camera &operator=(Camera const &other);

        QVector3D const &getRotation() const;
        void setRotation(QVector3D const &rotVec);
        void setRotation(float x, float y, float z);

        void addRotation(QVector3D const &rotVec);
        void addRotation(float x, float y, float z);

        void focusOnEntity();
        void setEntity(Entity *ent);
        Entity &getEntity();
        Entity const &getEntity() const;
};

#endif
