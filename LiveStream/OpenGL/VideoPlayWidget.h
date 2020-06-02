#ifndef CVIDEOPLAYWIDGET_H
#define CVIDEOPLAYWIDGET_H

#include <QOpenGLWidget>
#include <QWidget>
#include "Shader/Shader.h"
#include <QOpenGLFunctions_4_0_Core>
#include "Decode/VideoDecode.h"
#include <QFile>
#include <QBuffer>
//#define STB_IMAGE_IMPLEMENTATION 1
#include "Shader/stb_image.h"

#include <iostream>
#include <stdio.h>
class CVideoPlayWidget : public QOpenGLWidget,protected QOpenGLFunctions_4_0_Core
{
    Q_OBJECT
public:
    CVideoPlayWidget(QWidget* pWidget = nullptr);
    ~CVideoPlayWidget();
public:
    void setPicData(const PicData& struPicData);
protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    void paintEvent(QPaintEvent *e) override;
private:
    CShader m_shader;
    unsigned int m_shaderProgram;//着色器程序
    unsigned int m_VBO;//顶点缓冲对象
    unsigned int m_VAO;//顶点数组对象
    unsigned int m_EBO;//索
    unsigned int m_textureY;
    unsigned int m_textureU;
    unsigned int m_textureV;
    PicData m_PicData;
    FILE* fp_yuv;
};

#endif // CVIDEOPLAYWIDGET_H
