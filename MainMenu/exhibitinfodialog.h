#ifndef EXHIBITINFODIALOG_H
#define EXHIBITINFODIALOG_H

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class ExhibitInfoDialog;
}

class ExhibitInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExhibitInfoDialog(const QJsonObject& exhibit_info, QWidget *parent = nullptr);
    ~ExhibitInfoDialog();

private:
    Ui::ExhibitInfoDialog *ui;

};

#endif // EXHIBITINFODIALOG_H
