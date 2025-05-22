#ifndef ADDEXHIBITDIALOG_H
#define ADDEXHIBITDIALOG_H

#include <QDialog>

namespace Ui {
class AddExhibitDialog;
}

class AddExhibitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddExhibitDialog(QWidget *parent = nullptr);
    ~AddExhibitDialog();

private:
    Ui::AddExhibitDialog *ui;
};

#endif // ADDEXHIBITDIALOG_H
