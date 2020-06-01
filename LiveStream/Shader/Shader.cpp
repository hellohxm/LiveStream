#include "Shader.h"
#include <QDebug>


CShader::CShader()
    :m_bInit(false)
{
}
CShader::~CShader()
{

}

unsigned int CShader::loadShaderProgram(const QString& strVertexFilePath, const QString& strFragmentFilePath)
{
    if(!m_bInit)
    {
        if(initializeOpenGLFunctions())
        {
            m_bInit = true;
        }
    }

    unsigned int vertexShader=loadShader(strVertexFilePath,GL_VERTEX_SHADER);
    unsigned int fragmentShader=loadShader(strFragmentFilePath,GL_FRAGMENT_SHADER);
    if(vertexShader<=0 ||fragmentShader<=0)
    {
        return 0;
    }
    //链接着色器程序

   unsigned int uShaderProgram = glCreateProgram();//创建程序
    glAttachShader(uShaderProgram,vertexShader);//附加顶点着色器
    glAttachShader(uShaderProgram,fragmentShader);//附加片段着色器
    glLinkProgram(uShaderProgram);//链接程序
    int success;
    char infoLog[512];
    glGetProgramiv(uShaderProgram,GL_LINK_STATUS,&success);
    if(!success)
    {
        glGetProgramInfoLog(uShaderProgram,512,NULL,infoLog);
        qDebug()<<"ERROR::Program::Link error\n" << infoLog;
        return 0;
    }
    //glUseProgram(m_shaderProgram);//激活程序对象
    glDeleteShader(vertexShader);//着色器对象链接到程序对象后，就可以删除着色器对象了
    glDeleteShader(fragmentShader);
    return uShaderProgram;
}
unsigned int CShader::loadShader(const QString& strFilePath, GLenum type)
{
    if(strFilePath.isEmpty())
    {
        return 0;
    }
    QFile flShader(strFilePath);
    if(!flShader.open(QIODevice::ReadOnly))
    {
        return 0;
    }
    QByteArray byte = flShader.readAll();//一定要分开写，一起写会无效
    char* cShaderSource = byte.data();
    flShader.close();
    //编译着色器
    unsigned int uShader;
    uShader = glCreateShader(type);
    //uShader = glCreateShader(type);//创建顶点着色器
    glShaderSource(uShader,1,&cShaderSource,NULL);//把这个着色器源码附加到着色器对象上
    glCompileShader(uShader);//编译着色器
    int success;
    char infoLog[512];
    glGetShaderiv(uShader,GL_COMPILE_STATUS,&success);
    if(!success)//判断是否编译成功
    {
        glGetShaderInfoLog(uShader,512,NULL,infoLog);
        qDebug()<<"ERROR::SHADER::COMPILATION_FAILED\n" << infoLog;
        return 0;
    }
    return uShader;
}
