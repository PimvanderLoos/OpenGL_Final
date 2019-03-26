#include "model.ih"

using namespace experimental;

atomic<size_t> Model::s_id = 0;

// A Private Vertex class for vertex comparison
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

Model::Model(std::string filePath)
:
    d_ID(s_id++),
    d_fileName(filePath.c_str())
{
    init();
    filesystem::path path = filePath;
    string name = path.filename();
    string directory = path.parent_path();
    directory += "/";

    qDebug() << "name =" << name.c_str() << ", dir =" << directory.c_str();

    readModel(directory, name);
}

void Model::addSubModel(SubModel *sub)
{
    d_subModels.append(sub);
}

void Model::addNewSubModels(size_t count)
{
    if (d_subModels.size() + count >= d_subModels.capacity())
        d_subModels.reserve(count);

    for (size_t idx = 0; idx != count; ++idx)
        d_subModels.append(new SubModel());
}

void Model::init()
{}

size_t Model::getID() const
{
    return d_ID;
}

Model::Model(QOpenGLFunctions_3_3_Core *gl)
:
    d_ID(s_id++)
{
    init();
    d_gl = gl;
}

SubModel &Model::getSubModel(size_t idx)
{
    return *d_subModels[idx];
}

SubModel const &Model::getSubModel(size_t idx) const
{
    return *d_subModels[idx];
}

/**
 * @brief Model::alignData
 *
 * Make sure that the indices from the vertices align with those
 * of the normals and the texture coordinates, create extra vertices
 * if vertex has multiple normals or texturecoords
 */
void Model::alignData()
{
    for (size_t idx = 0; idx != d_subModels.size(); ++idx)
    {
        SubModel *sm = d_subModels[idx];
        QVector<QVector3D> verts  = QVector<QVector3D>();
        verts.reserve(sm->vertices_indexed.size());
        QVector<QVector3D> norms  = QVector<QVector3D>();
        norms.reserve(sm->vertices_indexed.size());
        QVector<QVector2D> texcs  = QVector<QVector2D>();
        texcs.reserve(sm->vertices_indexed.size());
        QVector<Vertex>    vs     = QVector<Vertex>();
        QVector<unsigned> ind     = QVector<unsigned>();
        ind.reserve(sm->indices.size());

        unsigned currentIndex = 0;

        for (int i = 0; i != sm->indices.size(); ++i)
        {
            QVector3D v = this->vertices_indexed[sm->indices[i]];
            QVector3D n = QVector2D(0, 0);
            if (sm->hNorms)
                n = this->norm[sm->normal_indices[i]];

            QVector2D t = QVector2D(0, 0);
            if (sm->hTexs)
                t = this->tex[sm->texcoord_indices[i]];

            Vertex k;
            k = Vertex(v, n, t);

            if (vs.contains(k))
                // Vertex already exists, use that index
                ind.append(vs.indexOf(k));
            else
            {
                // Create a new vertex
                verts.append(v);
                norms.append(n);
                texcs.append(t);
                vs.append(k);
                ind.append(currentIndex);
                ++currentIndex;
            }
        }
        // Remove old data
        sm->vertices_indexed.clear();
        sm->normals_indexed.clear();
        sm->textureCoords_indexed.clear();

        // Set the new data
        sm->vertices_indexed = verts;
        sm->normals_indexed = norms;
        sm->textureCoords_indexed = texcs;
    }
}

/**
 * @brief Model::unpackIndexes
 *
 * Unpack indices so that they are available for glDrawArrays()
 */
void Model::unpackIndexes()
{
    for (size_t idx = 0; idx != d_subModels.size(); ++idx)
    {
        SubModel *sm = d_subModels[idx];
        sm->vertices.clear();
        sm->normals.clear();
        sm->textureCoords.clear();
        for (int i = 0; i != sm->indices.size(); ++i)
        {
            sm->vertices.append(this->vertices_indexed[sm->indices[i]]);
            if (sm->hNorms)
                sm->normals.append(this->norm[sm->normal_indices[i]]);
            if (sm->hTexs)
                sm->textureCoords.append(this->tex[sm->texcoord_indices[i]]);
        }
    }
}

