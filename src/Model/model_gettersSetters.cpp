#include "model.ih"

Material &Model::getMaterial()
{
    return d_material;
}

Material const &Model::getMaterial() const
{
    return d_material;
}

void Model::setMaterial(Material const &mat)
{
    d_material = mat;
}



InitStage const &Model::getInitStage() const
{
    return d_initStage;
}

void Model::setInitStage(InitStage stage)
{
    d_initStage = stage;
}



QVector3D const &Model::getRotationOffset() const
{
    return d_rotationOffset;
}

QVector3D &Model::getRotationOffset()
{
    return d_rotationOffset;
}

void Model::setRotationOffset(QVector3D const &offset)
{
    d_rotationOffset = offset;
}



QVector3D const &Model::getCameraOffset() const
{
    return d_cameraOffset;
}

QVector3D &Model::getCameraOffset()
{
//    qDebug() << "Getting camera offset! It is: " << d_cameraOffset;
    return d_cameraOffset;
}

void Model::setCameraOffset(QVector3D const &offset)
{
    qDebug() << "Setting camera offset to: " << offset;
    d_cameraOffset = offset;
}



size_t const &Model::getScale() const
{
    return d_scale;
}

size_t &Model::getScale()
{
    return d_scale;
}

void Model::setScale(size_t scale)
{
    d_scale = scale;
}



ShaderType const &Model::getShaderType() const
{
    return d_shaderType;
}

ShaderType &Model::getShaderType()
{
    return d_shaderType;
}

void Model::setShaderType(ShaderType &shaderType)
{
    d_shaderType = shaderType;
}

void Model::setShaderType(ShaderType &&shaderType)
{
    d_shaderType = shaderType;
}