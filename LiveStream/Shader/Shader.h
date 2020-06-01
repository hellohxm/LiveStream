#ifndef SHADER_H
#define SHADER_H

#include <QObject>
#include <QOpenGLFunctions_4_0_Core>
#include <QFile>

class CShader : protected QOpenGLFunctions_4_0_Core
{
public:
    CShader();
    ~CShader();

public:
    unsigned int loadShaderProgram(const QString& strVertexFilePath, const QString& strFragmentFilePath);
protected:
    unsigned int loadShader(const QString& strFilePath,GLenum type);

private:
    bool m_bInit;
};

#endif // CTRIANGLEOPENGL_H
