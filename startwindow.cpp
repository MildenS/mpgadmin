#include "mainwindow.h"
#include "ui_startwindow.h"

#include "MainMenu/mainmenuwindow.h"

StartWindow::StartWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::StartWindow)
{
    ui->setupUi(this);
}

StartWindow::~StartWindow()
{
    delete ui;
}

void StartWindow::on_startButton_clicked()
{
    MainMenuWindow* menu = new MainMenuWindow;
    menu->show();
    this->close();
}
