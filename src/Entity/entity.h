#ifndef GAME_ENTITY_H
#define GAME_ENTITY_H

#include <QOpenGLFunctions_3_3_Core>

#include "../Model/model.h"
#include "../AABB/AABB.h"

/**
 * The entity class is meant to wrap interactive objects, such as NPCs, balls etc.
 * The tick function should update the position and everything.
 */

class Entity
{
    protected:
        ShaderType d_shaderType;
        QOpenGLFunctions_3_3_Core *d_gl = nullptr;
        static std::atomic<size_t> s_id;
        size_t const d_ID;
        Model *d_model = nullptr;
        AABB d_aabb;
        bool d_noClip = false;

        size_t d_ticks = 0;

        QVector3D d_startPos;

        QVector3D d_pos;
        QVector3D d_lastPos;
        // When rendering at a position, a corner will be put there, not the center.
        QVector3D d_renderPos;
        QVector3D d_velocity;

        QVector3D d_rot;
        QVector3D d_lastRot;
        QVector3D d_velocityRot;

        QVector3D d_lookVec;
        QVector3D d_sideVec;
        QVector3D d_heightVec;

        QVector3D d_cameraOffset;
        QVector3D d_rotationOffset = QVector3D(0, 0, 0);

        double d_gravity;

        double d_friction;
        double d_lastTime;
        double d_time;

        double d_scale;

        bool d_underwater;
        bool d_swimming;
        bool d_onGround;

        void init();

    public:
        Entity() = delete;
        Entity(QOpenGLFunctions_3_3_Core *gl);
        Entity(Entity *other, bool newID = true);
        virtual ~Entity();

        // Tick function handles all entity movement / physics etc.
        virtual void tick(float time);
        // Convert the current entity to a regular entity.
        virtual void toRegular();
        // Convert the current entity to a specialised entity.
        virtual void fromRegular();

        void setGravity(double grav);
        bool hasMoved();

        void addVelocity(double x, double y, double z);
        void addVelocity(QVector3D const &addVec);
        void setVelocity(double x, double y, double z);
        void setVelocity(QVector3D setVec);

        void resetRotationOffset();

        void addRotVelocity(double x, double y, double z);
        void addRotVelocity(QVector3D const &addVec);
        void setRotVelocity(double x, double y, double z);
        void setRotVelocity(QVector3D setVec);

        void setPosition(double x, double y, double z);
        void setPosition(QVector3D const &pos);
        void move(double x, double y, double z);
        void move(QVector3D const &moveVal);

        virtual void generateAABB();
        virtual void updateAABBDims(double x, double y, double z);

        void updateLookVec();

        bool noClip();
        void setNoClip(bool val);

        size_t getID() const;

        // ------ Getters and Setters ------- //
        QVector3D const &getPosition() const;
        QVector3D &getPosition();

        QVector3D const &getLookVector() const;
        QVector3D &getLookVector();

        QVector3D const &getSideVector() const;
        QVector3D &getSideVector();

        QVector3D const &getHeightVector() const;
        QVector3D &getHeightVector();

        QVector3D const &getRotation() const;
        QVector3D &getRotation();
        void setRotation(QVector3D &rot);

        QVector3D const &getCameraOffset() const;
        QVector3D &getCameraOffset();
        void setCameraOffset(QVector3D const &offset);

        QVector3D getCameraOffsetTotal() const;

        QVector3D const &getRotationOffset() const;
        QVector3D &getRotationOffset();
        void setRotationOffset(QVector3D const &offset);

        ShaderType const &getShaderType() const;
        ShaderType &getShaderType();
        void setShaderType(ShaderType const &type);

        Model *getModel();
        Model const *getModel() const;
        void setModel(Model *model);

        bool isUnderwater() const;
        bool isSwimming() const;
        bool isOnGround() const;

        AABB &getAABB();
        AABB const &getAABB() const;

        double getGravity() const;

        double getScale() const;
        void setScale(double scale);
        double getTotalScale() const;

        QOpenGLFunctions_3_3_Core *getQGL();
};


#endif
