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
    void init();
    void set_defaults();
    void print(double **X, double ***W);
    void plot(double **X);

    double*** create_W();
    void destroy(double ***W, double **X);
    double** calc_X(double *X0, double *G, double ***W);
    double calc_I(double *A, double **X, double ***W);
    double** calc_p(double *A, double *G, double **X, double ***W);
    double*** calc_W(double **X, double **p, double ***W);
    void calc(double *X0, double *A, double *G, double ***W, double **X, double I);

private:
    Ui::MainWindow *ui;
    int num_neur;   // число нейронов
    int num_layers; // число слоев
    double time_n;  // время процесса
    double limit;   // ограничения на управления
    double M1;      // штрафные коэффициенты
    double M2;      // штрафные коэффициенты
    double step;    // шаг градиен. спуска
    double accuracy;// точность
    double dt;      // dt = T/q
    int iteration;  // итерация
};

#endif // MAINWINDOW_H
