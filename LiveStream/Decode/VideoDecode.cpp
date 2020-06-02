#include "VideoDecode.h"
#define flag_play       1
#define flag_pause      2
#define flag_break      3
#define flag_refresh    4
int videoindex;
AVFormatContext *pFormatCtxVideo;
AVCodecContext *pCodecCtxVideo;
static int audio_time;
int event_flag=0;  //0=无任务，1=继续播放，2=暂停，3=关闭正在播放的视频，4=播放新视频
CVideoDecode::CVideoDecode(QObject *parent) : QObject(parent)
{

}

void CVideoDecode::init()
{
    event_flag = flag_play;
    AVCodec *pCodecVideo;
    avcodec_register_all();
    av_register_all();
    pFormatCtxVideo = avformat_alloc_context();
    if(avformat_open_input(&pFormatCtxVideo, FileName.toLatin1().constData(), NULL, NULL) != 0)
    {
        qDebug() << "Open Video Fail";
    }
    if(avformat_find_stream_info(pFormatCtxVideo, NULL) < 0)
    {
        qDebug() << "FindVideo Fail";
    }
    videoindex = -1;
    for(int i = 0; pFormatCtxVideo->nb_streams; i++)
    {
        if(pFormatCtxVideo->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoindex = i;
            break;
        }
    }
    if(videoindex == -1)
        qDebug() << "Din't find video stream";
    pCodecCtxVideo = pFormatCtxVideo->streams[videoindex]->codec;
    AVCodecContext *pCodecCtxVideo2 = pCodecCtxVideo;
    pCodecVideo = avcodec_find_decoder(pCodecCtxVideo->codec_id);
    if(pCodecVideo == NULL)
    {
        qDebug() << "codec not find";
    }
    if(avcodec_open2(pCodecCtxVideo, pCodecVideo, NULL) < 0)
    {
        qDebug() << "can't open codec";
    }
}

void CVideoDecode::play()
{
    AVFrame *pFrameVideo,*pFrameRGB;
    SwsContext *convertCtx;
    AVPacket *packetVideo;
    int got_picture;

    pFrameVideo = av_frame_alloc();
    pFrameRGB = av_frame_alloc();

    QString str=QString("%1:%2").arg(pFormatCtxVideo->duration/60000000).arg(pFormatCtxVideo->duration/1000000);

    emit sendVideodata(str);
    int size = pCodecCtxVideo->width*pCodecCtxVideo->height;
    packetVideo=(AVPacket *)malloc(sizeof(AVPacket));
    av_new_packet(packetVideo, size);
    uint8_t *buffer;
    buffer = new uint8_t[avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtxVideo->width, pCodecCtxVideo->height)];
    avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_YUV420P, pCodecCtxVideo->width, pCodecCtxVideo->height);
    convertCtx = sws_getContext(pCodecCtxVideo->width,pCodecCtxVideo->height,
                                            pCodecCtxVideo->pix_fmt,
                                            pCodecCtxVideo->width,
                                            pCodecCtxVideo->height,
                                            AV_PIX_FMT_YUV420P,
                                            SWS_BICUBIC,
                                            NULL, NULL, NULL);
    while(1)
    {
        if(event_flag == flag_play)
        {
            if(av_read_frame(pFormatCtxVideo,packetVideo) >= 0)
            {
                if(packetVideo->stream_index == videoindex)
                {
                    int rec = avcodec_decode_video2(pCodecCtxVideo, pFrameVideo, &got_picture, packetVideo);
                    _sleep(20);
                    if(rec > 0)
                    {
                        sws_scale(convertCtx, (const uint8_t* const*)pFrameVideo->data, pFrameVideo->linesize,
                                  0, pCodecCtxVideo->height, pFrameRGB->data, pFrameRGB->linesize);
                        PicData struPicData;
                        struPicData.pRGBData = (uchar *)pFrameRGB->data[0];
                        struPicData.yuvData[0] = (uchar *)pFrameRGB->data[0];
                        struPicData.yuvData[1] = (uchar *)pFrameRGB->data[1];
                        struPicData.yuvData[2] = (uchar *)pFrameRGB->data[2];
                        struPicData.iWidth = pCodecCtxVideo->width;
                        struPicData.iHeight = pCodecCtxVideo->height;
                        m_listPic.append(struPicData);
                        if(m_listPic.size()>200)
                        {
                            m_listPic.removeFirst();
                        }
                    }
                }
                av_free_packet(packetVideo);
            }
            else
            {
                event_flag=0;
                emit CloseVideo();
                break;
            }
        }
        else if(event_flag == flag_refresh)
        {
            emit refresh();
            break;
        }

    }
    av_frame_free(&pFrameRGB);   //AVFrame的销毁函数
    av_frame_free(&pFrameVideo);
    avcodec_close(pCodecCtxVideo);   //关闭解码器
    avformat_close_input(&pFormatCtxVideo);  //关闭视频输入文件
    audio_time=0;
    QThread::msleep(100);
}

void CVideoDecode::setFileName(QString name)
{
    FileName = name;
}

PicData CVideoDecode::getPicData()
{
    if(m_listPic.size()<=0)
    {
        return PicData();
    }
    PicData struPicData = m_listPic.at(0);
    m_mutex.lock();
    m_listPic.removeFirst();
    m_mutex.unlock();
    return struPicData;
}
