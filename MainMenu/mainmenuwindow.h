#ifndef MAINMENUWINDOW_H
#define MAINMENUWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QSharedPointer>

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

private:
    Ui::MainMenuWindow *ui;

    QSharedPointer<HttpWorker> httpWorker;
    ExhibitsTableModel* tableModel;


};

#endif // MAINMENUWINDOW_H
