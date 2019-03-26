#include "entity.ih"

bool Entity::noClip()
{
    return d_noClip;
}

void Entity::setNoClip(bool val)
{
    d_noClip = val;
}

QVector3D const &Entity::getPosition() const
{
    return d_pos;
}

QVector3D &Entity::getPosition()
{
    return d_pos;
}

QVector3D const &Entity::getLookVector() const
{
    return d_lookVec;
}

QVector3D &Entity::getLookVector()
{
    return d_lookVec;
}

QVector3D const &Entity::getSideVector() const
{
    return d_sideVec;
}

QVector3D &Entity::getSideVector()
{
    return d_sideVec;
}

QVector3D const &Entity::getHeightVector() const
{
    return d_heightVec;
}

QVector3D &Entity::getHeightVector()
{
    return d_heightVec;
}

QVector3D const &Entity::getRotation() const
{
    return d_rot;
}

QVector3D &Entity::getRotation()
{
    return d_rot;
}

void Entity::setRotation(QVector3D &rot)
{
    d_rot = rot;
}

bool Entity::isUnderwater() const
{
    return d_underwater;
}

bool Entity::isSwimming() const
{
    return d_swimming;
}

bool Entity::isOnGround() const
{
    return d_onGround;
}

Model const *Entity::getModel() const
{
    return d_model;
}

Model *Entity::getModel()
{
    return d_model;
}

void Entity::setModel(Model *model)
{
    d_model = model;
    generateAABB();
}

double Entity::getGravity() const
{
    return d_gravity;
}

size_t Entity::getID() const
{
    return d_ID;
}

QVector3D &Entity::getCameraOffset()
{
    return d_cameraOffset;
}

QVector3D const &Entity::getCameraOffset() const
{
    return d_cameraOffset;
}

void Entity::setCameraOffset(QVector3D const &offset)
{
    d_cameraOffset = offset;
}

QVector3D Entity::getCameraOffsetTotal() const
{
    return getTotalScale() * (getCameraOffset() + (d_model == nullptr ? QVector3D(0,0,0) : d_model->getCameraOffset()));
}

double Entity::getScale() const
{
    return d_scale;
}

void Entity::setScale(double scale)
{
    d_scale = scale;
    double totalScale = getTotalScale();
    updateAABBDims(totalScale, totalScale, totalScale);
}

double Entity::getTotalScale() const
{
    return d_model == nullptr ? d_scale : d_scale + d_model->getScale();
}

QVector3D &Entity::getRotationOffset()
{
    return d_rotationOffset;
}

QVector3D const &Entity::getRotationOffset() const
{
    return d_rotationOffset;
}

void Entity::setRotationOffset(QVector3D const &offset)
{
    d_rotationOffset = offset;
}

void Entity::resetRotationOffset()
{
    d_rotationOffset = QVector3D(0, 0, 0);
}

AABB &Entity::getAABB()
{
    return d_aabb;
}

AABB const &Entity::getAABB() const
{
    return d_aabb;
}

ShaderType &Entity::getShaderType()
{
    return d_shaderType;
}

ShaderType const &Entity::getShaderType() const
{
    return d_shaderType;
}

void Entity::setShaderType(ShaderType const &type)
{
    d_shaderType = type;
}

QOpenGLFunctions_3_3_Core *Entity::getQGL()
{
    return d_gl;
}