void Model::unitize()
{
    float maxX = std::numeric_limits<float>::min();
    float maxY = std::numeric_limits<float>::min();
    float maxZ = std::numeric_limits<float>::min();
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();

    for (size_t idx = 0; idx != d_subModels.size(); ++idx)
        d_subModels[idx]->getMinMax(&minX, &minY, &minZ, &maxX, &maxY, &maxZ);

    for (size_t idx = 0; idx != d_subModels.size(); ++idx)
        d_subModels[idx]->applyUnitization(&minX, &minY, &minZ, &maxX, &maxY, &maxZ);
}

MatLib *Model::getMatLib()
{
    return this->d_matLib;
}

Model::~Model()
{
    GLuint meshVAO;
    GLuint meshVBO;
    for (size_t idx = 0; idx != d_subModels.size(); ++idx)
    {
        d_gl->glDeleteTextures    (1, &getSubModel(idx).getMaterial().map_Kd.d_ptr);
        d_gl->glDeleteTextures    (1, &getSubModel(idx).getMaterial().map_Ks.d_ptr);
        d_gl->glDeleteTextures    (1, &getSubModel(idx).getMaterial().map_Bump.d_ptr);
        meshVAO = getSubModel(idx).getMeshVAO();
        d_gl->glDeleteVertexArrays(1, &meshVAO);
        meshVBO = getSubModel(idx).getMeshVBO();
        d_gl->glDeleteBuffers     (1, &meshVBO);
    }
    delete d_matLib;
}

size_t Model::getSubModelCount() const
{
    return d_subModels.size();
}

string const &Model::getCurrentDirectory() const
{
    return d_currentDirectory;
}

string const &Model::getRelativeDirectory() const
{
    return d_relativeDirectory;
}

std::string Model::getFileName() const
{
    return d_fileName;
}

QString Model::getQFileName() const
{
    return d_fileName.c_str();
}

void Model::initRead(string resourcesDir, string relativeDirectory)
{
    d_currentDirectory  = resourcesDir + relativeDirectory;
    d_relativeDirectory = relativeDirectory;
    d_objectFile = "";

    for (auto &p : filesystem::directory_iterator(d_currentDirectory))
        if (p.path().extension() == ".obj")
            d_objectFile = p.path().filename().string();

    if (d_objectFile != "")
    {
        qDebug() << "Object d_file is found in folder:" << d_currentDirectory.c_str();
        d_objectFileAvailable = true;
        d_fileName = filesystem::path(d_objectFile).stem().c_str();
    }

    if (!d_objectFileAvailable)
        qDebug() << "NO OBJECT FILE FOUND IN FOLDER:" << d_currentDirectory.c_str();
}

