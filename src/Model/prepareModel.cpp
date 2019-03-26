#include "model.ih"

void Model::prepareModel()
{
    if (d_binaryMeshFileAvailable)
        prepareMeshFromBinary();
    else if (d_objectFileAvailable)
    {
        readModel(d_currentDirectory, d_objectFile);
        unitize();
        prepareMesh();
    }
    else
    {
        qDebug() << "No binary/spec file available, so not reading any models! Directory:"
                 << d_currentDirectory.c_str();
        return;
    }

    for (size_t idx = 0; idx != getSubModelCount(); ++idx)
        prepareTextures(getSubModel(idx));

    d_initStage = InitStage::LOADING;
}

void Model::prepareMesh()
{
    MeshData md;

    for (size_t idx = 0; idx != getSubModelCount(); ++idx)
    {
        SubModel &sm = getSubModel(idx);
        sm.setParent(this);
        generateMeshData(sm);

        if (sm.d_MTL == nullptr)
            sm.setMaterial(d_material);
        else
            sm.setMaterial(*sm.d_MTL);
    }
}

void Model::prepareMeshFromBinary()
{
    std::experimental::filesystem::path file = d_fileName;

    ifstream specFile(d_binarySpecFile);
    size_t subObjectCount = 0;

    specFile >> subObjectCount;
    string fileName;
    specFile >> fileName;
    specFile.close();

    std::experimental::filesystem::path binSpecPath = d_binarySpecFile;

    d_matLib = new MatLib();
    d_matLib->setDirectory((binSpecPath.parent_path().string() + "/").c_str());
    d_matLib->readFromFile((d_currentDirectory + fileName + ".mtl").c_str());

    addNewSubModels(subObjectCount);
    for (size_t idx = 0; idx != subObjectCount; ++idx)
    {
        SubModel &sm = getSubModel(idx);
        string path = d_currentDirectory + fileName + "_" + to_string(idx) + d_myMeshFileExtension;

        std::experimental::filesystem::path objBinFile = d_currentDirectory + fileName + "_" + to_string(idx) + d_myMeshFileExtension;

        ifstream ifs(objBinFile.string(), std::ios::binary);
        boost::archive::binary_iarchive ia(ifs);
        MeshData md;
        ia >> md;
        ifs.close();

        QVector<float> *qVec = new QVector<float>;
        qVec->reserve(static_cast<int>(md.d_meshDataVec.size()));
        for (float fl : md.d_meshDataVec)
            qVec->append(fl);

        QString usedMTLName = md.d_usedMTLName.c_str();

        if (usedMTLName == "")
            sm.setMaterial(d_material);
        else
            sm.setMaterial(*d_matLib->matLibFromName(usedMTLName));

        sm.setParent(this);

        MeshData &submodelmd = sm.getMeshData();
        submodelmd.d_meshSize = md.d_meshSize;
        submodelmd.d_meshData = qVec;
    }
}

void Model::generateMeshData(SubModel &sm)
{
    QVector<QVector3D> vertexCoords  = sm.getVertices();
    QVector<QVector3D> vertexNormals = sm.getNormals();
    QVector<QVector2D> textureCoords = sm.getTextureCoords();

    int size = 2 * 3 * vertexCoords.size() + 2 * textureCoords.size();
    auto *meshDataVec = new QVector<float>;
    meshDataVec->reserve(size);

    for (int idx = 0; idx < vertexCoords.size(); ++idx)
    {
        meshDataVec->append(vertexCoords.at(idx).x());
        meshDataVec->append(vertexCoords.at(idx).y());
        meshDataVec->append(vertexCoords.at(idx).z());
        meshDataVec->append(vertexNormals.at(idx).x());
        meshDataVec->append(vertexNormals.at(idx).y());
        meshDataVec->append(vertexNormals.at(idx).z());
        meshDataVec->append(textureCoords.at(idx).x());
        meshDataVec->append(textureCoords.at(idx).y());
    }

    MeshData &md = sm.getMeshData();
    md.d_meshData = meshDataVec;
    md.d_meshSize = vertexCoords.size();
    md.d_usedMTLName = sm.d_usedMTLName.toStdString();
    md.d_meshName = sm.d_subModelName.toStdString();
}

