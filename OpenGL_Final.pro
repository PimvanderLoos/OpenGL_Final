#-------------------------------------------------
#
# Project created by QtCreator 2017-01-24T12:26:14
#
#-------------------------------------------------

QT += core gui

QMAKE_CXXFLAGS += -std=c++17 -O3 -Wall -fopenmp -Wno-comment
QMAKE_LFLAGS += -lstdc++fs

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -lstdc++fs -lboost_serialization

TARGET = OpenGL_Final
TEMPLATE = app

CONFIG += -std=gnu++17

SOURCES += src/main.cpp \
src/MainView/data.cpp \
src/MainView/mainview.cpp \
src/MainView/saveLoad.cpp \
src/MainView/loadCubeMap.cpp \
src/MainView/user_input.cpp \
src/MainView/utility.cpp \
src/MainView/DSA.cpp \
src/MainView/loadShaders.cpp \
src/MainView/initObjects.cpp \
src/MainView/loadSplashScreen.cpp \
src/MainView/createShaders.cpp \
src/MainView/paintGL.cpp \
src/MainWindow/mainwindow.cpp \
src/Model/model.cpp \
src/Model/subModel.cpp \
src/Model/prepareModel.cpp \
src/Model/loadModel.cpp \
src/Model/model_gettersSetters.cpp \
src/MatLib/matLib.cpp \
src/AABB/AABB.cpp \
src/Camera/camera.cpp \
src/FrameBuffer/frameBuffer.cpp \
src/Entity/entity.cpp \
src/Entity/entity_water.cpp \
src/Entity/entity_gettersSetters.cpp \
src/EntityStorage/entityStorage.cpp \
src/MyShaderProgram/myShaderProgram_AABB.cpp \
src/MyShaderProgram/myShaderProgram_Cubemap.cpp \
src/MyShaderProgram/myShaderProgram_Gouraud.cpp \
src/MyShaderProgram/myShaderProgram_Normal.cpp \
src/MyShaderProgram/myShaderProgram_Phong.cpp \
src/MyShaderProgram/myShaderProgram_Splash.cpp \
src/MyShaderProgram/myShaderProgram_Water.cpp

HEADERS += src/MainView/mainview.h \
src/MainView/mainview.ih \
src/MainWindow/mainwindow.h \
src/MainWindow/ui_mainwindow.h \
src/Model/model.h \
src/Vertex/vertex.h \
src/MatLib/matLib.h \
src/MyStorage/myStorage.h \
src/AABB/AABB.h \
src/Camera/camera.h \
src/FrameBuffer/frameBuffer.h \
src/Entity/entity.h \
src/Entity/entity_water.h \
src/Entity/entity.ih \
src/ThreadSafeUnorderedMap/threadSafeUnorderedMap.h \
src/MyShaderProgram/myShaderProgram.h \
src/MyShaderProgram/myShaderProgram.h \
src/MyShaderProgram/myShaderProgram_Gouraud.h \
src/MyShaderProgram/myShaderProgram_Normal.h \
src/MyShaderProgram/myShaderProgram_Splash.h \
src/MyShaderProgram/myShaderProgram_Water.h \
src/MyShaderProgram/myShaderProgram_AABB.h \
src/MyShaderProgram/myShaderProgram_Cubemap.h \
src/MyShaderProgram/myShaderProgram_Phong.h \
src/TextureData/textureData.h \
src/MeshData/meshData.h \
src/EntityStorage/entityStorage.h \
src/platformSpecificIncludes.ih \
src/Model/model.ih

FORMS += src/MainWindow/mainwindow.ui

RESOURCES += shaders.qrc

DISTFILES += \
    CMakeLists.txt
