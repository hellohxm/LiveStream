#ifndef CVideoDecode_H
#define CVideoDecode_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QImage>
#include <QList>
#include <mutex>
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
}
#undef main
#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio
struct PicData
{
    uchar* pRGBData=nullptr;
    uchar *yuvData[3]={nullptr,nullptr,nullptr};
    int iWidth=0;
    int iHeight=0;
    QString strTimeNow;
    QString strTimeTotal;
    QImage::Format enFormat = QImage::Format_RGB32;
};

struct AudioData
{

};

class CVideoDecode : public QObject
{
    Q_OBJECT
public:
    explicit CVideoDecode(QObject *parent = 0);


public:
    PicData getPicData();
signals:
    void SendImage(QImage img);
    void CloseVideo();
    void refresh();
    void sendNowData(QString);
public slots:
    void play(const QString& strPath);
    void setFileName(QString name);
private:
    bool init();
    bool initVideo();
    bool initAudio();
    void decodeAudio();

private:
    QList<PicData> m_listPic;
    std::mutex m_mutex;
    QString m_strFileName;//文件名
    AVFormatContext *m_pFormatCtxVideo{nullptr};
    AVCodecContext *m_pCodecCtxVideo{nullptr};
    AVFormatContext *m_pFormatCtxAudio{nullptr};
    AVCodecContext *m_pCodecCtxAudio{nullptr};
    int m_iVideoIndex;//视频流标记
    int m_iAudioIndex;//音频流标记
};

#endif // CVideoDecode_H
