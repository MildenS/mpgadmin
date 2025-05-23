#include "addexhibitdialog.h"
#include "ui_addexhibitdialog.h"

#include <QBuffer>
#include <QFileDialog>
#include <QHttpMultiPart>
#include <QMessageBox>

AddExhibitDialog::AddExhibitDialog(QSharedPointer<HttpWorker> httpWorker_, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddExhibitDialog)
{
    ui->setupUi(this);
    httpWorker = httpWorker_;
}

AddExhibitDialog::~AddExhibitDialog()
{
    delete ui;
}

void AddExhibitDialog::on_mainImageSelectPushButton_clicked()
{
    QString imagePath = QFileDialog::getOpenFileName(
        this,
        "Choose image",
        "",
        "Image (*.png *.jpg *.jpeg *.bmp);;"
    );

    if (imagePath.isEmpty()) {
        qDebug() << "Files didn't choose";
        return;
    }
    ui->mainImagePathLabel->setText(imagePath);
}

void AddExhibitDialog::on_selectTrainImagesPushButton_clicked()
{
    QStringList imagePaths = QFileDialog::getOpenFileNames(
        this,
        "Choose train images",
        "",
        "Image (*.png *.jpg *.jpeg *.bmp);;"
    );

    if (imagePaths.isEmpty()) {
        qDebug() << "Files didn't choose";
        return;
    }
    ui->trainImagesPathsLabel->setText(imagePaths.join("\n"));
}

void AddExhibitDialog::on_cancelButton_clicked()
{
    this->close();
}

void AddExhibitDialog::on_addButton_clicked()
{
    QString exhibitTitle = ui->exhibitTitleLineEdit->text();
    if (exhibitTitle == "")
    {
        QMessageBox errorDialog;
        errorDialog.setText("Write exhibit title!");
        errorDialog.exec();
        return;
    }

    QString exhibitDescription = ui->exhibitDescriptionTextEdit->toPlainText();
    if (exhibitDescription == "")
    {
        QMessageBox errorDialog;
        errorDialog.setText("Write exhibit description!");
        errorDialog.exec();
        return;
    }

    QString mainImagePath = ui->mainImagePathLabel->text();
    if (mainImagePath == "")
    {
        QMessageBox errorDialog;
        errorDialog.setText("Choose exhibit main image!");
        errorDialog.exec();
        return;
    }

    QStringList trainImagesPaths = ui->trainImagesPathsLabel->text().split("\n", QString::SkipEmptyParts);
    if (trainImagesPaths.empty())
    {
        QMessageBox errorDialog;
        errorDialog.setText("Choose exhibit train images!");
        errorDialog.exec();
        return;
    }

    httpWorker->addExhibit(exhibitTitle,exhibitDescription, mainImagePath, trainImagesPaths);
    emit exhibitAdded();
    this->close();
}
