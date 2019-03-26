#include "entity_water.h"

void Entity_Water::tick(float time)
{
    Entity::tick(time);
}

Entity_Water::Entity_Water(QOpenGLFunctions_3_3_Core *gl)
:
    Entity(gl)
{} // The framebuffer should not be initialized at any given point! Leads to a messy situation.

Entity_Water::Entity_Water(Entity *other, bool newID)
:
    Entity(other, newID)
{} // The framebuffer should not be initialized at any given point! Leads to a messy situation.

void Entity_Water::verifyFBO()
{
    fromRegular();
}

bool Entity_Water::isFBOgenerated()
{
    return d_reflectionBuffer != nullptr;
}

Entity_Water::~Entity_Water()
{
    toRegular();
}

void Entity_Water::setMoveFactor(float newVal)
{
    d_moveFactor = newVal;
}

FrameBuffer *Entity_Water::getReflectionBuffer()
{
    if (d_reflectionBuffer == nullptr)
        qDebug() << "Getting reflection buffer. but it's nullptr!";
    return d_reflectionBuffer;
}

void Entity_Water::setReflectionBuffer(FrameBuffer *fbo)
{
    d_reflectionBuffer = fbo;
}

FrameBuffer const *Entity_Water::getReflectionBuffer() const
{
    return d_reflectionBuffer;
}

void Entity_Water::generateAABB()
{
    float totalScale = static_cast<float>(getTotalScale());
    d_aabb = AABB(d_pos, totalScale, 1, totalScale);
}

void Entity_Water::updateAABBDims(double x, double y, double z)
{
    d_aabb.setDims(QVector3D(x, 1, z));
}

float Entity_Water::getMoveFactor() const
{
    return d_moveFactor;
}

void Entity_Water::setWaveSpeed(float newVal)
{
    d_waveSpeed = newVal;
}

float Entity_Water::getWaveSpeed() const
{
    return d_waveSpeed;
}

void Entity_Water::fromRegular()
{
    if (d_reflectionBuffer != nullptr)
        return;

    setReflectionBuffer(new FrameBuffer(d_gl, FrameBufferType::REFLECTION, static_cast<unsigned int>(512),
                                                                           static_cast<unsigned int>(512), 1.5f));
    if (!getReflectionBuffer()->isComplete())
        qDebug() << "Reflection GLFramebuffer  incomplete!!!" << getReflectionBuffer()->getStatus().c_str();
}

void Entity_Water::toRegular()
{
    if (d_reflectionBuffer != nullptr)
        delete d_reflectionBuffer;
    d_reflectionBuffer = nullptr;
}