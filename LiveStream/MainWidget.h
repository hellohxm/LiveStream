#ifndef CMAINWIDGET_H
#define CMAINWIDGET_H

#include <QWidget>
#include "Decode/VideoDecode.h"
#include <QThread>
#include <QTimer>
#include "Decode/PushStream.h"
QT_BEGIN_NAMESPACE
namespace Ui { class CMainWidget; }
QT_END_NAMESPACE

class CMainWidget : public QWidget
{
    Q_OBJECT

public:
    CMainWidget(QWidget *parent = nullptr);
    ~CMainWidget();

signals:
    void signalPlayVideo(const QString&);
private slots:
    void onTimerSetData();
    void onPlayVideo();
private:
    Ui::CMainWidget *ui;
    CVideoDecode* m_pDecode{nullptr};
    QThread* m_pThreadDecode{nullptr};
    QThread* m_pThreadPushStream{nullptr};
    QTimer m_timer;
    CPushStream* m_pPushStream{nullptr};
};
#endif // CMAINWIDGET_H
