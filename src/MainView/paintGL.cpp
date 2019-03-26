#include "mainview.ih"

using namespace std;
using Clock = std::chrono::steady_clock;

void MainView::unbindCurrentFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, d_originalFBO);
    glViewport(0, 0, width(), height());
}

// --- OpenGL drawing
void MainView::paintGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (d_initStage != InitStage::DONE)
    {
        paintGL_Splash();
        update();
        if (d_initStage == InitStage::LOADING)
        {
            loadModels();
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &d_originalFBO);

            updateTime();
         }
        return;
    }

    if (d_mainEntity->hasMoved())
        emit mainEntityMoved();

    // Check how many ms ago the last catch was caught.
    float lastCatch = chrono::duration_cast<chrono::milliseconds>(Clock::now() - this->d_lastCatch).count();
    // Make the screen blink green.
    if (lastCatch < 90)
    {
        glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        update();
        return;
    }

    auto start = Clock::now();

    // Set the current shader, which is the shader that will be used for this frame,
    // even if the d_activeShader is changed mid-way through
    d_currentShader = d_activeShader;

    // Clear the screen before rendering
    glClearColor(0.5f, 0.7f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    processMovement();
    QVector4D clipPlane(0, 1, 0, 0);

    glEnable(GL_CLIP_DISTANCE0);

    // Generate reflection buffers for all water objects.
    for (auto &modelEntry : d_models)
    {
        Model *model = d_models.getModel(modelEntry);
        vector<Entity *> *entities = d_models.getEntitiesOfModel(modelEntry);

        if (entities == nullptr || model->getShaderType() != ShaderType::WATER ||
                                   model->getInitStage()  != InitStage::DONE)
            continue;

        for (Entity *entity_base : *entities)
        {
            Entity_Water *water = reinterpret_cast<Entity_Water *>(entity_base);
            // First make sure the water entity's FBO is actually properly generated.
            // It's generated here and not on entity creation because of OpenGL fuckery.
            water->verifyFBO();
            // Render reflection framebuffer.
            Camera reflectionCam(d_mainCamera);
            reflectionCam.setType(CameraType::REFLECTION);
            reflectionCam.invertPitch();
            reflectionCam.move(0, -2 * (reflectionCam.getPosition().y() - water->getPosition().y()), 0);

            water->getReflectionBuffer()->bindFrameBuffer();
            water->getReflectionBuffer()->clearBuffer();
            clipPlane.setW(-water->getPosition().y() - 0.07f); // Subtract small offset to prevent seams
            renderScene(reflectionCam, clipPlane, true);
        }
    }

    // Render refraction framebuffer.
    d_refractionBuffer->bindFrameBuffer();
    d_refractionBuffer->clearBuffer();

    clipPlane.setY(-1.0f);
    clipPlane.setW(-clipPlane.w() + 0.07f); // Add small offset to prevent seams
    renderScene(d_mainCamera, clipPlane, true);

    glDisable(GL_CLIP_DISTANCE0);

    // Render objects as normal.
    clipPlane = QVector4D(0, -1, 0, 9999999);
    unbindCurrentFrameBuffer();
    renderScene(d_mainCamera, clipPlane, false);


    d_waterShader->bind();
    for (auto &modelEntry : d_models)
    {
        Model *model = d_models.getModel(modelEntry);
        vector<Entity *> *entities = d_models.getEntitiesOfModel(modelEntry);

        if (entities == nullptr || model->getShaderType() != ShaderType::WATER ||
                                   model->getInitStage()  != InitStage::DONE)
            continue;

        for (Entity *entity_base : *entities)
            renderWater(reinterpret_cast<Entity_Water *>(entity_base), d_mainCamera, clipPlane);
    }
    d_waterShader->release();

    paint_AABBs(clipPlane);

    update();

    auto end  = Clock::now();
    auto diff = chrono::duration_cast<chrono::microseconds>(end - start);
    d_currentFrameTime = static_cast<float>(diff.count()) / 1000.0f;

    updateBenchMarkData();

    updateTime();
}

