#include "mainview.ih"

using namespace std;
using Clock = std::chrono::steady_clock;

MainView::MainView(QWidget *parent) : QOpenGLWidget(parent)
{
    qDebug() << "MainView constructor";

    d_timer = new QTimer(this);
    connect(d_timer, SIGNAL(timeout()), this, SLOT(update()));
    d_timer->start(200);

    d_initStage = InitStage::PREPARATION;
    d_startTime = Clock::now();

    drawBoxes.setVal(false);
    bumpMap.setVal(false);
    specMap.setVal(true);
    diffMap.setVal(true);

    d_baseDir = getCurrentPath().c_str();
    findSpecialDirectories();

    d_shaderTypeConversion["DEFAULT"] = ShaderType::DEFAULT;
    d_shaderTypeConversion[ "WATER" ] = ShaderType::WATER;
}

MainView::~MainView()
{
    makeCurrent();
    debugLogger->stopLogging();
    delete debugLogger;

    glDeleteBuffers(1, &d_meshVBO);

    delete d_normalShader;
    delete d_gouraudShader;
    delete d_phongShader;
    delete d_splashShader;
    delete d_waterShader;
    delete d_aabbShader;
    delete d_cubemapShader;

    delete d_dummyEntity;

    delete d_refractionBuffer;

    delete d_aabbModel;

    doneCurrent();
}

// --- OpenGL initialization
void MainView::initializeGL()
{
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    debugLogger = new QOpenGLDebugLogger();
    connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ),
             this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

    if (debugLogger->initialize())
    {
        qDebug() << ":: Logging initialized";
        debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
        debugLogger->enableMessages();
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CLIP_DISTANCE0);

    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0, 1.0, 0.0, 1.0);

    light.pos = QVector3D(3698, 4321, 6940);
    light.col = {1.0f, 1.0f, 1.0f};
    d_viewTransform.setToIdentity();

    createShaderProgram();

    // Set the center point for rotations etc.
    d_baseCamRot = QVector3D(0.0,0.0,0.0);
    d_mainCamera = Camera(QVector3D(0.0,0.0,-10.0f));
    d_mainCamera.setType(CameraType::MAIN);

    initRefractionBuffer();

    // Initialize transformations
    updateProjectionTransform();

    loadSplashScreen();

    // Load default scene.
    threadedLoadScene("");
    d_dummyEntity = new Entity(this);
    d_dummyEntity->setScale(5);
    d_dummyEntity->setModel(nullptr);
    d_dummyEntity->generateAABB();
    d_dummyEntity->setCameraOffset(d_freeCamOffset);

    d_aabbModel = new Model(this);
    d_aabbModel = initModel(d_resourcesDir, "cube/");
    d_aabbModel->setScale(1);
    d_aabbModel->prepareModel();
    d_aabbModel->loadModel();
}

double MainView::isPositionUnderWater(Entity_Water const *water, double x, double y, double z)
{
    // First check if the position is even above/under the water and not just next to it.
    float deltaX = x - water->getPosition().x();
    float deltaZ = z - water->getPosition().z();

    // If the object isn't under or above the water, max double is returned.
    if (abs(deltaX) >= water->getTotalScale() / 2.0F ||
        abs(deltaZ) >= water->getTotalScale() / 2.0F)
        return numeric_limits<double>::max();

    return y - water->getPosition().y();
}

float MainView::getTime() const
{
    return d_time;
}

void MainView::updateTime()
{
    d_time = chrono::duration_cast<chrono::milliseconds>(Clock::now() - this->d_startTime).count();
}

void MainView::resizeGL(int newWidth, int newHeight)
{
    updateProjectionTransform();

}

void MainView::updateProjectionTransform()
{
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());
    d_projectionTransform.setToIdentity();
    d_projectionTransform.perspective(80 / d_scale, aspect_ratio, 0.01, 1000);
    updateFrameBuffersDimensions();
}

void MainView::updateFrameBuffersDimensions()
{
    if (d_refractionBuffer == nullptr)
        return;

    d_refractionBuffer->updateDims(width(), height());

    for (auto &modelEntry : d_models)
    {
        Model *model = d_models.getModel(modelEntry);
        vector<Entity *> *entities = d_models.getEntitiesOfModel(modelEntry);

        if (entities == nullptr || model->getShaderType() != ShaderType::WATER)
            continue;

        for (Entity *entity_base : *entities)
        {
            Entity_Water *water = reinterpret_cast<Entity_Water *>(entity_base);
            if (water->isFBOgenerated())
                water->getReflectionBuffer()->updateDims(width(), height());
        }
    }
}

