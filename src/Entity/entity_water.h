#ifndef GAME_ENTITY_WATER_H
#define GAME_ENTITY_WATER_H

#include "entity.h"
#include "../FrameBuffer/frameBuffer.h"

class Entity_Water : public Entity
{
    private:
        FrameBuffer *d_reflectionBuffer = nullptr;
        float d_waveSpeed = 0.02f;
        float d_moveFactor = 0;

    public:
        Entity_Water() = delete;
        Entity_Water(Entity_Water &&other) = delete;
        Entity_Water(QOpenGLFunctions_3_3_Core *gl);
        Entity_Water(Entity *other, bool newID = true);

        ~Entity_Water();
        void toRegular();
        void fromRegular();
        void tick(float time);

        void  setMoveFactor(float newVal);
        float getMoveFactor() const;

        void verifyFBO();
        bool isFBOgenerated();

        virtual void generateAABB();
        virtual void updateAABBDims(double x, double y, double z);

        void setReflectionBuffer(FrameBuffer *fbo);

        FrameBuffer *getReflectionBuffer();
        FrameBuffer const *getReflectionBuffer() const;

        void  setWaveSpeed(float newVal);
        float getWaveSpeed() const;
};

#endif