void MainView::updateBenchMarkData()
{
    size_t currentTime = static_cast<size_t>(d_time);

    ++d_frameCount;
    if (d_currentFrameTime > d_maxFrameTime)
    {
        d_maxFrameTime = d_currentFrameTime;
        emit maxFrameTimeUpdate();
    }
    d_totalFrameTime += d_currentFrameTime;

    // Update these values every second.
    if (currentTime / 1000 > d_seconds)
    {
        d_seconds = currentTime / 1000;
        // Get number of frames rendered in the last second.
        d_fps = d_frameCount - d_oldFrameCount;
        d_oldFrameCount = d_frameCount;
        // Get average frame time of the frames rendered in the last second.
        d_avgFrameTime = d_totalFrameTime / d_fps;

        emit fpsUpdate();
        emit idlePercentageChanged();
        emit avgFrameTimeUpdate();

        d_maxFrameTime = 0.0f;
        d_totalFrameTime = 0.0f;
    }
}

void MainView::renderScene(Camera const &camera, QVector4D const &clipPlane, bool offscreen)
{
    updateViewTransform(camera);
    d_projectionViewTransform = d_projectionTransform * d_viewTransform;
    this->paintGL_CubeMap();

    // Load current shader
    (this->*s_shadingloader[static_cast<int>(d_currentShader)])(false);

    for (auto &modelEntry : d_models)
    {
        Model *model = d_models.getModel(modelEntry);
        vector<Entity *> *entities = d_models.getEntitiesOfModel(modelEntry);

        if (entities == nullptr || model->getShaderType() == ShaderType::WATER ||
                                   model->getInitStage()  != InitStage::DONE)
            continue;

        if (model->getShaderType() == ShaderType::DEFAULT)
            for (Entity *entity : *entities)
                paint_entity(entity, clipPlane, camera.getPosition(), offscreen);
    }

    // Unload current shader
    (this->*s_shadingloader[ static_cast<int>(d_currentShader)]) (true);
}

void MainView::paint_entity(Entity *entity, QVector4D const &clipPlane, QVector3D const &camPos, bool offscreen)
{
    QMatrix4x4 modelTransform = getModelTransform(*entity);
    QMatrix4x4 PMV = d_projectionViewTransform * modelTransform;
    QMatrix3x3 normalTransform = modelTransform.normalMatrix();

    (this->*s_shadingmode[static_cast<int>(d_currentShader)]) (PMV, modelTransform, normalTransform, entity, clipPlane,
                                                               camPos);
}

// Phong-specific PaintGL part:
void MainView::paintGL_Phong(QMatrix4x4 const &PMV, QMatrix4x4 const &modelTransform, QMatrix3x3 const &normalTransform,
                             Entity const *entity, QVector4D const &clipPlane, QVector3D const &camPos)
{
    RenderData rDat;
    rDat.PMV = &PMV;
    rDat.modelTransform = &modelTransform;
    rDat.normalTransform = &normalTransform;
    rDat.clipPlane = &clipPlane;
    rDat.light = &light;
    rDat.camPos = &camPos;
    rDat.diffMap = diffMap.getValue();
    rDat.specMap = specMap.getValue();
    rDat.bumpMap = bumpMap.getValue();
    rDat.entity = entity;
    d_phongShader->paint(rDat);
}

void MainView::renderWater(Entity_Water *entity, Camera const &camera, QVector4D const &clipPlane)
{
    entity->setMoveFactor(fmod(entity->getMoveFactor() + getTime() / 1000 * entity->getWaveSpeed(), 1.0f));

    QMatrix4x4 modelTransform = getModelTransform(*entity);
    QMatrix4x4 PMV = d_projectionViewTransform * modelTransform;
    QMatrix3x3 normalTransform = modelTransform.normalMatrix();
    paintGL_Water(PMV, normalTransform, entity, clipPlane, modelTransform, d_viewTransform, camera.getPosition());
}

void MainView::paint_AABBs(QVector4D const &clipPlane)
{
    d_aabbShader->bind();
    // Paint AABBs is necessary.
    if (drawBoxes.getValue())
    {
        for (auto &modelEntry : d_models)
        {
            Model *model = d_models.getModel(modelEntry);
            vector<Entity *> *entities = d_models.getEntitiesOfModel(modelEntry);

            if (entities == nullptr || model->getInitStage() != InitStage::DONE)
                continue;

            for (Entity *entity : *entities)
            {
                QMatrix4x4 modelTransform = getModelTransform(entity->getAABB());
                paintGL_AABB(entity, modelTransform, clipPlane);
            }
        }
    }
    d_aabbShader->release();
}

