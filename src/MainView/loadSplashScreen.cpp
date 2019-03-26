#include "mainview.ih"

// Init, prepare, and load the splashscreen. The splash screen is render-ready after calling this function.
void MainView::loadSplashScreen()
{
    d_splashScreen = new Entity(this);
    Model *splashModel = new Model(this);
    splashModel->setWriteModel(false);
    SubModel *sm = new SubModel();
    splashModel->addSubModel(sm);
    d_splashScreen->setModel(splashModel);

    QVector<float> *meshData = new QVector<float>;
    meshData->reserve(48);

    QVector3D a(-1,-1,-1); // BOT LEFT
    QVector3D b( 1,-1,-1); // BOT RIGHT
    QVector3D c(-1, 1,-1); // TOP LEFT
    QVector3D d( 1, 1,-1); // TOP RIGHT
    // ORDER: ABD, ADC

    meshData->append(a.x());
    meshData->append(a.y());
    meshData->append(a.z());
    meshData->append(0);
    meshData->append(0);
    meshData->append(1);
    meshData->append(0);
    meshData->append(0);

    meshData->append(b.x());
    meshData->append(b.y());
    meshData->append(b.z());
    meshData->append(0);
    meshData->append(0);
    meshData->append(1);
    meshData->append(1);
    meshData->append(0);

    meshData->append(d.x());
    meshData->append(d.y());
    meshData->append(d.z());
    meshData->append(0);
    meshData->append(0);
    meshData->append(1);
    meshData->append(1);
    meshData->append(1);


    meshData->append(a.x());
    meshData->append(a.y());
    meshData->append(a.z());
    meshData->append(0);
    meshData->append(0);
    meshData->append(1);
    meshData->append(0);
    meshData->append(0);

    meshData->append(d.x());
    meshData->append(d.y());
    meshData->append(d.z());
    meshData->append(0);
    meshData->append(0);
    meshData->append(1);
    meshData->append(1);
    meshData->append(1);

    meshData->append(c.x());
    meshData->append(c.y());
    meshData->append(c.z());
    meshData->append(0);
    meshData->append(0);
    meshData->append(1);
    meshData->append(0);
    meshData->append(1);

    sm->getMeshData().d_meshSize = 18;
    sm->getMeshData().d_meshData = meshData;
    d_splashScreen->setScale(100);

    sm->getMaterial().map_Kd.d_file = d_resourcesDir +  "splashScreen/SplashScreen.png";
    splashModel->prepareTextures(*sm);
    splashModel->loadModel();
}