#ifndef MAINMENUWINDOW_H
#define MAINMENUWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

#include <HttpWorker/httpworker.h>
#include <MainMenu/exhibitstablemodel.h>

namespace Ui {
class MainMenuWindow;
};

class MainMenuWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainMenuWindow(QWidget *parent = nullptr);
    virtual ~MainMenuWindow();

private slots:
    void on_showExhibitButton_clicked();

    void on_addExhibitButton_clicked();

    void onExhibitAdd();

    void on_deleteExhibitButton_clicked();

    void on_filterExhibitsLineEdit_textChanged(const QString &arg1);

private:
    Ui::MainMenuWindow *ui;

    QSharedPointer<HttpWorker> httpWorker;
    ExhibitsTableModel* tableModel;
    QSortFilterProxyModel *proxyModel;


};

#endif // MAINMENUWINDOW_H