// Update movement.
void MainView::processMovement()
{
    float moveSpeed = 14;
    if (holdShift)
        moveSpeed *= 5.0f;

    QVector3D controlVec(holdW ? -moveSpeed : holdS ? moveSpeed : 0,
                         holdSpace ? -moveSpeed : holdCtrl ? moveSpeed : 0,
                         holdA ? -moveSpeed : holdD ? moveSpeed : 0);

    d_mainEntity->addVelocity(controlVec);

    double rot = 0;

    if (holdQ)
        rot -= 1.2f;
    if (holdE)
        rot += 1.2f;

    if (rot != 0)
        d_mainEntity->addRotVelocity(0, rot, 0);
    d_mainEntity->tick(d_time);
}

// Update d_viewTransform and mainModelViewTransform.
void MainView::updateViewTransform(Camera const &camera)
{
    // Only move the camera when it's allowed or if it's a dummy (otherwise, what purpose does the dummy server?).
    if (!d_holdCamera || isMainEntityDummy())
        d_mainCamera.focusOnEntity();
    d_viewTransform.setToIdentity();
    d_viewTransform.rotate(QQuaternion::fromEulerAngles(camera.getRotation()));
    d_viewTransform.rotate(QQuaternion::fromEulerAngles(d_globalRotation));
    d_viewTransform.translate(-camera.getPosition());
}

QMatrix4x4 MainView::getModelTransform(Entity &entity)
{
    QMatrix4x4 retMatrix;
    QVector3D position = entity.getPosition();

    retMatrix.translate(position);
    retMatrix.scale(entity.getTotalScale());

    // Get and apply rotation of the object around its own center.
    retMatrix.rotate(QQuaternion::fromEulerAngles(entity.getModel()->getRotationOffset() +
                                                  entity.getRotationOffset() -
                                                  entity.getRotation()));
    return retMatrix;
}

QMatrix4x4 MainView::getModelTransform(AABB &aabb)
{
    QMatrix4x4 retMatrix;
    QVector3D position = aabb.getCenter();

    retMatrix.translate(position);
    retMatrix.scale(aabb.getSize());

    return retMatrix;
}

// --- Public interface
void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    d_globalRotation = QVector3D(static_cast<float>(rotateX % 360),
                                 static_cast<float>(rotateY % 360),
                                 static_cast<float>(rotateZ % 360));
}

void MainView::addRotation(int rotateX, int rotateY, int rotateZ)
{
    setRotation(static_cast<int>(d_mainCamera.getRotation().x()) + rotateX,
                static_cast<int>(d_mainCamera.getRotation().y()) + rotateY,
                static_cast<int>(d_mainCamera.getRotation().z()) + rotateZ);
}

void MainView::resetRotation()
{
    d_globalRotation = d_baseCamRot;
}

void MainView::setScale(int newScale)
{
    d_scale = static_cast<float>(newScale) / 100.f;
    updateProjectionTransform();
    emit scaleValueChanged();
}

void MainView::setShadingMode(ShadingMode shading)
{
    qDebug() << "Changed shading to" << shading;

    d_activeShader = shading;
    update();
}

void MainView::setHitBoxes(bool print)
{
    this->drawBoxes.setVal(print);
}

void MainView::toggleNoClip()
{
    d_mainEntity->setNoClip(!d_mainEntity->noClip());
}

// --- Private helpers
void MainView::onMessageLogged( QOpenGLDebugMessage Message )
{
    if (
//           Message.id() == 131218 // Unsure, but read that it's useless
//        ||
           Message.id() == 131204 // Texture state usage warning; More annoying than useful.
        ||
           Message.id() == 131185 // Buffer detailed info: Buffer object ... (bound to GL_ARRAY_BUFFER_ARB, usage hint
                        // is GL_STATIC_DRAW) will use VIDEO memory as the source for buffer object operations
                        // NOTE: This one CAN be useful, for this error: "GL_INVALID_FRAMEBUFFER_OPERATION error
                        // generated. Operation is not valid because a bound framebuffer is not framebuffer complete."
        ||
           Message.id() == 131169 // Framebuffer detailed info: The driver allocated storage for renderbuffer...
        ||
           Message.id() == 1282
       )
        return;
    qDebug() << " → Log:" << Message;
}

std::string MainView::getCurrentPath()
{
    char cCurrentPath[FILENAME_MAX];

    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
        return nullptr;

    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';

    return string(cCurrentPath) + "/"; // All dirs in this software MUST end in a '/'.
}

void eraseSubStr(string &mainStr, string const &toErase)
{
    size_t pos;
    while((pos = mainStr.find(toErase)) != string::npos)
        mainStr.erase(pos, toErase.length());
}

