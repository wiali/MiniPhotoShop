﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    state=STATE::INIT;
    pen=NULL;
    brush=NULL;
    displayImage=NULL;
    ui->setupUi(this);
    ui->MainDisplayWidget->SetState(&state);

    connect(ui->menuBar,SIGNAL(triggered(QAction*)),this,SLOT(menuTriggered(QAction*)));

    ui->action_drawLine->setCheckable(true);
    ui->action_drawEllipse->setCheckable(true);
    connect(ui->penWidthSlider,SIGNAL(valueChanged(int)),this,SLOT(PenWidthSliderChanged(int)));
    connect(ui->MainDisplayWidget,SIGNAL(StateChanged()),this,SLOT(StateChanged()));
    connect(ui->foreColorButton,SIGNAL(pressed()),this,SLOT(ButtonForeColorPressed()));
    connect(ui->backColorButton,SIGNAL(pressed()),this,SLOT(ButtonBackColorPressed()));
    connect(ui->penStyleComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(PenStyleComboBoxChanged(int)));
    ui->penStyleComboBox->insertItem(0,QString(QStringLiteral("实线")),QString("solid"));
    ui->penStyleComboBox->insertItem(1,QString(QStringLiteral("虚线")),QString("dash"));
    ui->penStyleComboBox->insertItem(2,QString(QStringLiteral("点线")),QString("dash_dot"));
    ui->penStyleComboBox->insertItem(3,QString(QStringLiteral("点划线")),QString("dash_dot_dot"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setAddLineCommand(const shared_ptr<BaseCommand> &addLineCommand)
{
    ui->MainDisplayWidget->setAddLineCommand(addLineCommand);
}
void MainWindow::setAddEllipseCommand(const shared_ptr<BaseCommand> &addEllipseCommand){
    ui->MainDisplayWidget->setAddEllipseCommand(addEllipseCommand);

}

void MainWindow::setPenUpdateCommand(const shared_ptr<BaseCommand> &penUpdateCommand)
{
    this->penUpdateCommand=penUpdateCommand;
}

void MainWindow::update(Params params)
{

}

void MainWindow::SetPen(const Pen* pen)
{
    this->pen=pen;
    ui->foreColorButton->setStyleSheet(QString("background-color: rgb(%1, %2, %3);").arg(pen->getForeR()).arg(pen->getForeG()).arg(pen->getForeB()));
    ui->penWidthSlider->setValue(pen->getLineWidth());
    ui->MainDisplayWidget->SetPen(pen);
}

void MainWindow::SetBrush(const Brush* brush)
{
    this->brush=brush;
    ui->backColorButton->setStyleSheet(QString("background-color: rgb(%1, %2, %3);").arg(brush->getBackR()).arg(brush->getBackG()).arg(brush->getBackB()));
    ui->MainDisplayWidget->SetBrush(brush);
}

void MainWindow::SetDisplayImage(const QImage* displayImage)
{
    this->displayImage=displayImage;
    ui->MainDisplayWidget->SetImage(displayImage);
}

void MainWindow::setNewCanvasCommand(const shared_ptr<BaseCommand> &newCanvasCommand)
{
    this->newCanvasCommand=newCanvasCommand;
    ui->MainDisplayWidget->setNewCanvasCommand(newCanvasCommand);
    Params params;
    params.setInts({ui->MainDisplayWidget->getRealWidth(),ui->MainDisplayWidget->getRealHeight()});
    newCanvasCommand->setParams(params);
    newCanvasCommand->exec();
}

void MainWindow::menuTriggered(QAction* action)
{
    if(action->text()==ui->action_aboutQt->text())
    {
        QMessageBox::aboutQt(NULL);
        return;
    }
    if(action->text()==ui->action_drawLine->text())
    {
        if(state==STATE::INIT)
        {
            state=STATE::DRAW_LINE_INIT;
            StateChanged();
        }
        return;
    }
    if(action->text()==ui->action_drawEllipse->text())
    {
        if(state==STATE::INIT)
        {
            state=STATE::DRAW_ELLIPSE_INIT;
            StateChanged();
        }
        return;
    }

}

void MainWindow::StateChanged()
{
    ui->action_drawLine->setChecked(false);
    ui->action_drawEllipse->setChecked(false);
    switch(state)
    {
    case STATE::INIT:

        break;
    case STATE::DRAW_LINE_INIT:
        ui->action_drawLine->setChecked(true);

        break;

    case STATE::DRAW_ELLIPSE_INIT:

        ui->action_drawEllipse->setChecked(true);
        break;

    }
    QWidget::update();
}

void MainWindow::ButtonForeColorPressed()
{
    const QColor& color = QColorDialog::getColor(QColor(pen->getForeR(),pen->getForeG(),pen->getForeB()),this,"设置前景色");
    if(color.isValid())
    {
        ui->foreColorButton->setStyleSheet(QString("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
        Params params;
        params.setType(COMMAND::UPDATE_PEN_COLOR);
        params.setInts({color.red(),color.green(),color.blue()});
        penUpdateCommand->setParams(params);
        penUpdateCommand->exec();
    }
}

void MainWindow::ButtonBackColorPressed()
{
    const QColor& color = QColorDialog::getColor(QColor(brush->getBackR(),brush->getBackG(),brush->getBackB()),this,"设置背景色");
    if(color.isValid())
    {
        qDebug()<<"pressed";
        ui->backColorButton->setStyleSheet(QString("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
        Params params;
        params.setType(COMMAND::UPDATE_BRUSH_COLOR);
        params.setInts({color.red(),color.green(),color.blue()});
        brushUpdateCommand->setParams(params);
        brushUpdateCommand->exec();
    }
}

void MainWindow::PenWidthSliderChanged(int value)
{
    if(penUpdateCommand!=nullptr)
    {
        Params params;
        params.setType(COMMAND::UPDATE_PEN_WIDTH);
        params.setInts({value});
        penUpdateCommand->setParams(params);
        penUpdateCommand->exec();
    }

}

void MainWindow::PenStyleComboBoxChanged(int index)
{
    if(penUpdateCommand!=nullptr)
    {
        Params params;
        params.setType(COMMAND::UPDATE_PEN_STYLE);
        params.setInts({index+1});
        penUpdateCommand->setParams(params);
        penUpdateCommand->exec();

    }
}
