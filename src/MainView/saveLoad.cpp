#include "mainview.ih"

void MainView::threadedLoadScene(QString sceneFile)
{
    if (th_modelLoader.isRunning())
    {
        qDebug() << "Already loading a scene, please be patient!";
        return;
    }

    loadScene(sceneFile);
}

void MainView::threadedSaveScene(QString sceneFile)
{
    qDebug() << "Threaded save scene!";
    if (d_threadWriteInUse)
    {
        qDebug() << "Already reading/writing scene file, please be patient!";
        return;
    }

    if (th_sceneWriter != nullptr)
    {
        th_sceneWriter->join();
        delete th_sceneWriter;
    }

    d_threadWriteInUse = true;
    th_sceneWriter = new std::thread(&MainView::saveScene, this, sceneFile);
}

void MainView::convertLoadSavePath(QString &sceneFile)
{
    if (sceneFile == "")
        sceneFile = d_scenesDir + "default";
    if (sceneFile.length() < 20) // Temporary!
        sceneFile = d_scenesDir + sceneFile;
    sceneFile += d_sceneFileExtension;
}

void MainView::saveScene(QString sceneFile)
{
    qDebug() << "Save scene!";
    convertLoadSavePath(sceneFile);
    qDebug() << "Writing to:" << sceneFile;

    // When saving, I don't care about
    ofstream ofs(sceneFile.toStdString(), std::ios::trunc);

    if (!ofs)
    {
        qDebug() << "Failed to open d_file: " << sceneFile;
        return;
    }

    ofs << "MODELS:\n";

    for (auto &modelEntry : d_models)
    {
        Model *model = d_models.getModel(modelEntry);
        ofs << "dir: "     << model->getRelativeDirectory()   << '\n';
        ofs << "scale: "   << model->getScale() << '\n';
        ofs << "cOffset: " << model->getCameraOffset().x()   << ' '  << model->getCameraOffset().y()   << ' ' <<
                              model->getCameraOffset().z()   << '\n';
        ofs << "rOffset: " << model->getRotationOffset().x() << ' '  << model->getRotationOffset().y() << ' ' <<
                              model->getRotationOffset().z() << '\n';
        ofs << "sType: "   << model->getShaderType() << '\n';
        if (model == d_defaultModel)
            ofs << "defaultModel\n";
        ofs << '\n';
    }
    ofs << "ENTITIES:\n";

    for (auto &model : d_models)
    {
        vector<Entity *> *entities = d_models.getEntitiesOfModel(model.second.first);
        if (entities == nullptr)
            continue;
        for (Entity *entity : *entities)
        {
            ofs << "pos: "     << entity->getPosition().x() << ' '  << entity->getPosition().y() << ' '
                               << entity->getPosition().z() << '\n';
            ofs << "rot: "     << entity->getRotation().x() << ' '  << entity->getRotation().y() << ' '
                               << entity->getRotation().z() << '\n';
            ofs << "cOffset: " << entity->getCameraOffset().x() << ' '  << entity->getCameraOffset().y() << ' '
                               << entity->getCameraOffset().z() << '\n';
            ofs << "rOffset: " << entity->getRotationOffset().x() << ' '  << entity->getRotationOffset().y() << ' '
                               << entity->getRotationOffset().z() << '\n';
            ofs << "scale: "   << entity->getScale() << '\n';
            ofs << "gravity: " << entity->getGravity() << '\n';
            ofs << "model: "   << entity->getModel()->getFileName() << '\n';
            if (entity == d_mainEntity)
                ofs << "mainEntity\n";
            ofs << '\n';
        }
    }
    ofs.close();
    d_threadWriteInUse = false;
}

