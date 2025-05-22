#include "mainmenuwindow.h"
#include "ui_mainmenuwindow.h"
#include "exhibitinfodialog.h"
#include "addexhibitdialog.h"

#include <QScroller>

MainMenuWindow::MainMenuWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainMenuWindow)
{
    ui->setupUi(this);
    httpWorker = QSharedPointer<HttpWorker>(new HttpWorker);
    QJsonDocument exhibits = httpWorker->getExhibits();
    tableModel = new ExhibitsTableModel(exhibits);
    ui->exhibitsTable->setModel(tableModel);
    ui->exhibitsTable->resizeColumnsToContents();
    ui->exhibitsTable->resizeRowsToContents();
    //ui->exhibitsTable->verticalHeader()->setDefaultSectionSize(256);
    ui->exhibitsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->exhibitsTable->setColumnHidden(2, true);
    QScroller::grabGesture(ui->exhibitsTable->viewport(), QScroller::LeftMouseButtonGesture);
}

MainMenuWindow::~MainMenuWindow()
{
    delete ui;
}


void MainMenuWindow::on_showExhibitButton_clicked()
{
    QModelIndex currentIndex = ui->exhibitsTable->currentIndex();
    const QJsonObject exhibitInfo = tableModel->getExhibitInfo(currentIndex);

    ExhibitInfoDialog* exhibitInfoDialog = new ExhibitInfoDialog(exhibitInfo);
    exhibitInfoDialog->show();
}

void MainMenuWindow::on_addExhibitButton_clicked()
{
    AddExhibitDialog* add_ex_dialog = new AddExhibitDialog(httpWorker);
    connect(add_ex_dialog, &AddExhibitDialog::exhibitAdded,
            this, &MainMenuWindow::onExhibitAdd);
    add_ex_dialog->exec();
}

void MainMenuWindow::onExhibitAdd()
{
    QJsonDocument exhibits = httpWorker->getExhibits();
    tableModel->updateModel(exhibits);
}