// This constructor also loads MTL files!
void Model::readModel(string directory, string objFile)
{
    qDebug() << "Going to loadObjAndMtl now! dir =" << directory.c_str() << "objFile =" << objFile.c_str();

    d_currentDirectory = directory;
    QString mtlFile = "";
    d_matLib = new MatLib();
    {
        QString qDirectory = directory.c_str();
        d_matLib->setDirectory(qDirectory);
    }
    experimental::filesystem::path path(directory + "/" + objFile);
    d_fileName = path.stem();

    size_t subObjectCount = 0;
    bool lateAdd = false;

    ifstream file;
    file.open(path);

    if (!file)
    {
        qDebug() << "Failed to open d_file: " << (directory + "/" + objFile).c_str();
        return;
    }

    string line;
    QStringList tokens;

    while (!file.eof())
    {
        getline(file,line);
        ++d_lineCount;
        QString qLine = line.c_str();

        if (qLine.startsWith("#") || line == "")
            continue; // skip comments and blank lines

        tokens = qLine.split(" ", QString::SkipEmptyParts);

        // An obj d_file can use multiple mtl files.
        if (mtlFile == "")
            if (tokens[0] == "mtllib")
            {
                string mtlFile = d_currentDirectory + tokens[1].toStdString();
                d_matLib->readFromFile(mtlFile.c_str());
            }

        // 'o' means a new object was encountered in the wavefront .obj d_file.
        if (tokens[0] == "o" || tokens[0] == "g" || tokens[0] == 'v')
        {
            if (tokens[0] == "o" || tokens[0] == "g")
                qDebug() << "ObjFile:" << objFile.c_str() << "Found new subobject!" << tokens[1] << "While reading the model!";
            // If the object d_file doens't have explicitly defined objects and starts with a vertex instead,
            // give the object an empty name and make sure the first vertex isn't forgotten about.
            QString subObjectName;
            if (tokens[0] == 'v')
            {
                lateAdd = true;
                subObjectName = " ";
            }
            else
                // Get the name of the first object to be added
                subObjectName = tokens[1];
            // Then, for as long as there are new objects, read them.
            while (subObjectName != nullptr)
            {
                this->addSubModel(new SubModel(subObjectName));
                if (lateAdd)
                {
                    d_subModels[0]->parseVertex(tokens);
                    lateAdd = false;
                }
                subObjectName = this->readNewObject(file, subObjectCount);
                ++subObjectCount;
            }
        }
    }

    file.close();

    // Calculate how many vertices_index, norm, and tex qvectors there are in all submodels combined.
    size_t vertices_indexedCount = 0;
    size_t normCount = 0;
    size_t texCount = 0;
    for (size_t idx = 0; idx != d_subModels.size(); ++idx)
    {
        vertices_indexedCount += d_subModels[idx]->vertices_indexed.size();
        normCount += d_subModels[idx]->norm.size();
        texCount += d_subModels[idx]->tex.size();
    }

    // Then reserve space to store pointers to all those vectors in this class.
    this->vertices_indexed = QVector<QVector3D>();
    this->vertices_indexed.reserve(vertices_indexedCount);
    this->norm = QVector<QVector3D>();
    this->norm.reserve(normCount);
    this->tex = QVector<QVector2D>();
    this->tex.reserve(texCount);

    for (size_t idx = 0; idx != d_subModels.size(); ++idx)
    {
        SubModel *sm = d_subModels[idx];

        for (int idx2 = 0; idx2 != sm->vertices_indexed.size(); ++idx2)
            this->vertices_indexed.append(sm->vertices_indexed[idx2]);
        for (int idx2 = 0; idx2 != sm->norm.size(); ++idx2)
            this->norm.append(sm->norm[idx2]);
        for (int idx2 = 0; idx2 != sm->tex.size(); ++idx2)
            this->tex.append(sm->tex[idx2]);
    }
    this->unpackIndexes();
    this->alignData();
}

/*
 * Reads an objects from a QTextStream and stores its data in
 * d_subModels[idx].
 * Returns the name of the next object (if it exists).
 */
QString Model::readNewObject(ifstream &in, size_t idx)
{
    string line;
    QStringList tokens;
    while (!in.eof())
    {
        getline(in, line);
        QString qLine = line.c_str();

        if (qLine.startsWith("#") || line == "")
            continue; // skip comments and blank lines

        tokens = qLine.split(" ", QString::SkipEmptyParts);

        // When the token 'o' has been encountered, it means a new model will be defined from here on out.
        // So, return the name of the new model.
        if (tokens[0] == "o" || tokens[0] == "g")
            return tokens[1];

        // Switch depending on first element
        if (qLine.startsWith("usemtl"))
        {
            qDebug() << "Obj:" << d_fileName.c_str() << "(" << idx << ") uses MTL:" << tokens[1];
            d_subModels[idx]->d_MTL = d_matLib->matLibFromName(tokens[1]);
            d_subModels[idx]->d_usedMTLName = tokens[1];
        }

        if (tokens[0] == "v")
            d_subModels[idx]->parseVertex(tokens);

        if (tokens[0] == "vn")
            d_subModels[idx]->parseNormal(tokens);

        if (tokens[0] == "vt")
            d_subModels[idx]->parseTexture(tokens);

        if (tokens[0] == "f")
            d_subModels[idx]->parseFace(tokens);
    }
    return nullptr;
}