void MainView::loadScene(QString sceneFile)
{
    qDebug() << "Load scene!";
    convertLoadSavePath(sceneFile);
    qDebug() << "Reading from:" << sceneFile;

    ifstream ifs;
    ifs.open(sceneFile.toStdString());

    if (!ifs)
    {
        qDebug() << "Failed to open d_file: " << sceneFile;
        return;
    }

    string line;
    QStringList tokens;
    QString qLine;
    {
        QVector3D rOffset = QVector3D(0, 0, 0);
        QVector3D cOffset = QVector3D(0, 0, 0);
        size_t scale = 5;
        ShaderType sType = ShaderType::DEFAULT;
        string relativeDirectory = "";
        bool firstModel = true;
        bool defaultModel = false;

        while (!ifs.eof())
        {
            getline(ifs, line);
            qLine = line.c_str();

            if (qLine.startsWith("#"))
                continue; // skip comments

            if (line.empty())
            {
                if (!relativeDirectory.empty())
                {
                    Model *model = initModel(d_resourcesDir, relativeDirectory.c_str());

                    model->setRotationOffset(rOffset);
                    model->setCameraOffset(cOffset);
                    model->setScale(scale);
                    model->setShaderType(sType);
                    if (firstModel || defaultModel)
                    {
                        d_defaultModel = model;
                        firstModel = false;
                    }
                }
                rOffset = QVector3D(0, 0, 0);
                cOffset = QVector3D(0, 0, 0);
                scale = 0;
                sType = ShaderType::DEFAULT;
                relativeDirectory = "";
                defaultModel = false;
                continue;
            }

            // Empty line means end of entity. So ad entity to storage if needed, then reset and read the next one.
            tokens = qLine.split(" ", QString::SkipEmptyParts);

            if (tokens[0] == "ENTITIES:")
                break;
            if (tokens[0] == "dir:")
                relativeDirectory = tokens[1].toStdString();
            if (tokens[0] == "scale:")
                scale = static_cast<size_t>(stoi(tokens[1].toStdString()));
            if (tokens[0] == "cOffset:")
                cOffset = QVector3D(stof(tokens[1].toStdString()), stof(tokens[2].toStdString()),
                                    stof(tokens[3].toStdString()));
            if (tokens[0] == "rOffset:")
                rOffset = QVector3D(stof(tokens[1].toStdString()), stof(tokens[2].toStdString()),
                                    stof(tokens[3].toStdString()));
            if (tokens[0] == "sType:")
                sType = static_cast<ShaderType>(stoi(tokens[1].toStdString()));
            if (tokens[0] == "defaultModel")
                defaultModel = true;
        }
    }

    d_models.removeAllEntities();
    th_modelLoader.setThread(new std::thread(&MainView::prepareModels, this));

    {
        string modelName = "";
        QVector3D pos = QVector3D(0, 0, 0);
        QVector3D rot = QVector3D(0, 0, 0);
        QVector3D cOffset = QVector3D(0, 0, 0);
        QVector3D rOffset = QVector3D(0, 0, 0);
        size_t scale = 5;
        double gravity = 9.81;
        bool mainEntity = false;
        bool firstEntity = true;

        while (!ifs.eof())
        {
            getline(ifs, line);
            qLine = line.c_str();

            if (qLine.startsWith("#"))
                continue; // skip comments

            // Empty line means end of entity. So ad entity to storage if needed, then reset and read the next one.
            if (line.empty())
            {
                if (!modelName.empty())
                {
                    Model *model = d_models.getModel(modelName.c_str());
                    if (model == nullptr)
                    {
                        qDebug() << "Could not find model:" << modelName.c_str();
                        goto reset_variables;
                    }
                    Entity *entity;
                    if (model->getShaderType() == ShaderType::DEFAULT)
                        entity = new Entity(this);
                    else if (model->getShaderType() == ShaderType::WATER)
                        entity = new Entity_Water(this);
                    else
                        goto reset_variables;
                    entity->setModel(model);
                    entity->setPosition(pos);
                    entity->setRotation(rot);
                    entity->setCameraOffset(cOffset);
                    entity->setRotationOffset(rOffset);
                    entity->setScale(scale);
                    entity->setGravity(gravity);

                    entity->generateAABB();
                    entity->setShaderType(model->getShaderType());
                    d_models.addEntity(entity);

                    // If the entity is described as main entity, or if it's the first entity to be added,
                    // Make it the mainEntity.
                    if (mainEntity || firstEntity)
                    {
                        setMainEntity(entity);
                        firstEntity = false;
                        emit mainEntityChanged();
                    }
                }
                reset_variables:
                modelName = "";
                pos = QVector3D(0, 0, 0);
                rot = QVector3D(0, 0, 0);
                cOffset = QVector3D(0, 0, 0);
                rOffset = QVector3D(0, 0, 0);
                scale = 5;
                gravity = 5;
                mainEntity = false;
                continue;
            }

            tokens = qLine.split(" ", QString::SkipEmptyParts);

            if (tokens[0] == "model:")
                modelName = tokens[1].toStdString();
            if (tokens[0] == "scale:")
                scale = static_cast<size_t>(stoi(tokens[1].toStdString()));
            if (tokens[0] == "gravity:")
                gravity = stod(tokens[1].toStdString());
            if (tokens[0] == "cOffset:")
                cOffset = QVector3D(stof(tokens[1].toStdString()), stof(tokens[2].toStdString()),
                                    stof(tokens[3].toStdString()));
            if (tokens[0] == "rOffset:")
                rOffset = QVector3D(stof(tokens[1].toStdString()), stof(tokens[2].toStdString()),
                                    stof(tokens[3].toStdString()));
            if (tokens[0] == "rot:")
                rot = QVector3D(stof(tokens[1].toStdString()), stof(tokens[2].toStdString()),
                                stof(tokens[3].toStdString()));
            if (tokens[0] == "pos:")
                pos = QVector3D(stof(tokens[1].toStdString()), stof(tokens[2].toStdString()),
                                stof(tokens[3].toStdString()));
            if (tokens[0] == "mainEntity")
                mainEntity = true;
        }
        if (firstEntity)
            switchToDummyEntity();
    }
    ifs.close();
}






