void MainView::findSpecialDirectories()
{
    string offset = "";
    string baseDir = d_baseDir.toStdString(); // No Qt bs.

    if (experimental::filesystem::exists(baseDir + "../scenes/"))
        offset = "../";
    else if (experimental::filesystem::exists(baseDir + "../../scenes/"))
        offset = "../../";
    else
    {
        for (auto &it : experimental::filesystem::directory_iterator(baseDir + "../"))
        {
            if (!experimental::filesystem::is_directory(it))
                continue;

            for (auto &it2 : experimental::filesystem::directory_iterator(it))
            {
                if (!experimental::filesystem::is_directory(it2))
                    continue;

                experimental::filesystem::path path = it2.path();

                if (path.stem() == "scenes")
                {
                    string pathStr = path.c_str();
                    eraseSubStr(pathStr, baseDir);
                    offset = pathStr;
                    eraseSubStr(offset, path.stem());
                    break;
                }
            }
        }
    }

    if (!experimental::filesystem::exists(baseDir + offset.c_str() + "scenes/"))
        qDebug() << "Failed to find the scenes directory! (and resources by extension). "
                    "This software cannot function without this!";

    d_scenesDir    = d_baseDir + offset.c_str() + "scenes/";
    d_resourcesDir = d_baseDir + offset.c_str() + "resources/";

    qDebug() << "scenes directory =" << d_scenesDir << " resources directory =" << d_resourcesDir;
}

// Select the "next" object in either direction (step of 1 or -1).
void MainView::selectNextEntity(int const increment)
{
    if (increment != 1 && increment != -1)
        return;

    // Abandon when no entities exist.
    if (d_models.entityCount() == 0)
        return;

    size_t const maxIDX = d_models.entityCount() - 1;
    size_t currentID = d_mainEntity->getID();

    // If the main entity is currently a dummy entity, select either the first or the last entity (depending on order).
    if (isMainEntityDummy())
    {
        if (d_previousEntity == nullptr)
        {
            // If a dummy entity is being used when trying to switch, go to the previous entity if it exists (and if
            // previous was pressed). Otherwise go to either 0 or max.
            setMainEntity(d_models.getEntityAt(increment == 1 ? 0 : maxIDX));
            return;
        }
        if (increment == -1)
        {
            setMainEntity(d_previousEntity);
            return;
        }
        currentID = d_previousEntity->getID();
    }

    size_t currentIDX = 0;
    for (auto &modelEntry : d_models)
    {
        vector<Entity *> *entities = d_models.getEntitiesOfModel(modelEntry);

        if (entities == nullptr)
            continue;

        for (auto entity : *entities)
        {
            if (entity->getID() == currentID)
                goto entity_found;
            ++currentIDX;
        }
    }
    entity_found:

    size_t newIDX = currentIDX == 0      && increment == -1 ? maxIDX :
                    currentIDX == maxIDX && increment ==  1 ? 0      : currentIDX + increment;
    setMainEntity(d_models.getEntityAt(newIDX));
}

void MainView::switchToDummyEntity()
{
    if (isMainEntityDummy())
        return;

    QVector3D newPos;
    d_freeCamOffset = d_mainEntity->getCameraOffset();
    newPos = d_mainCamera.getPosition()      +
             d_mainEntity->getLookVector()   * d_freeCamOffset.x() +
             d_mainEntity->getHeightVector() * d_freeCamOffset.y() +
             d_mainEntity->getSideVector()   * d_freeCamOffset.z();
    d_dummyEntity->setPosition(newPos);
    d_dummyEntity->setRotation(d_mainEntity->getRotation());
    d_previousEntity = d_mainEntity;
    setMainEntity(d_dummyEntity);
    d_dummyEntity->setCameraOffset(d_freeCamOffset);
}

void MainView::setMainEntity(Entity *entity)
{
    d_mainEntity = entity;
    d_mainCamera.setEntity(entity);

    emit mainEntityChanged();
}

bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
{
    return s1.toLower() < s2.toLower();
}

QStringList MainView::getModelList()
{
    QStringList ret;
    for (auto &model : d_models)
        ret.append(model.second.first->getQFileName());
    return ret;
}

QStringList MainView::getSortedModelList()
{
    QStringList ret = getModelList();
    qSort(ret.begin(), ret.end(), caseInsensitiveLessThan);
    return ret;
}

Entity const *MainView::getMainEntity() const
{
    return d_mainEntity;
}

Entity *MainView::getMainEntity()
{
    return d_mainEntity;
}

void MainView::modelSelected(QString const &newModel)
{
    if (isMainEntityDummy())
        return;

    Model *model = d_models.getModel(newModel.toStdString());
    if (model == nullptr)
    {
        qDebug() << "Failed to find model:" << newModel;
        return;
    }
    setMainEntity(d_models.updateEntity(d_mainEntity, model));
}

void MainView::printMainLocation()
{
    qDebug() << "MainEntity is at:" << d_mainEntity->getPosition();
}

void MainView::toggleHoldCamera()
{
    d_holdCamera = !d_holdCamera;
    if (d_holdCamera && isMainEntityDummy() && d_previousEntity != nullptr)
        setMainEntity(d_previousEntity);
}
