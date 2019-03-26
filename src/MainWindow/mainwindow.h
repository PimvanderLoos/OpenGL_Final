#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../MainView/mainview.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::MainWindow *ui;

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private slots:
        void on_mainEntityChanged();
        void on_mainModelChanged();

        void changeModelEnabled(bool status);
        void changeEntityEnabled(bool status);

        void dial_ChangeEntityRotation();
        void text_ChangeEntityRotation();
        void updateMainEntityPosition();

        void dial_ChangeModelRotation();
        void text_ChangeModelRotation();
        void text_ChangeModelOffset();

        void on_ResetScaleButton_clicked(bool checked);
        void on_ScaleSlider_valueChanged(int value);

        void on_PhongButton_toggled(bool checked);
        void on_NormalButton_toggled(bool checked);
        void on_GouraudButton_toggled(bool checked);

        void recalc_ScaleSlider();
        void recalc_FPS();
        void recalc_maxFrameTime();
        void recalc_avgFrameTime();
        void on_checkBox_stateChanged(int arg1);
        void on_comboBox_Model_activated(QString const &arg1);
        void recalc_modelList();
        void on_button_next_entity_clicked();
        void on_button_previous_entity_clicked();
        void on_button_free_cam_clicked();
        void on_button_new_entity_clicked();
        void on_button_dupe_entity_clicked();
        void on_button_kill_entity_clicked();
        void on_button_hold_cam_clicked();
        void on_RotationDialX_Entity_valueChanged(int value);
        void on_RotationDialY_Entity_valueChanged(int value);
        void on_RotationDialZ_Entity_valueChanged(int value);
        void on_pushButton_Reset_Rotation_Entity_clicked(bool checked);
        void on_horizontalSlider_Scale_Entity_valueChanged(int value);
        void on_lineEdit_Console_returnPressed();
        void on_doubleSpinBox_rotationX_Entity_valueChanged(double arg1);
        void on_doubleSpinBox_rotationY_Entity_valueChanged(double arg1);
        void on_doubleSpinBox_rotationZ_Entity_valueChanged(double arg1);
        void on_doubleSpinBox_Scale_Entity_valueChanged(double arg1);
        void on_doubleSpinBox_positionX_Entity_valueChanged(double arg1);
        void on_doubleSpinBox_positionY_Entity_valueChanged(double arg1);
        void on_doubleSpinBox_positionZ_Entity_valueChanged(double arg1);
        void on_RotationDialX_Model_valueChanged(int value);
        void on_RotationDialY_Model_valueChanged(int value);
        void on_RotationDialZ_Model_valueChanged(int value);
        void on_doubleSpinBox_rotationX_Model_valueChanged(double arg1);
        void on_doubleSpinBox_rotationY_Model_valueChanged(double arg1);
        void on_doubleSpinBox_rotationZ_Model_valueChanged(double arg1);
        void on_horizontalSlider_Scale_Model_valueChanged(int value);
        void on_doubleSpinBox_Scale_Model_valueChanged(double arg1);
        void on_comboBox_Entity_Model_activated(const QString &arg1);
        void on_doubleSpinBox_offsetX_Model_valueChanged(double arg1);
        void on_doubleSpinBox_offsetY_Model_valueChanged(double arg1);
        void on_doubleSpinBox_offsetZ_Model_valueChanged(double arg1);
};

#endif // MAINWINDOW_H
