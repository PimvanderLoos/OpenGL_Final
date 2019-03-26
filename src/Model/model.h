#ifndef MODEL_H
#define MODEL_H

#include <QTextStream>
#include <QOpenGLFunctions_3_3_Core>

#include "../MatLib/matLib.h"
#include "../MeshData/meshData.h"
#include "../MyStorage/myStorage.h"
#include "subModel.h"

#include <functional>
#include <unordered_map>

/**
 * @brief The Model class
 *
 * Loads all data from a Wavefront .obj file
 * IMPORTANT! Current only supports TRIANGLE meshes!
 */

enum ShaderType
{
    DEFAULT, WATER
};


enum class InitStage
{
    PREPARATION, LOADING, DONE
};

class Model
{
    private:
        static std::atomic<size_t> s_id;
        static std::atomic<size_t> timeSpent;
        size_t d_ID;

        QOpenGLFunctions_3_3_Core *d_gl = nullptr;

        MyStorage<SubModel> d_subModels;

        InitStage d_initStage = InitStage::PREPARATION;

        MatLib *d_matLib = 0;
        Material d_material;
        std::string d_currentDirectory;
        std::string d_relativeDirectory;
        bool d_objectFileAvailable = false;

        std::string d_objectFile;
        ShaderType d_shaderType = ShaderType::DEFAULT;
        static constexpr bool d_writeModel = false; // Whether or not this model should be serialized.

        std::unordered_map<std::string, TextureData> d_preparedTextures;
        std::unordered_map<std::string, GLuint>  d_loadedTextures;

        QVector3D d_rotationOffset = QVector3D(0.0,0.0,0.0);
        QVector3D d_cameraOffset = QVector3D(0.0,0.0,0.0);
        size_t d_scale;

        size_t d_lineCount;

        GLuint d_meshVBO;

        void init();

        QVector<QVector3D> vertices_indexed;
        QVector<QVector3D> norm;
        QVector<QVector2D> tex;

        // Model preparation.
        void prepareMesh();
        void generateMeshData(SubModel &sm);
        bool prepareTexture(TextureData &texData);
        QVector<quint8>* imageToBytes(QImage &image);

        // Model loading.
        void loadAllMeshes();
        void generateVAOVBO(SubModel &sm);
        void loadAllTextures();
        void loadTextures(SubModel &sm);
        void loadTexture(TextureData &texData);

        std::string d_fileName;

    public:
        Model() = delete;
        Model(QOpenGLFunctions_3_3_Core *gl);
        Model(std::string filePath);
        ~Model();

        size_t getID() const;

        void addSubModel(SubModel *sub);
        void addNewSubModels(size_t count);
        SubModel &getSubModel(size_t idx);
        SubModel const &getSubModel(size_t idx) const;
        size_t getSubModelCount() const;

        std::string const &getCurrentDirectory() const;
        std::string const &getRelativeDirectory() const;

        void unitize();

        void initRead(std::string baseDir, std::string relativeDir);

        // Alignment of data
        void alignData();

        void unpackIndexes();

        void readModel();
        void readModel(std::string directory, std::string objFile);
        QString readNewObject(std::ifstream &in, size_t idx);

        MatLib *getMatLib();

        // Model preparation.
        void prepareModel();
        void prepareTextures(SubModel &sm);

        // Model loading.
        void loadModel();

        std::string getFileName() const;
        QString getQFileName() const;

        // ------ Getters and Setters ------- //
        Material &getMaterial();
        Material const &getMaterial() const;
        void setMaterial(Material const &mat);

        InitStage const &getInitStage() const;
        void setInitStage(InitStage stage);

        QVector3D const &getRotationOffset() const;
        QVector3D &getRotationOffset();
        void setRotationOffset(QVector3D const &offset);

        QVector3D const &getCameraOffset() const;
        QVector3D &getCameraOffset();
        void setCameraOffset(QVector3D const &offset);

        size_t const &getScale() const;
        size_t &getScale();
        void setScale(size_t scale);

        ShaderType const &getShaderType() const;
        ShaderType &getShaderType();
        void setShaderType(ShaderType &shaderType);
        void setShaderType(ShaderType &&shaderType);
};

struct ModelHasher
{
    size_t operator()(Model const &model) const
    {
        return std::hash<std::size_t>()(model.getID());
    };
};

struct ModelComparator
{
    bool operator() (Model const *m1, Model const *m2) const
    {
        return m1->getID() == m2->getID();
    }
};

#endif
