#include "VideoDecode.h"
#include <thread>
#define flag_play       1
#define flag_pause      2
#define flag_break      3
#define flag_refresh    4
static int audio_time;
int event_flag=0;  //0=无任务，1=继续播放，2=暂停，3=关闭正在播放的视频，4=播放新视频
CVideoDecode::CVideoDecode(QObject *parent) : QObject(parent)
{

}

bool CVideoDecode::init()
{
    event_flag = flag_play;

    //注册组件
    avcodec_register_all();
    av_register_all();
    if(initVideo() && initAudio())
    {
        return true;
    }
    return false;
}

bool CVideoDecode::initVideo()
{
    AVCodec *pCodecVideo;
    if(avformat_open_input(&m_pFormatCtxVideo, m_strFileName.toLatin1().constData(), NULL, NULL) != 0)//打开输入视频文件
    {
        qDebug() << "Open Video Fail";
        return false;
    }
    if(avformat_find_stream_info(m_pFormatCtxVideo, NULL) < 0)//获取视频文件信息
    {
        qDebug() << "Find Video Fail";
        return false;
    }

    //记录视频流标记
    m_iVideoIndex = -1;
    for(int i = 0; m_pFormatCtxVideo->nb_streams; i++)
    {
        if(m_pFormatCtxVideo->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            m_iVideoIndex = i;
            break;
        }
    }
    if(m_iVideoIndex == -1)
    {
        qDebug() << "Don't find video stream";
        return false;
    }

    m_pCodecCtxVideo = m_pFormatCtxVideo->streams[m_iVideoIndex]->codec;
    pCodecVideo = avcodec_find_decoder(m_pCodecCtxVideo->codec_id);//查找解码器
    if(pCodecVideo == NULL)
    {
        qDebug() << "video codec not find";
        return false;
    }
    if(avcodec_open2(m_pCodecCtxVideo, pCodecVideo, NULL) < 0)//打开解码器
    {
        qDebug() << "can't open video codec";
        return false;
    }
    return true;

}
bool CVideoDecode::initAudio()
{
    AVCodec *pCodecAudio;
    if(avformat_open_input(&m_pFormatCtxAudio, m_strFileName.toLatin1().constData(), NULL, NULL) != 0)//打开输入视频文件
    {
        qDebug() << "Open Audio Fail";
        return false;
    }
    if(avformat_find_stream_info(m_pFormatCtxAudio, NULL) < 0)//获取视频文件信息
    {
        qDebug() << "FindAudio Fail";
        return false;
    }

    //记录视频流标记
    m_iAudioIndex =-1;
    for(int i = 0; m_pFormatCtxAudio->nb_streams; i++)
    {
        if(m_pFormatCtxAudio->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            m_iAudioIndex = i;
            break;
        }
    }
    if(m_iAudioIndex == -1)
    {
        qDebug() << "Don't find audio stream";
        return false;
    }

    m_pCodecCtxAudio = m_pFormatCtxAudio->streams[m_iAudioIndex]->codec;
    pCodecAudio = avcodec_find_decoder(m_pCodecCtxAudio->codec_id);//查找解码器
    if(pCodecAudio == NULL)
    {
        qDebug() << "audio codec not find";
        return false;
    }
    if(avcodec_open2(m_pCodecCtxAudio, pCodecAudio, NULL) < 0)//打开解码器
    {
        qDebug() << "can't open audio codec";
        return false;
    }
    return true;
}
void CVideoDecode::play(const QString &strPath)
{
    m_strFileName = strPath;
    if(!init())
    {
        return;
    }
    int iGotPicture;
    std::thread th(&CVideoDecode::decodeAudio,this);
    AVFrame* pFrameVideo = av_frame_alloc();
    AVFrame* pFrameRGB = av_frame_alloc();
    AVPacket* pPacketVideo=(AVPacket *)malloc(sizeof(AVPacket));
    av_new_packet(pPacketVideo, m_pCodecCtxVideo->width*m_pCodecCtxVideo->height);
    uint8_t* buffer = new uint8_t[avpicture_get_size(AV_PIX_FMT_YUV420P, m_pCodecCtxVideo->width, m_pCodecCtxVideo->height)];
    avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_YUV420P, m_pCodecCtxVideo->width, m_pCodecCtxVideo->height);
    SwsContext* pConvertCtx = sws_getContext(m_pCodecCtxVideo->width,m_pCodecCtxVideo->height,
                                            m_pCodecCtxVideo->pix_fmt,
                                            m_pCodecCtxVideo->width,
                                            m_pCodecCtxVideo->height,
                                            AV_PIX_FMT_YUV420P,
                                            SWS_BICUBIC,
                                            NULL, NULL, NULL);
    while(1)
    {
        if(event_flag == flag_play)
        {
            //从输入文件读取一帧压缩数据
            if(av_read_frame(m_pFormatCtxVideo,pPacketVideo) >= 0)
            {
                //判断码流类型
                if(pPacketVideo->stream_index == m_iVideoIndex)
                {
                    //解码一帧压缩数据
                    int rec = avcodec_decode_video2(m_pCodecCtxVideo, pFrameVideo, &iGotPicture, pPacketVideo);
                    _sleep(20);
                    if(rec > 0)
                    {
                        sws_scale(pConvertCtx, (const uint8_t* const*)pFrameVideo->data, pFrameVideo->linesize,
                                  0, m_pCodecCtxVideo->height, pFrameRGB->data, pFrameRGB->linesize);
                        PicData struPicData;
                        struPicData.pRGBData = (uchar *)pFrameRGB->data[0];
                        struPicData.yuvData[0] = (uchar *)pFrameRGB->data[0];
                        struPicData.yuvData[1] = (uchar *)pFrameRGB->data[1];
                        struPicData.yuvData[2] = (uchar *)pFrameRGB->data[2];
                        struPicData.iWidth = m_pCodecCtxVideo->width;
                        struPicData.iHeight = m_pCodecCtxVideo->height;
                        m_listPic.append(struPicData);
                        if(m_listPic.size()>200)
                        {
                            m_listPic.removeFirst();
                        }
                    }
                }
                av_free_packet(pPacketVideo);
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
    avcodec_close(m_pCodecCtxVideo);   //关闭解码器
    avformat_close_input(&m_pFormatCtxVideo);  //关闭视频输入文件
    audio_time=0;
    QThread::msleep(100);
}

void CVideoDecode::decodeAudio()
{
    AVPacket* pPacketAudio=(AVPacket *)malloc(sizeof(AVPacket));
    av_init_packet(pPacketAudio);

    //Out Audio Param
    uint64_t out_channel_layout=AV_CH_LAYOUT_STEREO;
    //nb_samples: AAC-1024 MP3-1152
    int out_nb_samples=m_pCodecCtxAudio->frame_size;
    AVSampleFormat out_sample_fmt=AV_SAMPLE_FMT_S16;
    int out_sample_rate=44100;
    int out_channels=av_get_channel_layout_nb_channels(out_channel_layout);
    //Out Buffer Size
    int out_buffer_size=av_samples_get_buffer_size(NULL,out_channels ,out_nb_samples,out_sample_fmt, 1);

    uint8_t* out_buffer=(uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE*2);
    AVFrame* pFrameAudio=av_frame_alloc();

    //FIX:Some Codec's Context Information is missing
    int64_t in_channel_layout=av_get_default_channel_layout(m_pCodecCtxAudio->channels);
    //Swr
    struct SwrContext * struSwrContext = swr_alloc();
    struSwrContext=swr_alloc_set_opts(struSwrContext,out_channel_layout, out_sample_fmt, out_sample_rate,
        in_channel_layout,m_pCodecCtxAudio->sample_fmt , m_pCodecCtxAudio->sample_rate,0, NULL);
    swr_init(struSwrContext);
    int  iGotAudio=-1;
    FILE *pFile=fopen("output.pcm", "wb");
    while(av_read_frame(m_pFormatCtxAudio, pPacketAudio)>=0){
        if(pPacketAudio->stream_index==m_iAudioIndex){

            int ret = avcodec_decode_audio4( m_pCodecCtxAudio, pFrameAudio,&iGotAudio, pPacketAudio);
            if ( ret < 0 ) {
                printf("Error in decoding audio frame.\n");
                return;
            }
            if ( iGotAudio > 0 ){
                swr_convert(struSwrContext,&out_buffer, MAX_AUDIO_FRAME_SIZE,(const uint8_t **)pFrameAudio->data , pFrameAudio->nb_samples);

                //Write PCM
                fwrite(out_buffer, 1, out_buffer_size, pFile);
            }
        }
        av_free_packet(pPacketAudio);
    }

    swr_free(&struSwrContext);

    fclose(pFile);

    av_free(out_buffer);
    // Close the codec
    avcodec_close(m_pCodecCtxAudio);
    // Close the video file
    avformat_close_input(&m_pFormatCtxAudio);
}

void CVideoDecode::setFileName(QString name)
{
    m_strFileName = name;
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
