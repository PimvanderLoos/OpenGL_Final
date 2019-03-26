#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugLogger>
#include <QTimer>
#include <QVector3D>
#include <QMatrix4x4>

#include <thread>
#include <chrono>

#include "../Model/model.h"
#include "../MyStorage/myStorage.h"
#include "../Camera/camera.h"
#include "../FrameBuffer/frameBuffer.h"
#include "../ThreadSafeUnorderedMap/threadSafeUnorderedMap.h"
#include "../MeshData/meshData.h"
#include "../EntityStorage/entityStorage.h"

#include "../MyShaderProgram/myShaderProgram_Normal.h"
#include "../MyShaderProgram/myShaderProgram_Phong.h"
#include "../MyShaderProgram/myShaderProgram_Gouraud.h"
#include "../MyShaderProgram/myShaderProgram_Splash.h"
#include "../MyShaderProgram/myShaderProgram_Water.h"
#include "../MyShaderProgram/myShaderProgram_AABB.h"
#include "../MyShaderProgram/myShaderProgram_Cubemap.h"

struct Option
{
    private:
       bool d_val;
       QString d_name;
    public:
        Option() = default;
        Option(QString name, bool val)
        :
            d_val(val), d_name(name)
        {};

        QString getName()
        {
            return d_name;
        }

        bool getValue() const
        {
            return d_val;
        };

        void toggleVal()
        {
            d_val = !d_val;
        };

        void setVal(bool newVal)
        {
            d_val = newVal;
        };
};

struct MyThread
{
    private:
        std::thread *d_thread = nullptr;
        bool d_running = false;

    public:
        void setThread(std::thread *thread);
        void joinThread();
        bool isRunning() const;
};

inline void MyThread::setThread(std::thread *thread)
{
    d_thread = thread;
    d_running = true;
}

inline void MyThread::joinThread()
{
    d_thread->join();
    d_running = false;
    delete d_thread;
}

