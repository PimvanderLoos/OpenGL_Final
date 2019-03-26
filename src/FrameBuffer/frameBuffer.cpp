#include "frameBuffer.h"

#include <QOpenGLFunctions>
#include <QDebug>

using namespace std;

FrameBuffer::FrameBuffer(QOpenGLFunctions_3_3_Core *gl, FrameBufferType type, unsigned int width,
                         unsigned int height, float quality)
:
    d_gl(gl),
    d_quality(quality),
    d_type(type)
{
    d_gl->glGenFramebuffers(1, &d_frameBuffer);
    d_gl->glGenTextures(1, &d_texture);
    if (d_type == FrameBufferType::REFLECTION)
        d_gl->glGenRenderbuffers(1, &d_depthBuffer);
    else if (d_type == FrameBufferType::REFRACTION)
        d_gl->glGenTextures(1, &d_depthTexture);

    updateDims(width, height);
}

void FrameBuffer::updateDims(float width, float height)
{
    d_width  = static_cast<unsigned int>(width  * d_quality);
    d_height = static_cast<unsigned int>(height * d_quality);
    populateFBO();
}


void FrameBuffer::populateFBO()
{
    createFrameBuffer();
    createTextureAttachment();
    if (d_type == FrameBufferType::REFLECTION)
        createDepthBufferAttachment();
    else if (d_type == FrameBufferType::REFRACTION)
        createDepthTextureAttachment();
}

void FrameBuffer::createFrameBuffer()
{
    d_gl->glBindFramebuffer(GL_FRAMEBUFFER, d_frameBuffer);
    d_gl->glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

void FrameBuffer::createTextureAttachment()
{
    d_gl->glBindTexture(GL_TEXTURE_2D, d_texture);

    d_gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, d_width, d_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    d_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    d_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    d_gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d_texture, 0);
}

void FrameBuffer::setQuality(float quality)
{
    d_quality = quality;
}

float FrameBuffer::getQuality() const
{
    return d_quality;
}

void FrameBuffer::createDepthTextureAttachment()
{
    d_gl->glBindTexture(GL_TEXTURE_2D, d_depthTexture);

    d_gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, d_width, d_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    d_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    d_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    d_gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, d_depthTexture, 0);
}

void FrameBuffer::createDepthBufferAttachment()
{
    d_gl->glBindRenderbuffer(GL_RENDERBUFFER, d_depthBuffer);

    d_gl->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, d_width, d_height);
    d_gl->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, d_depthBuffer);
}

void FrameBuffer::bindFrameBuffer()
{
    d_gl->glBindTexture(GL_TEXTURE_2D, 0);
    d_gl->glBindFramebuffer(GL_FRAMEBUFFER, d_frameBuffer);
    d_gl->glViewport(0, 0, d_width, d_height);
}

void FrameBuffer::clearBuffer()
{
    d_gl->glClearColor(0.5f, 0.7f, 1.0f, 0.0f);
    d_gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameBuffer::printData()
{
    qDebug() << "framebuffer:" << d_frameBuffer << "texture:" << d_texture << "depthtexture:" << d_depthTexture
             << "depthbuffer:" << d_depthBuffer << "width:" << d_width << "height:" << d_height
             << "quality:" << d_quality;
}

FrameBuffer::~FrameBuffer()
{
    d_gl->glDeleteFramebuffers(1, &d_frameBuffer);
    d_gl->glDeleteTextures(1, &d_texture);

    if (d_type == FrameBufferType::REFLECTION)
        d_gl->glDeleteRenderbuffers(1, &d_depthBuffer);
    else if (d_type == FrameBufferType::REFRACTION)
        d_gl->glDeleteTextures(1, &d_depthTexture);
}

bool FrameBuffer::isComplete() const
{
    return d_gl->glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

string FrameBuffer::getStatus() const
{
    return to_string(d_gl->glCheckFramebufferStatus(GL_FRAMEBUFFER));
}

unsigned int FrameBuffer::getFrameBufferID() const
{
    return d_frameBuffer;
}

unsigned int FrameBuffer::getTexture() const
{
    return d_texture;
}

unsigned int FrameBuffer::getDepthTexture() const
{
    return d_depthTexture;
}