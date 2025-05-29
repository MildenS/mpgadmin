#include "mainmenuwindow.h"
#include "ui_mainmenuwindow.h"
#include "exhibitinfodialog.h"
#include "addexhibitdialog.h"

#include <QMessageBox>
#include <QMessageBox>
#include <QScroller>

MainMenuWindow::MainMenuWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainMenuWindow)
{
    ui->setupUi(this);
    httpWorker = QSharedPointer<HttpWorker>(new HttpWorker);
    QJsonDocument exhibits = httpWorker->getExhibits();
    tableModel = new ExhibitsTableModel(exhibits);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(tableModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(0);

    ui->exhibitsTable->setModel(proxyModel);
    ui->exhibitsTable->resizeColumnsToContents();
    ui->exhibitsTable->resizeRowsToContents();
    ui->exhibitsTable->verticalHeader()->setDefaultSectionSize(256);
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
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("No choosen exhibit"), tr("Please choose exhibit from table by click"));
        return;
    }

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
    ui->exhibitsTable->verticalHeader()->setDefaultSectionSize(256);
    ui->exhibitsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->exhibitsTable->setColumnHidden(2, true);
    QScroller::grabGesture(ui->exhibitsTable->viewport(), QScroller::LeftMouseButtonGesture);
}

void MainMenuWindow::on_deleteExhibitButton_clicked()
{
    QModelIndex currentIndex = ui->exhibitsTable->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("No choosen exhibit"), tr("Please choose exhibit from table by click"));
        return;
    }
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  tr("Confirmation"),
                                  tr("Are you sure you want to delete this exhibit?"),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    QModelIndex indexInSameRow = currentIndex.sibling(currentIndex.row(), 2);
    QString exhibitId = ui->exhibitsTable->model()->data(indexInSameRow).toString();
    qDebug() << exhibitId;
    bool isDeleted = httpWorker->deleteExhibit(exhibitId);
    if (isDeleted)
    {
        QMessageBox::information(this, tr("Exhibit delete status"), tr("Exhibit deleted successful"));
        onExhibitAdd(); // need to refacor
    }
    else {
         QMessageBox::warning(this, tr("Exhibit delete status"), tr("Exhibit deleted end with error"));
    }
}

void MainMenuWindow::on_filterExhibitsLineEdit_textChanged(const QString &arg1)
{
    proxyModel->setFilterFixedString(arg1);
}
