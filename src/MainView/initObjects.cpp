#include "mainview.ih"

#include <iostream>
#include <fstream>
#include <QVector>

using namespace std;

void MainView::addDefaultEntity()
{
    Entity *entity = new Entity(this);
    entity->setModel(d_defaultModel);
    entity->setPosition(QVector3D(0.0f, 0.0f, 0.0f));
    entity->generateAABB();
    d_models.addEntity(entity);
    setMainEntity(entity);
}

void MainView::duplicateMainEntity()
{
    if (isMainEntityDummy())
        return;

    setMainEntity(d_models.duplicateEntity(d_mainEntity));
    d_mainEntity->updateLookVec();
    d_mainEntity->move(d_mainEntity->getSideVector() * d_mainEntity->getTotalScale());
}

bool MainView::isMainEntityDummy()
{
    return d_mainEntity == d_dummyEntity;
}

void MainView::killMainEntity()
{
    if (isMainEntityDummy())
        return;
    Entity *dying = d_mainEntity;
    if (d_models.entityCount() == 1)
        switchToDummyEntity();
    else
        selectNextEntity(-1);
    d_models.removeEntity(dying);
}

void MainView::initRefractionBuffer()
{
    qDebug() << "Initializing refraction buffer!";
    d_refractionBuffer = new FrameBuffer(this, FrameBufferType::REFRACTION, width(), height(), 0.7);
    if (!d_refractionBuffer->isComplete())
        qDebug() << "Refraction GLFramebuffer incomplete!!!" << d_refractionBuffer->getStatus().c_str();
    unbindCurrentFrameBuffer();
    d_refractionBuffer->printData();
}

void MainView::prepareModels()
{
    chrono::time_point<Clock> start = Clock::now();

    qDebug() << "======================================================";
    qDebug() << "================ PREPARING MODELS NOW ================";
    qDebug() << "======================================================";

    prepareModelsInStorage(d_models);

    chrono::time_point<Clock> end = Clock::now();
    chrono::milliseconds diff = chrono::duration_cast<chrono::milliseconds>(end - start);

    qDebug() << "Preparing regular models took " << diff.count() << "ms\n";
    d_initStage = InitStage::LOADING;
}

// Prepare all initialized objects.
void MainView::prepareModelsInStorage(EntityStorage &storage)
{
//#pragma omp parallel for
    for (auto &modelEntry : storage) // Doesn't work with OpenMP until OpenMP5.0 has been implemented.
    {                                // Expected to be part of GCC9 in early 2019.
        Model *model = d_models.getModel(modelEntry);
        qDebug() << "Preparing model:" << model->getQFileName();
        if (model->getInitStage() != InitStage::PREPARATION)
            continue;
        model->prepareModel();
    }
}

// Load all initialized objects. They must be prepared first!
void MainView::loadModels()
{
    // Objects should only be loaded in the loading stage. Don't want to load unprepared objects or load them twice.
    if (d_initStage != InitStage::LOADING)
        throw "Trying to load models at incorrect stage in game initialization!";

    th_modelLoader.joinThread();

    chrono::time_point<Clock> start = Clock::now();

    qDebug() << "====================================================";
    qDebug() << "================ LOADING MODELS NOW ================";
    qDebug() << "====================================================";

    loadModelsInStorage(d_models);

    if (!d_cubeMapLoaded)
    {
        qDebug() << "Loading model: CubeMap";
        LoadCubeMap();
        d_cubeMapLoaded = true;
    }

    chrono::time_point<Clock> end = Clock::now();
    chrono::milliseconds diff = chrono::duration_cast<chrono::milliseconds>(end - start);

    qDebug() << "Loading models took" << diff.count() << "ms\n";

    d_initStage = InitStage::DONE;
}

void MainView::loadModelsInStorage(EntityStorage &storage)
{
    //#pragma omp parallel for
    for (auto &modelEntry : storage) // Doesn't work with OpenMP until OpenMP5.0 has been implemented.
    {                                // Expected to be part of GCC9 in early 2019.
        Model *model = d_models.getModel(modelEntry);
        qDebug() << "Loading model:" << model->getQFileName();
        if (model->getInitStage() != InitStage::LOADING)
            continue;
        model->loadModel();
    }

    emit modelListChanged();
}

Model *MainView::initModel(QString resourcesDir, QString relativeDir)
{
    Model *model = new Model(this);
    model->initRead(resourcesDir.toStdString(), relativeDir.toStdString());

    qDebug() << "Going to insert model:" << model->getQFileName();
    if (d_models.insertModel(model) == InsertStatus::ALREADYEXISTED)
    {
        qDebug() << "Already existed! Using that one now!";
        model = d_models.getModel(model->getFileName());
    }

    return model;
}