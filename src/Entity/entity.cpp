#include "entity.ih"

std::atomic<size_t> Entity::s_id = 0;

Entity::Entity(QOpenGLFunctions_3_3_Core *gl)
:
    d_ID(s_id++)
{
    d_gl = gl;
    init();
}

Entity::Entity(Entity *other, bool newID)
:
    d_ID(newID ? s_id++ : other->getID())
{
    d_onGround = other->d_onGround;
    d_swimming = other->d_swimming;
    d_underwater = other->d_underwater;
    d_gravity = other->d_gravity;
    d_lastPos = other->d_lastPos;
    d_lookVec = other->d_lookVec;
    d_velocity = other->d_velocity;
    d_velocityRot = other->d_velocityRot;
    d_cameraOffset = other->d_cameraOffset;
    d_rotationOffset = other->d_rotationOffset;
    d_rot = other->d_rot;
    d_friction = other->d_friction;
    d_lastTime = other->d_lastTime;
    d_time = other->d_time;
    setModel(other->d_model);
    d_pos = other->d_pos;
    d_gl = other->d_gl;
    d_shaderType = other->d_shaderType;
    setScale(other->d_scale);
    if (!newID)
        delete other;
}

Entity::~Entity()
{}

void Entity::init()
{
    d_onGround = false;
    d_swimming = false;
    d_underwater = false;
    d_gravity = 9.81;
    d_lastPos = QVector3D(0.0,0.0,0.0);
    d_lookVec = QVector3D(0.0,0.0,0.0);
    d_velocity = QVector3D(0.0,0.0,0.0);
    d_velocityRot = QVector3D(0.0,0.0,0.0);
    d_cameraOffset = QVector3D(0.0,0.0,0.0);
    d_rot = QVector3D(0.0,0.0,0.0);
    d_friction = 0.9f;
    d_lastTime = 0;
    d_time = 0;
    d_shaderType = ShaderType::DEFAULT;
}

void Entity::tick(float time)
{
    ++d_ticks;
    d_lastTime = d_time;
    d_time = time;
    float timeDelta = d_time - d_lastTime;

    d_lastPos = d_pos;
    d_lastRot = d_rot;

    if (d_lastTime == 0)
        timeDelta = 1.0f / 143 * 1000;

    QVector3D step = d_velocity * timeDelta / 1000;

    d_pos += d_lookVec * step.x() + d_heightVec * step.y() +  d_sideVec * step.z();

    d_velocity *= (1 - d_friction);

    d_rot += d_velocityRot;
    d_rot.setX(fmod(d_rot.x(), 360.0f));
    d_rot.setY(fmod(d_rot.y(), 360.0f));
    d_rot.setZ(fmod(d_rot.z(), 360.0f));

    if (abs(d_velocityRot.x()) < 0.001 && abs(d_velocityRot.y()) < 0.001 && abs(d_velocityRot.z()) < 0.001)
        d_velocityRot = QVector3D(0.0,0.0,0.0);
    else
        d_velocityRot *= (1 - d_friction);

    d_velocityRot *= (1 - d_friction);

    updateLookVec();

    getAABB().setPos(d_pos);
}

void Entity::updateLookVec()
{
    double pitch = d_rot.x() * M_PI / 180;
    double yaw   = d_rot.y() * M_PI / 180;
    double roll  = d_rot.z() * M_PI / 180;

    d_lookVec.setX(-cos(yaw)   * sin(pitch) * sin(roll) - sin(yaw) * cos(roll));
    d_lookVec.setZ(-sin(yaw)   * sin(pitch) * sin(roll) + cos(yaw) * cos(roll));
    d_lookVec.setY( cos(pitch) * sin(roll));
    d_lookVec.normalize();

    double yaw2 = fmod(d_rot.y() * M_PI / 180 + 67.5, 360);

    d_sideVec.setX(-cos(yaw2)  * sin(pitch) * sin(roll) - sin(yaw2) * cos(roll));
    d_sideVec.setZ(-sin(yaw2)  * sin(pitch) * sin(roll) + cos(yaw2) * cos(roll));
    d_sideVec.setY( cos(pitch) * sin(roll));
    d_sideVec.normalize();

    double roll2 = fmod(d_rot.z() * M_PI / 180 + 67.5, 360);
    d_heightVec.setX(-cos(yaw)   * sin(pitch) * sin(roll2) - sin(yaw) * cos(roll2));
    d_heightVec.setZ(-sin(yaw)   * sin(pitch) * sin(roll2) + cos(yaw) * cos(roll2));
    d_heightVec.setY( cos(pitch) * sin(roll2));
    d_heightVec.normalize();
}

void Entity::setGravity(double gravity)
{
    d_gravity = gravity;
}

bool Entity::hasMoved()
{
    return d_pos != d_lastPos;
}

void Entity::addVelocity(double x, double y, double z)
{
    setVelocity(d_velocity.x() + x,
                d_velocity.y() + y,
                d_velocity.z() + z);
}

void Entity::addVelocity(QVector3D const &addVec)
{
    d_velocity += addVec;
}

void Entity::setVelocity(double x, double y, double z)
{
    d_velocity.setX(x);
    d_velocity.setY(y);
    d_velocity.setZ(z);
}

void Entity::setVelocity(QVector3D setVec)
{
    d_velocity = setVec;
}

void Entity::addRotVelocity(double x, double y, double z)
{
    setRotVelocity(d_velocityRot.x() + x,
                   d_velocityRot.y() + y,
                   d_velocityRot.z() + z);
}

void Entity::setRotVelocity(double x, double y, double z)
{
    d_velocityRot.setX(d_velocityRot.x() + x);
    d_velocityRot.setY(d_velocityRot.y() + y);
    d_velocityRot.setZ(d_velocityRot.z() + z);
}

void Entity::addRotVelocity(QVector3D const &addVec)
{
    addRotVelocity(addVec.x(), addVec.y(), addVec.z());
}

void Entity::setRotVelocity(QVector3D setVec)
{
    setRotVelocity(setVec.x(), setVec.y(), setVec.z());
}

void Entity::setPosition(double x, double y, double z)
{
    d_pos.setX(x);
    d_pos.setY(y);
    d_pos.setZ(z);
}

void Entity::setPosition(QVector3D const &pos)
{
    setPosition(pos.x(), pos.y(), pos.z());
}

void Entity::generateAABB()
{
    d_aabb = AABB(d_pos, getTotalScale());
}

void Entity::updateAABBDims(double x, double y, double z)
{
    d_aabb.setDims(QVector3D(x, y, z));
}

void Entity::move(double x, double y, double z)
{
    setPosition(d_pos.x() + x, d_pos.y() + y, d_pos.z() + z);
}

void Entity::move(QVector3D const &moveVal)
{
    move(moveVal.x(),
         moveVal.y(),
         moveVal.z());
}

void Entity::fromRegular()
{}

void Entity::toRegular()
{}