void Model::prepareTextures(SubModel &sm)
{
    if (prepareTexture(sm.getMaterial().map_Kd))
        sm.d_texturedflag |= TEXTURED_KD;
    if (prepareTexture(sm.getMaterial().map_Ks))
        sm.d_texturedflag |= TEXTURED_KS;
    if (prepareTexture(sm.getMaterial().map_Bump))
        sm.d_texturedflag |= TEXTURED_BUMP;
}

// Load a texture from a file and generate QImage.
bool Model::prepareTexture(TextureData &texData)
{
    if (texData.d_file == "NOTEXTURE" || texData.d_file == "")
        return false;

    // Check if the texture was already prepared.
    if (d_preparedTextures.find(texData.d_file.toStdString()) != d_preparedTextures.end())
    {
        texData.d_textureData = d_preparedTextures.find(texData.d_file.toStdString())->second.d_textureData;
        texData.d_imageWidth  = d_preparedTextures.find(texData.d_file.toStdString())->second.d_imageWidth;
        texData.d_imageHeight = d_preparedTextures.find(texData.d_file.toStdString())->second.d_imageHeight;
        return true;
    }

    // Check if a serialized version is available.
    if (std::experimental::filesystem::exists(std::experimental::filesystem::path(texData.d_file.toStdString() + d_myTextureFileExtension)))
    {
        d_binaryTextureFileAvailable = true;
        prepareTextureFromBinary(texData);
        d_preparedTextures.insert({texData.d_file.toStdString(), texData});
        return true;
    }

    // If there's no way to avoid it, resort to actually loading the file.
    QImage textureImage(texData.d_file);
    texData.d_textureData = imageToBytes(textureImage);
    texData.d_imageWidth  = textureImage.width();
    texData.d_imageHeight = textureImage.height();
    d_preparedTextures.insert({texData.d_file.toStdString(), texData});
    return true;
}

void Model::prepareTextureFromBinary(TextureData &texData)
{
    std::experimental::filesystem::path readLoc(texData.d_file.toStdString() + d_myTextureFileExtension);

    ifstream ifs(readLoc.string(), std::ios::binary);
    boost::archive::binary_iarchive ia(ifs);
    TextureData td;
    ia >> td;
    ifs.close();

    texData.d_imageWidth = td.d_imageWidth;
    texData.d_imageHeight = td.d_imageHeight;
    texData.d_textureDataVec = td.d_textureDataVec;

    QVector<uint8_t > *qVec = new QVector<uint8_t >;
    qVec->reserve(static_cast<int>(texData.d_textureDataVec.size()));
    for (uint8_t fl : texData.d_textureDataVec)
        qVec->append(fl);
    texData.d_textureData = qVec;
}

QVector<quint8>* Model::imageToBytes(QImage &image)
{
    // needed since (0,0) is bottom left in OpenGL
    QImage im = image.mirrored();
    QVector<quint8> *pixelData = new QVector<quint8>;
    pixelData->reserve(im.width()*im.height()*4);

    for (int i = 0; i != im.height(); ++i)
    {
        for (int j = 0; j != im.width(); ++j)
        {
            QRgb pixel = im.pixel(j,i);

            // pixel is of format #AARRGGBB (in hexadecimal notation)
            // so with bitshifting and binary AND you can get
            // the values of the different components
            quint8 r = (quint8)((pixel >> 16) & 0xFF); // Red component
            quint8 g = (quint8)((pixel >>  8) & 0xFF); // Green component
            quint8 b = (quint8) (pixel        & 0xFF); // Blue component
            quint8 a = (quint8)((pixel >> 24) & 0xFF); // Alpha component

            // Add them to the Vector
            pixelData->append(r);
            pixelData->append(g);
            pixelData->append(b);
            pixelData->append(a);
        }
    }
    return pixelData;
}
