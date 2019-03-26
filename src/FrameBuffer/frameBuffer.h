#ifndef GAME_FRAMEBUFFER_H
#define GAME_FRAMEBUFFER_H

#include "../platformSpecificIncludes.ih"

#include <QOpenGLFunctions_3_3_Core>
#include <string>

enum class FrameBufferType
{
    REFLECTION, REFRACTION
};

class FrameBuffer
{
    private:
        QOpenGLFunctions_3_3_Core *d_gl = nullptr;
        unsigned int d_frameBuffer = 0;
        unsigned int d_texture = 0;
        unsigned int d_depthTexture = 0;
        unsigned int d_depthBuffer = 0;
        unsigned int d_width = 0;
        unsigned int d_height = 0;
        float d_quality = 0.7; // The quality of the buffer. 0.5 means rendered at half dimensions (quarter resolution).

        FrameBufferType d_type;

    public:
        FrameBuffer() = delete;

        FrameBuffer(QOpenGLFunctions_3_3_Core *gl, FrameBufferType type, unsigned int width, unsigned int height, float quality);

        void populateFBO();

        void createFrameBuffer();
        void createTextureAttachment();
        void createDepthTextureAttachment();
        void createDepthBufferAttachment();
        void bindFrameBuffer();
        void clearBuffer();

        void printData();

        void updateDims(float width, float height);

        ~FrameBuffer();

        bool isComplete() const;
        std::string getStatus() const;
        unsigned int getFrameBufferID() const;

        unsigned int getTexture() const;
        unsigned int getDepthTexture() const;

        float getQuality() const;
        void setQuality(float quality);
};


#endif
