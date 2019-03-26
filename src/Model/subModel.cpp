#include "subModel.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>

// A Private Vertex class for vertex comparison
// DO NOT include "vertex.h" or something similar in this d_file
struct Vertex
{
    QVector3D coord;
    QVector3D normal;
    QVector2D texCoord;

    Vertex()
    :
        coord(), normal(), texCoord()
    {}

    Vertex(QVector3D coords, QVector3D normal, QVector2D texc)
    :
        coord(coords), normal(normal), texCoord(texc)
    {}

    bool operator==(const Vertex &other) const
    {
        if (other.coord != coord)
            return false;
        if (other.normal != normal)
            return false;
        if (other.texCoord != texCoord)
            return false;
        return true;
    }
};

void SubModel::getMinMax(float *minXP, float *minYP, float *minZP, float *maxXP, float *maxYP, float *maxZP)
{
    QVector<QVector3D> vertices = this->getVertices();

    float max_x = std::numeric_limits<float>::min();;
    float max_y = std::numeric_limits<float>::min();;
    float max_z = std::numeric_limits<float>::min();;
    float min_x = std::numeric_limits<float>::max();;
    float min_y = std::numeric_limits<float>::max();;
    float min_z = std::numeric_limits<float>::max();;

    for (int i = 0; i < vertices.size(); ++i)
    {
        if (vertices[i][0] > max_x)
            max_x = vertices[i][0];
        if (vertices[i][1] > max_y)
            max_y = vertices[i][1];
        if (vertices[i][2] > max_z)
            max_z = vertices[i][2];
        if (vertices[i][0] < min_x)
            min_x = vertices[i][0];
        if (vertices[i][1] < min_y)
            min_y = vertices[i][1];
        if (vertices[i][2] < min_z)
            min_z = vertices[i][2];
    }

    dimensions = QVector3D(max_x - min_x, max_y - min_y, max_z - min_z);

    if (max_x  > *maxXP)
        *maxXP = max_x;
    if (max_y  > *maxYP)
        *maxYP = max_y;
    if (max_z  > *maxZP)
        *maxZP = max_z;

    if (min_x  < *minXP)
        *minXP = min_x;
    if (min_y  < *minYP)
        *minYP = min_y;
    if (min_z  < *minZP)
        *minZP = min_z;
}

void SubModel::applyUnitization(float *minX, float *minY, float *minZ, float *maxX, float *maxY, float *maxZ)
{
    float largest_difference = std::max(std::max(*maxX - *minX, *maxY - *minY), *maxZ - *minZ);
    float x_center = (*maxX + *minX) / 2;
    float y_center = (*maxY + *minY) / 2;
    float z_center = (*maxZ + *minZ) / 2;
    for (int i = 0; i < vertices.size(); ++i) // place object at 0, 0, 0
    {
        this->vertices[i][0] -= x_center;
        this->vertices[i][1] -= y_center;
        this->vertices[i][2] -= z_center;
        this->vertices[i][0] /= largest_difference;
        this->vertices[i][1] /= largest_difference;
        this->vertices[i][2] /= largest_difference;
    }
    dimensions /= largest_difference;
    position = QVector3D(dimensions.x() / 2 + x_center,
                         dimensions.y() / 2 + y_center,
                         dimensions.z() / 2 + z_center);
}


/**
 * @brief SubModel::unitize()
 * Unitize the model by scaling so that it fits a box with sides 1
 * and origin at 0,0,0 Useful for models with different scales
 */
void SubModel::unitize()
{
    float max_x = std::numeric_limits<float>::min();;
    float max_y = std::numeric_limits<float>::min();;
    float max_z = std::numeric_limits<float>::min();;
    float min_x = std::numeric_limits<float>::max();;
    float min_y = std::numeric_limits<float>::max();;
    float min_z = std::numeric_limits<float>::max();;

    QVector<QVector3D> vertices = this->getVertices();

    for (int i = 0; i < vertices.size(); ++i)
    {
        if (vertices[i][0] > max_x)
            max_x = vertices[i][0];
        if (vertices[i][1] > max_y)
            max_y = vertices[i][1];
        if (vertices[i][2] > max_z)
            max_z = vertices[i][2];
        if (vertices[i][0] < min_x)
            min_x = vertices[i][0];
        if (vertices[i][1] < min_y)
            min_y = vertices[i][1];
        if (vertices[i][2] < min_z)
            min_z = vertices[i][2];
    }

    float largest_difference = std::max(std::max(max_x - min_x, max_y - min_y), max_z - min_z);
    float x_center = (max_x + min_x) / 2;
    float y_center = (max_y + min_y) / 2;
    float z_center = (max_z + min_z) / 2;
    for (int i = 0; i < vertices.size(); ++i) // place object at 0, 0, 0
    {
        this->vertices[i][0] -= x_center;
        this->vertices[i][1] -= y_center;
        this->vertices[i][2] -= z_center;
        this->vertices[i][0] /= largest_difference;
        this->vertices[i][1] /= largest_difference;
        this->vertices[i][2] /= largest_difference;
    }
}

QVector<QVector3D> SubModel::getVertices() const
{
    return vertices;
}

QVector<QVector3D> SubModel::getNormals() const
{
    return normals;
}

QVector<QVector2D> SubModel::getTextureCoords() const
{
    return textureCoords;
}

QVector<QVector3D> SubModel::getVertices_indexed()
{
    return vertices_indexed;
}

QVector<QVector3D> SubModel::getNormals_indexed()
{
    return normals_indexed;
}

QVector<QVector2D> SubModel::getTextureCoords_indexed()
{
    return textureCoords_indexed;
}

QVector<unsigned> SubModel::getIndices()
{
    return indices;
}

