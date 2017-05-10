#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QtGui>
#include <QLabel>
#include <QVBoxLayout>
#include <QPalette>
#include <QMessageBox>
#include <vector>


std::vector<double> X0, A, G;
double ***W;
double **X;
int num_neur;   // число нейронов
int num_layers; // число слоев
int time_n;     // время процесса
int limit;      // ограничения на управления
double M1;         // штрафные коэффициенты
double M2;         // штрафные коэффициенты
int step;       // шаг градиен. спуска
int accuracy;   // точность
double dt;      // dt = T/q

void calc_X();
double calc_I();
double** calc_p();

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{   
    ui-> setupUi(this);
    QVBoxLayout *l_tab1 = new QVBoxLayout();

    QLabel *label = new QLabel();
    QPixmap myPixmap( "C:/Qt/projects/Inna_Anatolyevna/task/task.jpg" );
    label-> setPixmap( myPixmap );
    l_tab1->addWidget(label);
    ui->tabParams->setLayout(l_tab1);

    set_defaults();
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_lineEditNumNeur_textChanged(const QString &arg1)
{
    num_neur = arg1.toInt();

    ui->tableWidgetInitValues->setRowCount(num_neur);
    ui->tableWidgetInitValues->setColumnCount(3);
    ui->tableWidgetInitValues->setShowGrid(true);

    ui->tableWidgetControl->setRowCount(num_neur);
    ui->tableWidgetControl->setColumnCount(num_neur);
    ui->tableWidgetControl->setShowGrid(true);
}

void MainWindow::on_pushButtonStart_clicked()
{
    init();
    dt = time_n / num_layers;
    calc_X();
    auto I = calc_I();
    QMessageBox::information(this, QString("temporary view"), QString("I = %1").arg(I));
}

void MainWindow::init()
{
    for (int i = 0; i < num_neur; i++)
    {
        X0.push_back(ui->tableWidgetInitValues->item(i, 0)->text().toDouble());
        A.push_back(ui->tableWidgetInitValues->item(i, 1)->text().toDouble());
        G.push_back(ui->tableWidgetInitValues->item(i, 2)->text().toDouble());
    }
    num_layers = ui->lineEditNumLay->text().toInt();
    W = new double**[num_layers];
    for (int i = 0; i < num_layers; i++)
        W[i]= new double*[num_neur];
    for (int i = 0; i < num_layers; i++)
        for(int j = 0; j < num_neur; j++)
            W[i][j] = new double[num_neur];
    for(int k = 0; k < num_layers; k++)
        for(int i = 0; i < num_neur; i++)
            for(int j = 0; j < num_neur; j++)
                W[k][i][j] = ui->tableWidgetControl->item(i, j)->text().toDouble();
    time_n = ui->lineEditTime->text().toInt();
    limit = ui->lineEditLimit->text().toInt();
    M1 = ui->lineEditM1->text().toDouble();
    M2 = ui->lineEditM2->text().toDouble();
    step = ui->lineEditStep->text().toInt();
    accuracy = ui->lineEditAccur->text().toInt();
}

void MainWindow::set_defaults()
{
    ui->lineEditNumNeur->setText("3");
    ui->lineEditTime->setText("10");
    ui->lineEditNumLay->setText("300");
    ui->lineEditLimit->setText("1");
    ui->lineEditAccur->setText("0.01");
    ui->lineEditStep->setText("0.1");//?
    ui->lineEditM1->setText("1.0");
    ui->lineEditM2->setText("1.0");
}

void calc_X()
{
    X = new double*[num_layers];
    for (int i = 0; i < num_layers; i++)
        X[i] = new double[num_neur];
    for(int i = 0; i < num_neur; i++)
        X[0][i] = X0[i];
    for(int k = 0; k < num_layers - 1; k++)
        for(int i = 0; i < num_neur; i++)
        {
            double sum = 0.0;
            for (int j = 0; j < num_neur; j++)
                sum += W[k][i][j] * X[k][j];
            X[k+1][i] = X[k][i] + dt * (-G[i] * X[k][i] + sum);
        }
}

double calc_I()
{
    double sum1 = 0.0;
    double sum2 = 0.0;
    for(int k = 0; k < num_layers; k++)
        for(int i = 0; i < num_neur; i++)
            for(int j = 0; j < num_neur; j++)
                sum1 += pow(W[k][i][j], 2);
    for(int i = 0; i < num_neur; i++)
        sum2 += pow(X[num_layers - 1][i] - A[i], 2);
    return dt * M1 * sum1 + M2 * sum2;
}

double** calc_p()
{
    double **p = new double*[num_layers];
    for(int i = 0; i < num_layers; i++)
        p[i] = new double[num_neur];
    for(int i = 0; i < num_neur; i++)
        p[num_layers - 1][i] = -2 * M2 * (X[num_layers - 1][i] - A[i]);
    for(int i = 0; i < num_neur; i++)
        for(int k = num_layers - 2; k > 0; k--)
        {
            double sum = 0.0;
            for(int j = 0; j < num_neur; j++)//indeces begin with 0 or 1??????
                sum += W[k][i][j] * p[k + 1][j];
            p[k][i] = p[k + 1][i] - dt * G[i] * p[k + 1][i] + dt * sum;
        }
}




/*
    QPalette pal(ui->tabWidget->palette());
    pal.setColor(QPalette::Background, Qt::black);
    ui->tabWidget->setPalette(pal);*/
/*void MainWindow::on_pushButtonView_clicked()
{
    this->hide();
    QMainWindow * c = this->findChild<QMainWindow *>();
    if (c != NULL)
    {
        c->deleteLater();
    }
    QMainWindow * v = new ViewWindow(this);
    v->show();
}*/
