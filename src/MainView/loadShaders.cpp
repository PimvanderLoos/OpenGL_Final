#include "mainview.ih"

// Load / unload phong shader.
void MainView::load_Phong(bool isLoaded)
{
    if (!isLoaded)
        d_phongShader->bind();
    else
        d_phongShader->release();
}

// Load / unload normal shader.
void MainView::load_Normal(bool isLoaded)
{
    if (!isLoaded)
        d_normalShader->bind();
    else
        d_normalShader->release();
}

// Load / unload gouraud shader.
void MainView::load_Gouraud(bool isLoaded)
{
    if (!isLoaded)
    {
        d_gouraudShader->bind();
    }
    else
        d_gouraudShader->release();
}