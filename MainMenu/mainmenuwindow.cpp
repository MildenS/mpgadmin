#include "mainmenuwindow.h"
#include "ui_mainmenuwindow.h"
#include "exhibitinfodialog.h"

MainMenuWindow::MainMenuWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainMenuWindow)
{
    ui->setupUi(this);
    httpWorker = new HttpWorker;
    QJsonDocument exhibits = httpWorker->getExhibits();
    table_model = new ExhibitsTableModel(exhibits);
    ui->exhibitsTable->setModel(table_model);
    ui->exhibitsTable->resizeColumnsToContents();
    ui->exhibitsTable->resizeRowsToContents();
    //ui->exhibitsTable->verticalHeader()->setDefaultSectionSize(256);
    ui->exhibitsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->exhibitsTable->setColumnHidden(2, true);
}

MainMenuWindow::~MainMenuWindow()
{
    delete ui;
}


void MainMenuWindow::on_showExhibitButton_clicked()
{
    QModelIndex currentIndex = ui->exhibitsTable->currentIndex();
    const QJsonObject exhibitInfo = table_model->getExhibitInfo(currentIndex);

    ExhibitInfoDialog* exhibitInfoDialog = new ExhibitInfoDialog(exhibitInfo);
    exhibitInfoDialog->show();
}

void MainMenuWindow::on_addExhibitButton_clicked()
{

}
