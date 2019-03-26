#include "mainview.ih"

void MainView::createShaderProgram()
{
    d_normalShader  = new MyShaderProgram_Normal  (this, ":/src/shaders/fragshader_normal.glsl",
                                                         ":/src/shaders/vertshader_normal.glsl");

    d_gouraudShader = new MyShaderProgram_Gouraud (this, ":/src/shaders/fragshader_gouraud.glsl",
                                                         ":/src/shaders/vertshader_gouraud.glsl");

    d_phongShader   = new MyShaderProgram_Phong   (this, ":/src/shaders/fragshader_phong.glsl",
                                                         ":/src/shaders/vertshader_phong.glsl");

    d_splashShader  = new MyShaderProgram_Splash  (this, ":/src/shaders/fragshader_splash.glsl",
                                                         ":/src/shaders/vertshader_splash.glsl");

    d_waterShader   = new MyShaderProgram_Water   (this, ":/src/shaders/fragshader_water.glsl",
                                                         ":/src/shaders/vertshader_water.glsl");

    d_aabbShader    = new MyShaderProgram_AABB    (this, ":/src/shaders/fragshader_AABB.glsl",
                                                         ":/src/shaders/vertshader_AABB.glsl");

    d_cubemapShader = new MyShaderProgram_Cubemap (this, ":/src/shaders/fragshader_cubemap.glsl",
                                                         ":/src/shaders/vertshader_cubemap.glsl");
}