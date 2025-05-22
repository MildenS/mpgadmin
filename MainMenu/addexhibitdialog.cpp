#include "addexhibitdialog.h"
#include "ui_addexhibitdialog.h"

AddExhibitDialog::AddExhibitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddExhibitDialog)
{
    ui->setupUi(this);
}

AddExhibitDialog::~AddExhibitDialog()
{
    delete ui;
}
