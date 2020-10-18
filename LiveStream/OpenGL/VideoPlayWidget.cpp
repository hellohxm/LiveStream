#include "VideoPlayWidget.h"
#include <QApplication>
#include <QPainter>
#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4
float vertices[] = {//Flip up or down, otherwise there is a problem, but no cause has been found......
                    //......................
    // positions         // texture coords 做了上下翻转，不然显示有问题，还没找到原因
     1.0f,  1.0f, 0.0f,  1.0f, 0.0f, // top right
     1.0f, -1.0f, 0.0f,  1.0f, 1.0f, // bottom right
    -1.0f, -1.0f, 0.0f,  0.0f, 1.0f, // bottom left
    -1.0f,  1.0f, 0.0f,  0.0f, 0.0f  // top left
    };

const GLfloat vertexVertices[] = {
        1.0f, 1.0f,
        1.0f, -1.0f,
        -1.0f,  -1.0f,
        -1.0f,  1.0f,
    };

const GLfloat textureVertices[] = {
        0.0f,  1.0f,
        1.0f,  1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,
    };
unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};
CVideoPlayWidget::CVideoPlayWidget(QWidget* pWidget)
    :QOpenGLWidget(pWidget)
{
    setMouseTracking(true);
    //setAttribute(Qt::WA_AlwaysStackOnTop);
}

CVideoPlayWidget::~CVideoPlayWidget()
{
    glDeleteProgram(m_shaderProgram);
    glDeleteVertexArrays(1,&m_VAO);
    glDeleteBuffers(1,&m_VBO);
}
void CVideoPlayWidget::initializeGL()
{
    initializeOpenGLFunctions();
    QString strPath = QApplication::applicationDirPath();
    m_shaderProgram = m_shader.loadShaderProgram(strPath+"/shader.vert",strPath+"/shader.frag");
    if(m_shaderProgram<=0)
    {
        return;
    }
    glGenVertexArrays(1,&m_VAO);
    glGenBuffers(1,&m_VBO);//用来生成缓冲区对象的名称
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);//把之前定义的顶

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    //glBindAttribLocation(m_shaderProgram, 0, "vertexIn");
    //glBindAttribLocation(m_shaderProgram, 1, "textureIn");

    glGenTextures(1,&m_textureY);
    glBindTexture(GL_TEXTURE_2D,m_textureY);//绑定纹理
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1,&m_textureU);
    glBindTexture(GL_TEXTURE_2D,m_textureU);//绑定纹理
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1,&m_textureV);
    glBindTexture(GL_TEXTURE_2D,m_textureV);//绑定纹理
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}

void CVideoPlayWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w,h);
}

void CVideoPlayWidget::paintGL()
{
    static int i=0;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(m_PicData.iWidth>0 && m_PicData.iHeight>0)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureY);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_PicData.iWidth,m_PicData.iHeight, 0, GL_RED, GL_UNSIGNED_BYTE, m_PicData.yuvData[0]);
        glUniform1i(glGetUniformLocation(m_shaderProgram, "tex_y"), 0);
        //U
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_textureU);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_PicData.iWidth/2,m_PicData.iHeight/2, 0, GL_RED, GL_UNSIGNED_BYTE, m_PicData.yuvData[1]);
        glUniform1i(glGetUniformLocation(m_shaderProgram, "tex_u"), 1);
        //V
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_textureV);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_PicData.iWidth/2,m_PicData.iHeight/2, 0, GL_RED, GL_UNSIGNED_BYTE, m_PicData.yuvData[2]);
        glUniform1i(glGetUniformLocation(m_shaderProgram, "tex_v"), 2);

        glUseProgram(m_shaderProgram);//激活程序对象置
        //glBindVertexArray(m_VAO);
        glDrawArrays(GL_POLYGON,0,4);
    }

}
void CVideoPlayWidget::paintEvent(QPaintEvent *e)
{
    QOpenGLWidget::paintEvent(e);
}
void CVideoPlayWidget::setPicData(const PicData& struPicData)
{
    if(struPicData.iWidth<=0 || struPicData.iHeight<=0) return;
    m_PicData=struPicData;
    update();
}
