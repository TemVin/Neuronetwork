#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:


    void on_lineEditNumNeur_textChanged(const QString &arg1);

    void on_pushButtonStart_clicked();

private:
    Ui::MainWindow *ui;
    void init();
    void set_defaults();
};

#endif // MAINWINDOW_H