void MainView::paintGL_CubeMap()
{
    d_cubemapShader->bind();
    RenderData rDat;
    rDat.projectionTransform = &d_projectionTransform;
    rDat.camera = &d_mainCamera;
    rDat.skyBoxVAO = d_skyBoxVAO;
    rDat.skyBoxTexturePtr = d_skyBoxTexturePtr;
    d_cubemapShader->paint(rDat);
    d_cubemapShader->release();
}

// Normal-specific PaintGL part:
void MainView::paintGL_Normal(QMatrix4x4 const &PMV, QMatrix4x4 const &modelTransform, QMatrix3x3 const &normalTransform,
                              Entity const *entity, QVector4D const &clipPlane, QVector3D const &camPos)
{
    RenderData rDat;
    rDat.PMV = &PMV;
    rDat.modelTransform = &modelTransform;
    rDat.normalTransform = &normalTransform;
    rDat.entity = entity;
    rDat.clipPlane = &clipPlane;
    d_normalShader->paint(rDat);
}

void MainView::paintGL_AABB(Entity const *entity, QMatrix4x4 const &modelTransform, QVector4D const &clipPlane)
{
    RenderData rDat;
    rDat.projectionTransform = &d_projectionTransform;
    rDat.viewTransform = &d_viewTransform;
    rDat.modelTransform = &modelTransform;
    rDat.entity = entity;
    rDat.clipPlane = &clipPlane;
    rDat.model = d_aabbModel;
    d_aabbShader->paint(rDat);
}

// Gouraud-specific PaintGL part:
void MainView::paintGL_Gouraud(QMatrix4x4 const &PMV, QMatrix4x4 const &modelTransform, QMatrix3x3 const &normalTransform,
                               Entity const *entity, QVector4D const &clipPlane, QVector3D const &camPos)
{
    RenderData rDat;
    rDat.PMV = &PMV;
    rDat.modelTransform = &modelTransform;
    rDat.normalTransform = &normalTransform;
    rDat.clipPlane = &clipPlane;
    rDat.light = &light;
    rDat.camPos = &camPos;
    rDat.diffMap = diffMap.getValue();
    rDat.specMap = specMap.getValue();
    rDat.bumpMap = bumpMap.getValue();
    rDat.entity = entity;
    d_gouraudShader->paint(rDat);
}

// Water-specific PaintGL part:
void MainView::paintGL_Water(QMatrix4x4 const &PMV, QMatrix3x3 const &normalTransform, Entity_Water const *entity,
                             QVector4D const &clipPlane, QMatrix4x4 const &modelTransform,
                             QMatrix4x4 const &viewTransform, QVector3D const &camPos)
{
    RenderData rDat;
    rDat.PMV = &PMV;
    rDat.normalTransform = &normalTransform;
    rDat.modelTransform = &modelTransform;
    rDat.viewTransform = &viewTransform;
    rDat.projectionTransform = &d_projectionTransform;
    rDat.entity = entity;
    rDat.clipPlane = &clipPlane;
    rDat.light = &light;
    rDat.camPos = &camPos;
    rDat.time = fmod(getTime() / 1000.0f * entity->getWaveSpeed(), 1.0f);
    rDat.reflectBufferTex = entity->getReflectionBuffer()->getTexture();
    rDat.refractBufferTex = d_refractionBuffer->getTexture();
    rDat.refractDepthTex  = d_refractionBuffer->getDepthTexture();
    d_waterShader->paint(rDat);
}

// Water-specific PaintGL part:
void MainView::paintGL_Splash()
{
    d_splashShader->bind();
    RenderData rDat;
    QMatrix4x4 pmv = d_projectionTransform * getModelTransform(*d_splashScreen);
    QMatrix3x3 normal = pmv.normalMatrix();
    rDat.PMV = &pmv;
    rDat.normalTransform = &normal;
    rDat.light = &light;
    rDat.entity = d_splashScreen;
    d_splashShader->paint(rDat);
    d_splashShader->release();
}