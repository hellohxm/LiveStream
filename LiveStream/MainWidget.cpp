#include "MainWidget.h"
#include "ui_MainWidget.h"
#include <QFileDialog>
CMainWidget::CMainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CMainWidget)
{
    ui->setupUi(this);
    m_pDecode =new CVideoDecode;
    m_pThreadDecode= new QThread;
    m_pDecode->moveToThread(m_pThreadDecode);
    m_pThreadDecode->start();

    m_pThreadPushStream = new QThread;
    m_pPushStream = new CPushStream;
    m_pPushStream->moveToThread(m_pThreadPushStream);
    m_pThreadPushStream->start();
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(onTimerSetData()));
    connect(ui->m_pBtnPlay,&QPushButton::clicked,this,&CMainWidget::onPlayVideo);
    connect(this,&CMainWidget::signalPlayVideo,m_pDecode,&CVideoDecode::play);
    //connect(this,&CMainWidget::signalPlayVideo,m_pRtpDecode,&CRtpDecode::decoder);
    m_timer.start(40);

}

CMainWidget::~CMainWidget()
{
    if(nullptr!=m_pDecode)
    {
        delete m_pDecode;
        m_pDecode=nullptr;
    }
    if(nullptr !=m_pThreadDecode)
    {
        delete m_pThreadDecode;
        m_pThreadDecode = nullptr;
    }
    delete ui;
}

void CMainWidget::onTimerSetData()
{
    if(nullptr!=m_pDecode)
    {
        ui->m_pWidgetVideo->setPicData(m_pDecode->getPicData());
    }
}
void CMainWidget::onPlayVideo()
{
    //QString strPath =ui->lineEdit->text();//"rtmp://58.200.131.2:1935/livetv/hunantv";//QFileDialog::getOpenFileName(this, tr("Select a image file"), QApplication::applicationDirPath(), tr("Image Files (*.*)"));
    QString strPath = "D:/Titanic.ts";
    m_pDecode->setFileName(strPath);
    emit signalPlayVideo(strPath);
    //m_pPushStream->init();
}

