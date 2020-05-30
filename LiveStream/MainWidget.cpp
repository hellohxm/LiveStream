#include "MainWidget.h"
#include "ui_MainWidget.h"

CMainWidget::CMainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CMainWidget)
{
    ui->setupUi(this);
}

CMainWidget::~CMainWidget()
{
    delete ui;
}

