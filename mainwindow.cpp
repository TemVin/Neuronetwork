#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QtGui>
#include <QLabel>
#include <QVBoxLayout>
#include <QPalette>
#include <vector>


std::vector<double> X0, A, G;
double ***W;
double **X;
int num_neur;   // число нейронов
int num_layers; // число слоев
int time_n;     // время процесса
int limit;      // ограничения на управления
int M1;         // штрафные коэффициенты
int M2;         // штрафные коэффициенты
int step;       // шаг градиен. спуска
int accuracy;   // точность
double dt;      // dt = T/q

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
    ui->tab->setLayout(l_tab1);

    set_defaults();
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    num_neur = ui->lineEdit->text().toInt();

    ui->tableWidget->setRowCount(num_neur);
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setShowGrid(true);

    ui->tableWidget_2->setRowCount(num_neur);
    ui->tableWidget_2->setColumnCount(num_neur);
    ui->tableWidget_2->setShowGrid(true);
}

void MainWindow::on_pushButton_clicked()
{
    init();
    dt = time_n / num_layers;
}

void MainWindow::init()
{
    for (int i = 0; i < num_neur;i++)
    {
        X0.push_back(ui->tableWidget->item(i, 0)->text().toDouble());
        A.push_back(ui->tableWidget->item(i, 1)->text().toDouble());
        G.push_back(ui->tableWidget->item(i, 2)->text().toDouble());
    }
    num_layers = ui->lineEdit_3->text().toInt();
    W = new double**[num_layers];
    for (int i = 0; i < num_layers; i++)
        W[i]= new double*[num_neur];
    for (int i = 0; i < num_layers; i++)
        for(int j = 0; j < num_neur; j++)
            W[i][j] = new double[num_neur];
    for(int k = 0; k < num_layers; k++)
        for(int i = 0; i < num_neur; i++)
            for(int j = 0; j < num_neur; j++)
                W[k][i][j] = ui->tableWidget_2->item(i, j)->text().toDouble();
    time_n = ui->lineEdit_7->text().toInt();
    limit = ui->lineEdit_2->text().toInt();
    M1 = ui->lineEdit_5->text().toInt();
    M2 = ui->lineEdit_6->text().toInt();
    step = ui->lineEdit_4->text().toInt();
    accuracy = ui->lineEdit_8->text().toInt();

}

void MainWindow:: set_defaults()
{
    ui->lineEdit->setText("3");
    ui->lineEdit_7->setText("10");
    ui->lineEdit_3->setText("300");
    ui->lineEdit_2->setText("1");
    ui->lineEdit_8->setText("0.01");
    ui->lineEdit_4->setText("0.1");//?
    ui->lineEdit_5->setText("1");
    ui->lineEdit_6->setText("1");

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
