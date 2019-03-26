#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSplashScreen>
#include <unistd.h>
#include "../MainView/mainview.h"

#include "math.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->mainView, SIGNAL(scaleValueChanged()), this, SLOT(recalc_ScaleSlider()));
    connect(ui->mainView, SIGNAL(fpsUpdate()), this, SLOT(recalc_FPS()));
    connect(ui->mainView, SIGNAL(maxFrameTimeUpdate()), this, SLOT(recalc_avgFrameTime()));
    connect(ui->mainView, SIGNAL(avgFrameTimeUpdate()), this, SLOT(recalc_maxFrameTime()));
    connect(ui->mainView, SIGNAL(modelListChanged()), this, SLOT(recalc_modelList()));
    connect(ui->mainView, SIGNAL(mainEntityChanged()), this, SLOT(on_mainEntityChanged()));
    connect(ui->mainView, SIGNAL(mainEntityMoved()), this, SLOT(updateMainEntityPosition()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ResetScaleButton_clicked(bool checked)
{
    Q_UNUSED(checked);
    ui->ScaleSlider->setValue(static_cast<int>(ui->mainView->getDefaultScale() * 100));
    ui->mainView->setScale(   static_cast<int>(ui->mainView->getDefaultScale() * 100));
}

void MainWindow::on_ScaleSlider_valueChanged(int value)
{
    ui->mainView->setScale(value);
}

void MainWindow::on_PhongButton_toggled(bool checked)
{
    if (checked)
    {
        ui->mainView->setShadingMode(MainView::PHONG);
        ui->mainView->update();
    }
}

void MainWindow::on_NormalButton_toggled(bool checked)
{
    if (checked)
    {
        ui->mainView->setShadingMode(MainView::NORMAL);
        ui->mainView->update();
    }
}

void MainWindow::on_GouraudButton_toggled(bool checked)
{
    if (checked)
    {
        ui->mainView->setShadingMode(MainView::GOURAUD);
        ui->mainView->update();
    }
}

void MainWindow::recalc_ScaleSlider()
{
    ui->ScaleSlider->setValue(ui->mainView->getScale());
}

void MainWindow::recalc_FPS()
{
    ui->lcdNumber_fps->display(static_cast<int>(ui->mainView->getFPS()));
}

void MainWindow::recalc_maxFrameTime()
{
    ui->lcdNumber_frameTime_max->display(ui->mainView->getMaxFrameTime());
}

void MainWindow::recalc_avgFrameTime()
{
    ui->lcdNumber_frameTime_avg->display(ui->mainView->getAvgFrameTime());
}

void MainWindow::on_lineEdit_Console_returnPressed()
{
    qDebug() << ui->lineEdit_Console->text();
    if (ui->lineEdit_Console->text() == "noclip")
        ui->mainView->toggleNoClip();
    else if (ui->lineEdit_Console->text() == "bump")
        ui->mainView->bumpMap.toggleVal();
    else if (ui->lineEdit_Console->text() == "spec")
        ui->mainView->specMap.toggleVal();
    else if (ui->lineEdit_Console->text() == "diff")
        ui->mainView->diffMap.toggleVal();
    else if (ui->lineEdit_Console->text() == "pos")
        ui->mainView->printMainLocation();
    else if (ui->lineEdit_Console->text().split(' ').at(0) == "save")
        ui->mainView->threadedSaveScene(ui->lineEdit_Console->text().split(' ').size() > 1 ? ui->lineEdit_Console->text().split(' ').at(1) : "");
    else if (ui->lineEdit_Console->text().split(' ').at(0) == "load")
        ui->mainView->threadedLoadScene(ui->lineEdit_Console->text().split(' ').size() > 1 ? ui->lineEdit_Console->text().split(' ').at(1) : "");
    else if (ui->lineEdit_Console->text().split(' ').at(0) == "loadModel")
        qDebug() << "UNIMPLEMENTED!";
    ui->lineEdit_Console->clear();
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    ui->mainView->setHitBoxes(arg1 == 2);
}

void MainWindow::on_button_next_entity_clicked()
{
    ui->mainView->selectNextEntity(1);
}

void MainWindow::on_button_previous_entity_clicked()
{
    ui->mainView->selectNextEntity(-1);
}

void MainWindow::on_comboBox_Entity_Model_activated(const QString &modelName)
{
    ui->mainView->modelSelected(modelName);
    on_mainModelChanged();
}

void MainWindow::changeEntityEnabled(bool status)
{
    if (ui->RotationDialX_Entity->isEnabled() == status)
        return;

    ui->RotationDialX_Entity->setEnabled(status);
    ui->RotationDialY_Entity->setEnabled(status);
    ui->RotationDialZ_Entity->setEnabled(status);

    ui->doubleSpinBox_rotationX_Entity->setEnabled(status);
    ui->doubleSpinBox_rotationY_Entity->setEnabled(status);
    ui->doubleSpinBox_rotationZ_Entity->setEnabled(status);

    ui->doubleSpinBox_Scale_Entity->setEnabled(status);
    ui->horizontalSlider_Scale_Entity->setEnabled(status);

    ui->pushButton_Reset_Rotation_Entity->setEnabled(status);
    ui->button_kill_entity->setEnabled(status);
    ui->button_dupe_entity->setEnabled(status);
    ui->comboBox_Entity_Model->setEnabled(status);
}

void MainWindow::changeModelEnabled(bool status)
{
    if (ui->RotationDialX_Model->isEnabled() == status)
        return;

    ui->RotationDialX_Model->setEnabled(status);
    ui->RotationDialY_Model->setEnabled(status);
    ui->RotationDialZ_Model->setEnabled(status);

    ui->doubleSpinBox_rotationX_Model->setEnabled(status);
    ui->doubleSpinBox_rotationY_Model->setEnabled(status);
    ui->doubleSpinBox_rotationZ_Model->setEnabled(status);

    ui->doubleSpinBox_Scale_Model->setEnabled(status);
    ui->horizontalSlider_Scale_Model->setEnabled(status);

    ui->doubleSpinBox_offsetX_Model->setEnabled(status);
    ui->doubleSpinBox_offsetY_Model->setEnabled(status);
    ui->doubleSpinBox_offsetZ_Model->setEnabled(status);
}

void MainWindow::on_mainModelChanged()
{
    Model const *model = ui->mainView->getMainEntity() != nullptr ? ui->mainView->getMainEntity()->getModel() : nullptr;
    if (model == nullptr)
    {
        changeModelEnabled(false);
        return;
    }
    changeModelEnabled(true);

    // Save rotationOffset here, because otherwise it'd be changed by setting the value.
    QVector3D rotationOffset = model->getRotationOffset();
    ui->comboBox_Entity_Model->setCurrentText(model->getQFileName());

    ui->RotationDialX_Model->setValue(static_cast<int>(rotationOffset.x()));
    ui->RotationDialY_Model->setValue(static_cast<int>(rotationOffset.y()));
    ui->RotationDialZ_Model->setValue(static_cast<int>(rotationOffset.z()));

    ui->doubleSpinBox_rotationX_Model->setValue(rotationOffset.x());
    ui->doubleSpinBox_rotationY_Model->setValue(rotationOffset.y());
    ui->doubleSpinBox_rotationZ_Model->setValue(rotationOffset.z());

    QVector3D cameraOffset = model->getCameraOffset();
    ui->doubleSpinBox_offsetX_Model->setValue(cameraOffset.x() * 10);
    ui->doubleSpinBox_offsetY_Model->setValue(cameraOffset.y() * 10);
    ui->doubleSpinBox_offsetZ_Model->setValue(cameraOffset.z() * 10);

    ui->doubleSpinBox_Scale_Model->setValue(model->getScale());
    ui->horizontalSlider_Scale_Model->setValue(static_cast<int>(model->getScale()));
}

void MainWindow::on_mainEntityChanged()
{
    Entity const *entity = ui->mainView->getMainEntity();
    if (entity == nullptr || ui->mainView->isMainEntityDummy())
    {
        changeEntityEnabled(false);
        return;
    }
    changeEntityEnabled(true);

    // See comment in on_mainModelChanged().
    QVector3D rotationOffset = entity->getRotationOffset();
    ui->RotationDialX_Entity->setValue(static_cast<int>(rotationOffset.x()));
    ui->RotationDialY_Entity->setValue(static_cast<int>(rotationOffset.y()));
    ui->RotationDialZ_Entity->setValue(static_cast<int>(rotationOffset.z()));

    ui->doubleSpinBox_rotationX_Entity->setValue(rotationOffset.x());
    ui->doubleSpinBox_rotationY_Entity->setValue(rotationOffset.y());
    ui->doubleSpinBox_rotationZ_Entity->setValue(rotationOffset.z());

    ui->doubleSpinBox_Scale_Entity->setValue(entity->getScale());
    ui->horizontalSlider_Scale_Entity->setValue(static_cast<int>(entity->getScale()));

    if(entity->getModel() != nullptr)
        on_mainModelChanged();

    updateMainEntityPosition();
}

void MainWindow::updateMainEntityPosition()
{
    ui->doubleSpinBox_positionX_Entity->setValue(ui->mainView->getMainEntity()->getPosition().x());
    ui->doubleSpinBox_positionY_Entity->setValue(ui->mainView->getMainEntity()->getPosition().y());
    ui->doubleSpinBox_positionZ_Entity->setValue(ui->mainView->getMainEntity()->getPosition().z());
}

void MainWindow::recalc_modelList()
{
    ui->comboBox_Entity_Model->clear();
    for (auto &modelName : ui->mainView->getModelList())
        ui->comboBox_Entity_Model->addItem(modelName);
    on_mainModelChanged();
}

void MainWindow::on_button_free_cam_clicked()
{
    ui->mainView->switchToDummyEntity();
}

void MainWindow::on_button_new_entity_clicked()
{
    ui->mainView->addDefaultEntity();
}

void MainWindow::on_button_dupe_entity_clicked()
{
    ui->mainView->duplicateMainEntity();
}

void MainWindow::on_button_kill_entity_clicked()
{
    ui->mainView->killMainEntity();
}

void MainWindow::on_button_hold_cam_clicked()
{
    ui->mainView->toggleHoldCamera();
}

void MainWindow::dial_ChangeEntityRotation()
{
    ui->mainView->getMainEntity()->setRotationOffset(QVector3D(ui->RotationDialX_Entity->value(),
                                                               ui->RotationDialY_Entity->value(),
                                                               ui->RotationDialZ_Entity->value()));
}

void MainWindow::text_ChangeEntityRotation()
{
    ui->mainView->getMainEntity()->setRotationOffset(
            QVector3D(static_cast<float>(ui->doubleSpinBox_rotationX_Entity->value()),
                      static_cast<float>(ui->doubleSpinBox_rotationY_Entity->value()),
                      static_cast<float>(ui->doubleSpinBox_rotationZ_Entity->value())));
}

void MainWindow::dial_ChangeModelRotation()
{
    ui->mainView->getMainEntity()->getModel()->setRotationOffset(
            QVector3D(static_cast<float>(ui->RotationDialX_Model->value()),
                      static_cast<float>(ui->RotationDialY_Model->value()),
                      static_cast<float>(ui->RotationDialZ_Model->value())));
}

void MainWindow::text_ChangeModelRotation()
{
    ui->mainView->getMainEntity()->getModel()->setRotationOffset(
            QVector3D(static_cast<float>(ui->doubleSpinBox_rotationX_Model->value()),
                      static_cast<float>(ui->doubleSpinBox_rotationY_Model->value()),
                      static_cast<float>(ui->doubleSpinBox_rotationZ_Model->value())));
}

void MainWindow::text_ChangeModelOffset()
{
    ui->mainView->getMainEntity()->getModel()->setCameraOffset(
            QVector3D(static_cast<float>(ui->doubleSpinBox_offsetX_Model->value() / 10.0f),
                      static_cast<float>(ui->doubleSpinBox_offsetY_Model->value() / 10.0f),
                      static_cast<float>(ui->doubleSpinBox_offsetZ_Model->value() / 10.0f)));
}

void MainWindow::on_RotationDialX_Entity_valueChanged(int value)
{
    dial_ChangeEntityRotation();
    ui->doubleSpinBox_rotationX_Entity->setValue(ui->RotationDialX_Entity->value());
}

void MainWindow::on_RotationDialY_Entity_valueChanged(int value)
{
    dial_ChangeEntityRotation();
    ui->doubleSpinBox_rotationY_Entity->setValue(ui->RotationDialY_Entity->value());
}

void MainWindow::on_RotationDialZ_Entity_valueChanged(int value)
{
    dial_ChangeEntityRotation();
    ui->doubleSpinBox_rotationZ_Entity->setValue(ui->RotationDialZ_Entity->value());
}

void MainWindow::on_pushButton_Reset_Rotation_Entity_clicked(bool checked)
{
    ui->mainView->getMainEntity()->resetRotationOffset();
    ui->RotationDialX_Entity->setValue(0);
    ui->RotationDialY_Entity->setValue(0);
    ui->RotationDialZ_Entity->setValue(0);
    on_mainEntityChanged();
}

void MainWindow::on_horizontalSlider_Scale_Entity_valueChanged(int value)
{
    ui->mainView->getMainEntity()->setScale(value);
    ui->doubleSpinBox_Scale_Entity->setValue(value);
}

void MainWindow::on_doubleSpinBox_rotationX_Entity_valueChanged(double value)
{
    text_ChangeEntityRotation();
    ui->RotationDialX_Entity->setValue(static_cast<int>(ui->doubleSpinBox_rotationX_Entity->value()));
}

void MainWindow::on_doubleSpinBox_rotationY_Entity_valueChanged(double value)
{
    text_ChangeEntityRotation();
    ui->RotationDialY_Entity->setValue(static_cast<int>(ui->doubleSpinBox_rotationY_Entity->value()));
}

void MainWindow::on_doubleSpinBox_rotationZ_Entity_valueChanged(double value)
{
    text_ChangeEntityRotation();
    ui->RotationDialZ_Entity->setValue(static_cast<int>(ui->doubleSpinBox_rotationZ_Entity->value()));
}

void MainWindow::on_doubleSpinBox_Scale_Entity_valueChanged(double value)
{
    ui->mainView->getMainEntity()->setScale(value);
    ui->horizontalSlider_Scale_Entity->setValue(static_cast<int>(value));
}

void MainWindow::on_doubleSpinBox_positionX_Entity_valueChanged(double value)
{
    ui->mainView->getMainEntity()->move(value - ui->mainView->getMainEntity()->getPosition().x(), 0, 0);
}

void MainWindow::on_doubleSpinBox_positionY_Entity_valueChanged(double value)
{
    ui->mainView->getMainEntity()->move(0, value - ui->mainView->getMainEntity()->getPosition().y(), 0);
}

void MainWindow::on_doubleSpinBox_positionZ_Entity_valueChanged(double value)
{
    ui->mainView->getMainEntity()->move(0, 0, value - ui->mainView->getMainEntity()->getPosition().z());
}

void MainWindow::on_RotationDialX_Model_valueChanged(int value)
{
    dial_ChangeModelRotation();
    ui->doubleSpinBox_rotationX_Model->setValue(value);
}

void MainWindow::on_RotationDialY_Model_valueChanged(int value)
{
    dial_ChangeModelRotation();
    ui->doubleSpinBox_rotationY_Model->setValue(value);
}

void MainWindow::on_RotationDialZ_Model_valueChanged(int value)
{
    dial_ChangeModelRotation();
    ui->doubleSpinBox_rotationZ_Model->setValue(value);
}

void MainWindow::on_doubleSpinBox_rotationX_Model_valueChanged(double value)
{
    text_ChangeModelRotation();
    ui->RotationDialX_Model->setValue(value);
}

void MainWindow::on_doubleSpinBox_rotationY_Model_valueChanged(double value)
{
    text_ChangeModelRotation();
    ui->RotationDialY_Model->setValue(value);
}

void MainWindow::on_doubleSpinBox_rotationZ_Model_valueChanged(double value)
{
    text_ChangeModelRotation();
    ui->RotationDialZ_Model->setValue(value);
}

void MainWindow::on_horizontalSlider_Scale_Model_valueChanged(int value)
{
    ui->mainView->getMainEntity()->getModel()->setScale(value);
    ui->doubleSpinBox_Scale_Model->setValue(value);
}

void MainWindow::on_doubleSpinBox_Scale_Model_valueChanged(double value)
{
    ui->mainView->getMainEntity()->getModel()->setScale(value);
    ui->horizontalSlider_Scale_Model->setValue(static_cast<int>(value));
}

void MainWindow::on_comboBox_Model_activated(QString const &value)
{

}

void MainWindow::on_doubleSpinBox_offsetX_Model_valueChanged(double arg1)
{
    text_ChangeModelOffset();
}

void MainWindow::on_doubleSpinBox_offsetY_Model_valueChanged(double arg1)
{
    text_ChangeModelOffset();
}

void MainWindow::on_doubleSpinBox_offsetZ_Model_valueChanged(double arg1)
{
    text_ChangeModelOffset();
}
