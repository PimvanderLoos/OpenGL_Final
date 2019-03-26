#include "mainview.ih"

void (MainView::*MainView::s_shadingmode[])(QMatrix4x4 const &PMV, QMatrix4x4 const &modelTransform,
                                            QMatrix3x3 const &normalTransform, Entity const *entity,
                                            QVector4D  const &clipPlane, QVector3D  const &camPos) =
{
    &MainView::paintGL_Phong,
    &MainView::paintGL_Normal,
    &MainView::paintGL_Gouraud,
};

void (MainView::*MainView::s_shadingloader[])(bool isLoaded) =
{
    &MainView::load_Phong,
    &MainView::load_Normal,
    &MainView::load_Gouraud,
};