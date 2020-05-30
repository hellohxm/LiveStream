#ifndef CMAINWIDGET_H
#define CMAINWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class CMainWidget; }
QT_END_NAMESPACE

class CMainWidget : public QWidget
{
    Q_OBJECT

public:
    CMainWidget(QWidget *parent = nullptr);
    ~CMainWidget();

private:
    Ui::CMainWidget *ui;
};
#endif // CMAINWIDGET_H