QVector<float> SubModel::getVNInterleaved()
{
    QVector<float> buffer;

    for (int i = 0; i != vertices.size(); ++i)
    {
        QVector3D vertex = vertices.at(i);
        QVector3D normal = normals.at(i);
        buffer.append(vertex.x());
        buffer.append(vertex.y());
        buffer.append(vertex.z());
        buffer.append(normal.x());
        buffer.append(normal.y());
        buffer.append(normal.z());
    }
    return buffer;
}

QVector<float> SubModel::getVNTInterleaved()
{
    QVector<float> buffer;

    for (int i = 0; i != vertices.size(); ++i)
    {
        QVector3D vertex = vertices.at(i);
        QVector3D normal = normals.at(i);
        QVector2D uv     = textureCoords.at(i);
        buffer.append(vertex.x());
        buffer.append(vertex.y());
        buffer.append(vertex.z());
        buffer.append(normal.x());
        buffer.append(normal.y());
        buffer.append(normal.z());
        buffer.append(uv.x());
        buffer.append(uv.y());
    }

    return buffer;
}

// Throws when there are no normals or texture values
QVector<float> SubModel::getVNInterleaved_indexed()
{
    QVector<float> buffer;

    for (int i = 0; i != vertices_indexed.size(); ++i)
    {
        QVector3D vertex = vertices_indexed.at(i);
        QVector3D normal = normals_indexed.at(i);
        buffer.append(vertex.x());
        buffer.append(vertex.y());
        buffer.append(vertex.z());
        buffer.append(normal.x());
        buffer.append(normal.y());
        buffer.append(normal.z());
    }

    return buffer;
}

// Throws when there are no normals or texture values
QVector<float> SubModel::getVNTInterleaved_indexed()
{
    QVector<float> buffer;

    for (int i = 0; i != vertices_indexed.size(); ++i)
    {
        QVector3D vertex = vertices_indexed.at(i);
        QVector3D normal = normals_indexed.at(i);
        QVector2D uv     = textureCoords_indexed.at(i);
        buffer.append(vertex.x());
        buffer.append(vertex.y());
        buffer.append(vertex.z());
        buffer.append(normal.x());
        buffer.append(normal.y());
        buffer.append(normal.z());
        buffer.append(uv.x());
        buffer.append(uv.y());
    }

    return buffer;
}

/**
 * @brief SubModel::getNumTriangles
 *
 * Gets the number of triangles in the model
 *
 * @return number of triangles
 */
int SubModel::getNumTriangles()
{
    return vertices.size() / 3;
}

void SubModel::parseVertex(QStringList tokens)
{
    float x, y, z;
    x = tokens[1].toFloat();
    y = tokens[2].toFloat();
    z = tokens[3].toFloat();
    vertices_indexed.append(QVector3D(x, y, z));
}

void SubModel::parseNormal(QStringList tokens)
{
    hNorms = true;
    float x, y, z;
    x = tokens[1].toFloat();
    y = tokens[2].toFloat();
    z = tokens[3].toFloat();
    norm.append(QVector3D(x, y, z));
}

void SubModel::parseTexture(QStringList tokens)
{
    hTexs = true;
    float u, v;
    u = tokens[1].toFloat();
    v = tokens[2].toFloat();
    tex.append(QVector2D(u, v));
}

void SubModel::parseFace(QStringList tokens)
{
    QStringList elements;

    for (int i = 1; i != tokens.size(); ++i)
    {
        elements = tokens[i].split("/");
        // -1 since .obj count from 1
        indices.append(elements[0].toInt() - 1);

        if (elements.size() > 1 && !elements[1].isEmpty())
            texcoord_indices.append(elements[1].toInt() - 1);

        if (elements.size() > 2 && !elements[2].isEmpty())
            normal_indices.append(elements[2].toInt() - 1);
    }
}

MeshData const &SubModel::getMeshData() const
{
    return d_meshData;
}

MeshData &SubModel::getMeshData()
{
    return d_meshData;
}

void SubModel::setMeshData(MeshData &meshData)
{
    d_meshData = meshData;
}

Model const *SubModel::getParent() const
{
    return d_parent;
}

Model *SubModel::getParent()
{
    return d_parent;
}

void SubModel::setParent(Model *parent)
{
    d_parent = parent;
}

GLuint SubModel::getMeshVAO() const
{
    return d_meshVAO;
}

void SubModel::setMeshVAO(GLuint vao)
{
    d_meshVAO = vao;
}

GLuint SubModel::getMeshVBO() const
{
    return d_meshVBO;
}

void SubModel::setMeshVBO(GLuint vbo)
{
    d_meshVBO = vbo;
}

int SubModel::getMeshSize() const
{
    return d_meshData.d_meshSize;
}

Material const &SubModel::getMaterial() const
{
    return d_material;
}

Material &SubModel::getMaterial()
{
    return d_material;
}

void SubModel::setMaterial(Material &mat)
{
    d_material = mat;
}

void SubModel::setMaterial(Mtl &mtl)
{
    d_material.ns = mtl.ns;
    d_material.ka = mtl.ka;
    d_material.kd = mtl.kd;
    d_material.ks = mtl.ks;
    d_material.ke = mtl.ke;
    d_material.ni = mtl.ni;
    d_material.d = mtl.d;
    d_material.illumModel = mtl.illumModel;
    d_material.map_Kd.d_file = mtl.map_Kd;
    d_material.map_Ks.d_file = mtl.map_Ks;
    d_material.map_Bump.d_file = mtl.map_Bump;
    d_material.mtlName = mtl.mtlName;
}

SubModel::SubModel(QString subModelName)
:
    d_subModelName(subModelName)
{}

SubModel::SubModel()
:
    SubModel("")
{}