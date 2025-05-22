#ifndef ADDEXHIBITDIALOG_H
#define ADDEXHIBITDIALOG_H

#include <QDialog>
#include <QSharedDataPointer>
#include "HttpWorker/httpworker.h"

namespace Ui {
class AddExhibitDialog;
}

class AddExhibitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddExhibitDialog(QSharedPointer<HttpWorker> httpWorker_, QWidget *parent = nullptr);
    ~AddExhibitDialog();

private slots:
    void on_mainImageSelectPushButton_clicked();

    void on_selectTrainImagesPushButton_clicked();

    void on_cancelButton_clicked();

    void on_addButton_clicked();

signals:
    void exhibitAdded();

private:
    Ui::AddExhibitDialog *ui;

    QSharedPointer<HttpWorker> httpWorker;
};

#endif // ADDEXHIBITDIALOG_H
