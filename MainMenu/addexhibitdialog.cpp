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

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // --- Title
    QHttpPart titlePart;
    titlePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"exhibit_title\""));
    titlePart.setBody(exhibitTitle.toUtf8());
    multiPart->append(titlePart);

    // --- Description
    QHttpPart descriptionPart;
    descriptionPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"exhibit_description\""));
    descriptionPart.setBody(exhibitDescription.toUtf8());
    multiPart->append(descriptionPart);

    auto scaledImageData = [](const QString& imagePath) -> QByteArray {
        QImage image(imagePath);
        if (image.isNull()) {
            return {};
        }

        QSize maxSize(512, 512);
        QSize scaledSize = image.size().boundedTo(maxSize);
        QImage scaledImage = image.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        scaledImage.save(&buffer, "JPG", 90);
        return byteArray;
    };

    QByteArray mainImageBytes = scaledImageData(mainImagePath);
    if (mainImageBytes.isEmpty()) {
        QMessageBox::critical(nullptr, "Error", "Cannot load or scale main image.");
        multiPart->deleteLater();
        return;
    }
    QHttpPart mainImagePart;
    mainImagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                            QVariant("form-data; name=\"main-image\"; filename=\"" + QFileInfo(mainImagePath).fileName() + "\""));
    mainImagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    mainImagePart.setBody(mainImageBytes);
    multiPart->append(mainImagePart);

    // --- Train images
    for (int i = 0; i < trainImagesPaths.size(); ++i) {
        QString path = trainImagesPaths[i];
        QByteArray imageBytes = scaledImageData(path);
        if (imageBytes.isEmpty()) {
            QMessageBox::critical(nullptr, "Error", "Cannot load or scale training image: " + path);
            multiPart->deleteLater();
            return;
        }

        QHttpPart trainImagePart;
        QString key = QString("image-%1").arg(i);
        trainImagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                                 QVariant("form-data; name=\"" + key + "\"; filename=\"" + QFileInfo(path).fileName() + "\""));
        trainImagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
        trainImagePart.setBody(imageBytes);
        multiPart->append(trainImagePart);
    }

    QNetworkRequest request(QUrl("http://localhost:8888/add-exhibit"));
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkReply *reply = manager->post(request, multiPart);
    multiPart->setParent(reply);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QMessageBox::information(nullptr, "Success", "Exhibit added successfully!");
    } else {
        QMessageBox::critical(nullptr, "Error", "Failed to send exhibit: " + reply->errorString());
    }
    reply->deleteLater();
    emit exhibitAdded();
}