inline bool MyThread::isRunning() const
{
    return d_running;
}

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

    private:
        QOpenGLDebugLogger *debugLogger;
        QTimer *d_timer; // d_timer used for animation

        EntityStorage d_models;

        float d_time;
        size_t d_seconds = 0;
        size_t d_frameCount = 0;
        size_t d_oldFrameCount = 0;
        float d_maxFrameTime = 0.0f;
        float d_currentFrameTime = 0.0f;
        float d_avgFrameTime = 0.0f;
        float d_totalFrameTime = 0.0f;
        size_t d_fps = 0;

        QString d_baseDir;
        QString d_scenesDir;
        QString d_resourcesDir;
        QString d_sceneFileExtension = ".scene";
        std::map<std::string, enum ShaderType> d_shaderTypeConversion;

        Entity *d_mainEntity = nullptr;
        Entity *d_dummyEntity;
        Entity *d_previousEntity = nullptr;
        Entity *d_splashScreen;
        Model *d_defaultModel = nullptr;
        Model *d_aabbModel;
        InitStage d_initStage;
        QVector3D d_freeCamOffset = QVector3D(7.5f,-3.5f,0.0);

        bool d_holdCamera = false;
        bool d_cubeMapLoaded = false;

        GLint d_originalFBO; // Original FBO

        FrameBuffer *d_refractionBuffer = nullptr;

        GLuint d_skyBoxVAO;
        GLuint d_skyBoxTexturePtr;

        MyShaderProgram_Normal  *d_normalShader;
        MyShaderProgram_Gouraud *d_gouraudShader;
        MyShaderProgram_Phong   *d_phongShader;
        MyShaderProgram_Splash  *d_splashShader;
        MyShaderProgram_Water   *d_waterShader;
        MyShaderProgram_AABB    *d_aabbShader;
        MyShaderProgram_Cubemap *d_cubemapShader;

        GLuint d_meshVBO;

        float d_defaultScale = 0.85f;
        float d_scale = d_defaultScale;

        // Transforms
        QMatrix4x4 d_projectionTransform;
        QMatrix4x4 d_viewTransform;
        QMatrix4x4 d_projectionViewTransform;

        std::string getCurrentPath();

        Camera d_mainCamera;
        QVector3D d_baseCamRot;
        QVector3D d_globalRotation;

        float walkSpeed = 0.3; // Movement speed.
        float forwardMultiplier = 1.9; // Multiplier for moving forward.

        // Move controls
        bool holdW     = false; // Move forward
        bool holdA     = false; // Move to the left
        bool holdS     = false; // Move backwards
        bool holdD     = false; // Move to the right
        bool holdQ     = false; // Rotate to the left
        bool holdE     = false; // Rotate tot he right
        bool holdSpace = false; // Move up
        bool holdCtrl  = false; // Move down
        bool holdShift = false; // Move faster

        // Modifier keys
        bool altModActive = false;

        // Pointer to member function for the different shaders.
        static void (MainView::*s_shadingmode[]) (QMatrix4x4 const &PMV, QMatrix4x4 const &modelTransform,
                                                  QMatrix3x3 const &normalTransform, Entity const *entity,
                                                  QVector4D  const &clipPlane, QVector3D const &camPos);
        static void (MainView::*s_shadingloader[])(bool isLoaded);

    public:
        QVector<QString> getModelList();
        void modelSelected(QString const &newModel);
        Entity const *getMainEntity() const;
        Entity *getMainEntity();
        void printMainLocation();
        void toggleHoldCamera();

        void saveScene(QString sceneFile);
        void loadScene(QString sceneFile);
        void threadedSaveScene(QString sceneFile);
        void threadedLoadScene(QString sceneFile);
        void convertLoadSavePath(QString &sceneFile);

        size_t score = 0;

        Option drawBoxes;
        Option bumpMap;
        Option specMap;
        Option diffMap;

        enum ShadingMode : GLuint
        {
            PHONG = 0, NORMAL, GOURAUD
        };

        MainView(QWidget *parent = 0);
        ~MainView();

        // Functions for widget input events
        void setRotation(int rotateX, int rotateY, int rotateZ);
        void addRotation(int rotateX, int rotateY, int rotateZ);
        void setScale(int scale);
        void setShadingMode(ShadingMode shading);
        void resetRotation();
        void setHitBoxes(bool print);
        void toggleNoClip();

        void selectNextEntity(int const increment);
        void switchToDummyEntity();
        void setMainEntity(Entity *entity);

        void addDefaultEntity();
        void duplicateMainEntity();
        void killMainEntity();
        bool isMainEntityDummy();

        void findSpecialDirectories(); // Find special directories such as resources and scenes.
                                       // This can be a little tricky because Qt seems to put the executable
                                       // all over the damn place. :(

        Model *initModel(QString baseDir, QString relativeDir);

        void initRefractionBuffer();

        void unbindCurrentFrameBuffer();

        void updateViewTransform(Camera const &camera);

        void processMovement();

        int getScale();

        void updateTime();
        float getTime() const;

        size_t getFPS() const;
        float getMaxFrameTime() const;
        float getAvgFrameTime() const;

        float getDefaultScale() const;

    protected:
        void initializeGL();
        void resizeGL(int newWidth, int newHeight);
        void paintGL();

        // Functions for keyboard input events
        void keyPressEvent  (QKeyEvent *ev);
        void keyReleaseEvent(QKeyEvent *ev);

        // Function for mouse input events
        void mouseDoubleClickEvent(QMouseEvent *ev);
        void mouseMoveEvent       (QMouseEvent *ev);
        void mousePressEvent      (QMouseEvent *ev);
        void mouseReleaseEvent    (QMouseEvent *ev);
        void wheelEvent           (QWheelEvent *ev);

    private slots:
        void onMessageLogged(QOpenGLDebugMessage Message);

    signals:
        void scaleValueChanged();
        void scoreChange();
        void fpsUpdate();
        void maxFrameTimeUpdate();
        void avgFrameTimeUpdate();
        void idlePercentageChanged();
        void modelListChanged();
        void mainEntityChanged();
        void mainEntityMoved();

    private:
        MyThread th_modelLoader;
        std::thread *th_sceneWriter = nullptr;
        bool d_threadWriteInUse = false;
        std::chrono::time_point<std::chrono::steady_clock> d_startTime;
        std::chrono::time_point<std::chrono::steady_clock> d_lastCatch;
        ShadingMode d_activeShader  = ShadingMode::PHONG;
        ShadingMode d_currentShader = d_activeShader;
        Light light;
        void createShaderProgram();

        void updateBenchMarkData();

        double isPositionUnderWater(Entity_Water const *water, double x, double y, double z);

        void renderScene     (Camera const &camera, QVector4D const &clipPlane, bool offscreen);
        void renderWater     (Entity_Water *entity, Camera const &camera, QVector4D const &clipPlane);
        void paint_entity    (Entity *ent, QVector4D const &clipPlane, QVector3D const &camPos, bool offscreen);

        void paintGL_Water   (QMatrix4x4 const &PMV, QMatrix3x3 const &normalTransform, Entity_Water const *entity,
                              QVector4D const &clipPlane, QMatrix4x4 const &modelTransform,
                              QMatrix4x4 const &viewTransform, QVector3D const &camPos); // Paint water shader
        void paintGL_Phong   (QMatrix4x4 const &PMV, QMatrix4x4 const &modelTransform, QMatrix3x3 const &normalTransform,
                              Entity const *entity, QVector4D const &clipPlane, QVector3D const &camPos);
        void paintGL_Normal  (QMatrix4x4 const &PMV, QMatrix4x4 const &modelTransform, QMatrix3x3 const &normalTransform,
                              Entity const *entity, QVector4D const &clipPlane, QVector3D const &camPos);
        void paint_AABBs     (QVector4D const &clipPlane);
        void paintGL_Gouraud (QMatrix4x4 const &PMV, QMatrix4x4 const &modelTransform,  QMatrix3x3 const &normalTransform,
                              Entity const *entity, QVector4D const &clipPlane, QVector3D const &camPos);
        void paintGL_AABB    (Entity const *entity, QMatrix4x4 const &modelTransform, QVector4D const &clipPlane);
        void paintGL_Splash  ();
        void paintGL_CubeMap ();

        void load_Phong  (bool isLoaded);
        void load_Normal (bool isLoaded);
        void load_Gouraud(bool isLoaded);

        void updateProjectionTransform();
        void updateFrameBuffersDimensions();

        QMatrix4x4 getModelTransform(Entity &entity);
        QMatrix4x4 getModelTransform(AABB &aabb);

        QVector<quint8>* imageToBytes(QImage image);

        void LoadCubeMap();
        void loadCubemapTexture(std::string *faces);

        void loadSplashScreen();

    public:
        void prepareModelsInStorage(EntityStorage &storage);
        void loadModelsInStorage(EntityStorage &storage);
        void prepareModels();
        void loadModels();
};

inline int MainView::getScale()
{
    return static_cast<int> (d_scale * 100);
}

#endif
