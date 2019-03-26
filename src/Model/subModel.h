#ifndef SUBMODEL_H
#define SUBMODEL_H

#include "../MatLib/matLib.h"
#include "../TextureData/textureData.h"
#include "../MeshData/meshData.h"

static constexpr unsigned int NOT_TEXTURED  = 0;
static constexpr unsigned int TEXTURED_KD   = 1 << 0; // 0000 0001
static constexpr unsigned int TEXTURED_KS   = 1 << 1; // 0000 0010
static constexpr unsigned int TEXTURED_BUMP = 1 << 2; // 0000 0100

class Model;

class SubModel
{
    private:
        QVector3D dimensions;
        QVector3D position;
        MeshData d_meshData;
        Model *d_parent;

        GLuint d_meshVBO;
        GLuint d_meshVAO;

    public:
        SubModel();
        SubModel(QString subModelName);
        ~SubModel() = default;

        Material d_material;

        MeshData const &getMeshData() const;
        MeshData &getMeshData();
        void setMeshData(MeshData &meshData);

        Model const *getParent() const;
        Model *getParent();
        void setParent(Model *parent);

        GLuint getMeshVAO() const;
        void setMeshVAO(GLuint vao);

        GLuint getMeshVBO() const;
        void setMeshVBO(GLuint vbo);

        int getMeshSize() const;

        Material const &getMaterial() const;
        Material &getMaterial();
        void setMaterial(Material &mat);
        void setMaterial(Mtl &mtl);

        unsigned int d_texturedflag = NOT_TEXTURED;
        QString d_usedMTLName = "";

        QString d_subModelName;
        Mtl *d_MTL = nullptr;

        void unitize();

        void getMinMax(float *minX, float *minY, float *minZ, float *maxX, float *maxY, float *maxZ);
        void applyUnitization(float *minX, float *minY, float *minZ, float *maxX, float *maxY, float *maxZ);

        QVector3D const getDimensions() const;
        QVector3D const getPosition() const;

        // Used for glDrawArrays()
        QVector<QVector3D> getVertices() const;
        QVector<QVector3D> getNormals() const;
        QVector<QVector2D> getTextureCoords() const;

        // Used for interleaving into one buffer for glDrawArrays()
        QVector<float> getVNInterleaved();

        QVector<float> getVNTInterleaved();

        // Used for glDrawElements()
        QVector<QVector3D> getVertices_indexed();

        QVector<QVector3D> getNormals_indexed();

        QVector<QVector2D> getTextureCoords_indexed();

        // Used for interleaving into one buffer for glDrawElements()
        QVector<float> getVNInterleaved_indexed();

        QVector<float> getVNTInterleaved_indexed();

        QVector<unsigned> getIndices();

        int getNumTriangles();

        // OBJ parsing
        void parseVertex(QStringList tokens);

        void parseNormal(QStringList tokens);

        void parseTexture(QStringList tokens);

        void parseFace(QStringList tokens);

        // Intermediate storage of values
        QVector<QVector3D> vertices_indexed;
        QVector<QVector3D> normals_indexed;
        QVector<QVector2D> textureCoords_indexed;
        QVector<unsigned>  indices;

        bool hNorms = false;
        bool hTexs  = false;

        QVector<QVector3D> vertices;
        QVector<QVector3D> normals;
        QVector<QVector2D> textureCoords;

        // Utility storage
        QVector<unsigned>  normal_indices;
        QVector<unsigned>  texcoord_indices;

        QVector<QVector3D> norm;
        QVector<QVector2D> tex;
};

inline QVector3D const SubModel::getDimensions() const
{
    return dimensions;
}

inline QVector3D const SubModel::getPosition() const
{
    return position;
}

#endif
