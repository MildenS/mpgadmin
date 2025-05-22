#ifndef MAINMENUWINDOW_H
#define MAINMENUWINDOW_H

#include <QMainWindow>
#include <QString>

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

private:
    Ui::MainMenuWindow *ui;

    HttpWorker* httpWorker;
    ExhibitsTableModel* table_model;
};

#endif // MAINMENUWINDOW_H
