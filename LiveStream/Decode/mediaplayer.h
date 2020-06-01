#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

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

class MediaPlayer : public QObject
{
    Q_OBJECT
public:
    explicit MediaPlayer(QObject *parent = 0);

    QString FileName;

public:
    PicData getPicData();
signals:
    void SendImage(QImage img);
    void CloseVideo();
    void refresh();
    void sendVideodata(QString);
    void sendNowData(QString);
public slots:
    void init();
    void play();
    void setFileName(QString name);
private:
    QList<PicData> m_listPic;
    std::mutex m_mutex;
};

#endif // MEDIAPLAYER_H
