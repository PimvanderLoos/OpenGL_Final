cmake_minimum_required(VERSION 3.5)
project(game)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Quick REQUIRED)
FIND_PACKAGE(Boost REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

EXECUTE_PROCESS( COMMAND gcc -dumpversion GCC_VERSION )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3 -pedantic -Wall -Werror -Wno-error=unused-variable -fopenmp -Wno-comment")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pedantic -Wall -Werror -fopenmp -Wno-comment -g") # debug
include_directories("src")
include_directories("resources")

set(RESOURCE_FILES shaders.qrc)

set(SOURCE_FILES src/main.cpp src/MainView/data.cpp src/MainView/mainview.cpp src/MainWindow/mainwindow.cpp
        src/Model/model.cpp src/Model/subModel.cpp src/MainView/user_input.cpp src/MainView/utility.cpp
        src/MainView/DSA.cpp src/MatLib/matLib.cpp src/MainView/loadShaders.cpp
        src/MainView/initObjects.cpp src/MainView/loadSplashScreen.cpp src/MainView/createShaders.cpp
        src/MainView/paintGL.cpp src/AABB/AABB.cpp src/Camera/camera.cpp src/FrameBuffer/frameBuffer.cpp
        src/Entity/entity.cpp src/Model/prepareModel.cpp src/Model/loadModel.cpp src/EntityStorage/entityStorage.cpp
        src/Model/model_gettersSetters.cpp src/Entity/entity_gettersSetters.cpp src/Entity/entity_water.cpp
        src/MainView/saveLoad.cpp src/MainView/loadCubeMap.cpp src/MyShaderProgram/myShaderProgram_AABB.cpp
        src/MyShaderProgram/myShaderProgram_Cubemap.cpp src/MyShaderProgram/myShaderProgram_Gouraud.cpp
        src/MyShaderProgram/myShaderProgram_Normal.cpp src/MyShaderProgram/myShaderProgram_Phong.cpp
        src/MyShaderProgram/myShaderProgram_Splash.cpp src/MyShaderProgram/myShaderProgram_Water.cpp)

set(INTERNAL_HEADER_FILES src/MainView/mainview.ih src/platformSpecificIncludes.ih src/Model/model.ih
        src/Entity/entity.ih
        )

set(HEADER_FILES src/MainView/mainview.h src/MainWindow/mainwindow.h src/MainWindow/ui_mainwindow.h src/Model/model.h
        src/Vertex/vertex.h src/MatLib/matLib.h src/MyStorage/myStorage.h src/AABB/AABB.h src/Camera/camera.h
        src/FrameBuffer/frameBuffer.h src/Entity/entity.h src/ThreadSafeUnorderedMap/threadSafeUnorderedMap.h
        src/MyShaderProgram/myShaderProgram.h src/MyShaderProgram/myShaderProgram.h
        src/MyShaderProgram/myShaderProgram_Gouraud.h src/MyShaderProgram/myShaderProgram_Normal.h
        src/MyShaderProgram/myShaderProgram_Splash.h src/MyShaderProgram/myShaderProgram_Water.h
        src/MyShaderProgram/myShaderProgram_AABB.h src/MyShaderProgram/myShaderProgram_Cubemap.h
        src/MyShaderProgram/myShaderProgram_Phong.h src/TextureData/textureData.h src/MeshData/meshData.h
        src/EntityStorage/entityStorage.h src/Entity/entity_water.h
        )

include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

add_executable(game ${HEADER_FILES} ${INTERNAL_HEADER_FILES} ${SOURCE_FILES} ${RESOURCE_FILES})

set(UI_FILES src/MainWindow/mainwindow.ui)
QT5_WRAP_UI(${UI_FILES})

set(QT_INCLUDES Qt5::Core Qt5::Widgets Qt5::Quick)

target_link_libraries(game ${QT_INCLUDES} stdc++fs -lboost_serialization)

message(${QT_INCLUDES})
