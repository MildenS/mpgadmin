#include "exhibitinfodialog.h"
#include "ui_exhibitinfodialog.h"

ExhibitInfoDialog::ExhibitInfoDialog(const QJsonObject& exhibit_info, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExhibitInfoDialog)
{
    ui->setupUi(this);
    ui->exhibitTitle->setText(exhibit_info.value("exhibit_title").toString());
    ui->exhibitDescription->setText(exhibit_info.value("exhibit_description").toString());

    QByteArray base64 = exhibit_info.value("exhibit_image").toString().toUtf8();
    QPixmap pixmap;
    pixmap.loadFromData(QByteArray::fromBase64(base64));
    ui->exhibitImage->setPixmap(pixmap.scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

ExhibitInfoDialog::~ExhibitInfoDialog()
{
    delete ui;
}
