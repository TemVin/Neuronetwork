#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QtGui>
#include <QLabel>
#include <QVBoxLayout>
#include <QPalette>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{   
    ui-> setupUi(this);

    QPixmap myPixmap("C:/Git/projects/NN/task.JPG");
    ui->labelModel->setPixmap(myPixmap);

    set_defaults();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::set_defaults()
{
    ui->lineEditNumNeur->setText("3");
    ui->lineEditTime->setText("10.0");
    ui->lineEditNumLay->setText("100");
    ui->lineEditLimit->setText("1.0");
    ui->lineEditAccur->setText("0.001");
    ui->lineEditStep->setText("0.1");
    ui->lineEditM1->setText("1.0");
    ui->lineEditM2->setText("10.0");
}

void MainWindow::init()
{
    num_layers = ui->lineEditNumLay->text().toInt();
    time_n = ui->lineEditTime->text().toDouble();
    limit = ui->lineEditLimit->text().toDouble();
    M1 = ui->lineEditM1->text().toDouble();
    M2 = ui->lineEditM2->text().toDouble();
    step = ui->lineEditStep->text().toDouble();
    accuracy = ui->lineEditAccur->text().toDouble();
}

void MainWindow::on_lineEditNumNeur_textChanged(const QString &arg1)
{
    num_neur = arg1.toInt();

    ui->tableWidgetInitValues->setRowCount(num_neur);
    ui->tableWidgetInitValues->setColumnCount(3);
    for(int i = 0; i < num_neur; i++)
        for(int j = 0; j < 3; j++)
        {
            auto item = ui->tableWidgetInitValues->item(i, j);
            if(!item)
            {
                item = new QTableWidgetItem();
                ui->tableWidgetInitValues->setItem(i, j, item);
            }
            item->setText("0.1");
        }

    ui->tableWidgetControl->setRowCount(num_neur);
    ui->tableWidgetControl->setColumnCount(num_neur);
    for(int i = 0; i < num_neur; i++)
        for(int j = 0; j < num_neur; j++)
        {
            auto item = ui->tableWidgetControl->item(i, j);
            if(!item)
            {
                item = new QTableWidgetItem();
                ui->tableWidgetControl->setItem(i, j, item);
            }
            item->setText(i == j ? "0.0" : "0.5");
        }
}

void MainWindow::on_pushButtonStart_clicked()
{
    init();
    ui->textBrowserLog->clear();

    double *X0 = new double[num_neur];
    double *A = new double[num_neur];
    double *G = new double[num_neur];
    for (int i = 0; i < num_neur; i++)
    {
        X0[i] = ui->tableWidgetInitValues->item(i, 0)->text().toDouble();
        A[i] = ui->tableWidgetInitValues->item(i, 1)->text().toDouble();
        G[i] = ui->tableWidgetInitValues->item(i, 2)->text().toDouble();
    }

    auto W = create_W();
    for(int k = 0; k < num_layers; k++)
        for(int i = 0; i < num_neur; i++)
            for(int j = 0; j < num_neur; j++)
                W[k][i][j] = ui->tableWidgetControl->item(i, j)->text().toDouble();

    dt = time_n / num_layers;

    auto X = calc_X(X0, G, W);
    auto I = calc_I(A, X, W);

    calc(X0, A, G, W, X, I);
    QMessageBox::information(this, QString("temporary view"), QString("iterations done: %1").arg(iteration));
    print(X, W);
    plot(X);

    destroy(X, W);
    delete []X0;
    delete []A;
    delete []G;
}

void MainWindow::calc(double *X0, double *A, double *G, double ***W, double **X, double I)
{
    double ***W_, **X_, I_, dI, **p;
    iteration = 0;
    ui->textBrowserLog->append((QString("init I = %1").arg(I)));

    do
    {
        ui->textBrowserLog->append(QString("iteration %1:").arg(iteration));
        p = calc_p(A, G, X, W);

        do
        {
            W_ = calc_W(X, p, W);
            X_ = calc_X(X0, G, W_);
            I_ = calc_I(A, X_, W_);
            ui->textBrowserLog->append((QString("I = %1").arg(I_)));
            dI = I_ - I;

            if(dI > 0.0)
                step /= 2;
        } while(dI > 0.0 && fabs(dI) > accuracy);

        if(fabs(dI) > accuracy)
        {
            copy(X, X_, W, W_);
            I = I_;
            destroy(X_, W_);
            iteration++;
        }
    } while(fabs(dI) > accuracy);
}

double*** MainWindow::create_W()
{
    double ***W = new double**[num_layers];
    for (int i = 0; i < num_layers; i++)
        W[i]= new double*[num_neur];
    for (int i = 0; i < num_layers; i++)
        for(int j = 0; j < num_neur; j++)
            W[i][j] = new double[num_neur];
    return W;
}

void MainWindow::copy(double **X, double **X_, double ***W, double ***W_)
{
    for(int i = 0; i < num_layers + 1; i++)
        for(int j = 0; j < num_neur; j++)
            X[i][j] = X_[i][j];


    for(int i = 0; i < num_layers; i++)
        for(int j = 0; j < num_neur; j++)
            for(int k = 0; k < num_neur; k++)
                W[i][j][k] = W_[i][j][k];
}

void MainWindow::destroy(double **X, double ***W)
{    
    for (int i = 0; i < num_layers + 1; i++)
        delete []X[i];
    delete []X;

    for (int i = 0; i < num_layers; i++)
        for(int j = 0; j < num_neur; j++)
            delete []W[i][j];
    for (int i = 0; i < num_layers + 1; i++)
        delete []W[i];
    delete []W;
}

double** MainWindow::calc_X(double *X0, double *G, double ***W)
{
    double **X = new double*[num_layers + 1];
    for (int i = 0; i < num_layers + 1; i++)
        X[i] = new double[num_neur];

    for(int i = 0; i < num_neur; i++)
        X[0][i] = X0[i];
    for(int k = 0; k < num_layers; k++)
        for(int i = 0; i < num_neur; i++)
        {
            double sum = 0.0;
            for (int j = 0; j < num_neur; j++)
                sum += W[k][i][j] * X[k][j];
            X[k + 1][i] = X[k][i] + dt * (-G[i] * X[k][i] + sum);
        }
    return X;
}

double MainWindow::calc_I(double *A, double **X, double ***W)
{
    double sum1 = 0.0;
    double sum2 = 0.0;
    for(int k = 0; k < num_layers; k++)
        for(int i = 0; i < num_neur; i++)
            for(int j = 0; j < num_neur; j++)
                sum1 += pow(W[k][i][j], 2.0);
    for(int i = 0; i < num_neur; i++)
        sum2 += pow(X[num_layers][i] - A[i], 2.0);
    return dt * M1 * sum1 + M2 * sum2;
}

double** MainWindow::calc_p(double *A, double *G, double **X, double ***W)
{
    double **p = new double*[num_layers + 1];
    for(int i = 0; i < num_layers + 1; i++)
        p[i] = new double[num_neur];

    for(int i = 0; i < num_neur; i++)
        p[num_layers][i] = -2 * M2 * (X[num_layers][i] - A[i]);
    for(int k = num_layers - 1; k > 0; k--)
        for(int i = 0; i < num_neur; i++)
        {
            double sum = 0.0;
            for(int j = 0; j < num_neur; j++)
                sum += W[k][i][j] * p[k + 1][j];
            p[k][i] = p[k + 1][i] - dt * G[i] * p[k + 1][i] + dt * sum;
        }
    return p;
}

double*** MainWindow::calc_W(double **X, double **p, double ***W)
{
    auto W_ = create_W();
    for(int k = 0; k < num_layers; k++)
        for(int i = 0; i < num_neur; i++)
            for(int j = 0; j < num_neur; j++)
            {
                W_[k][i][j] = W[k][i][j] - step * (2 * M1 * dt * W[k][i][j] - p[k + 1][i] * dt * X[k][j]);
                if(W_[k][i][j] > limit)
                    W_[k][i][j] = limit;
                if(W_[k][i][j] < -limit)
                    W_[k][i][j] = -limit;
            }
    return W_;
}

void MainWindow::print(double **X, double ***W)
{
    ui->tableWidgetFinalX->setRowCount(num_layers + 1);
    ui->tableWidgetFinalX->setColumnCount(num_neur);
    for(int i = 0; i < num_layers + 1; i++)
        for(int j = 0; j < num_neur; j++)
        {
            auto item = ui->tableWidgetFinalX->item(i, j);
            if(!item)
            {
                item = new QTableWidgetItem();
                ui->tableWidgetFinalX->setItem(i, j, item);
            }
            item->setText(QString("%1").arg(X[i][j]));
        }

    for(int k = 0; k < num_layers; k++)
    {
        auto layer = new QTableWidget(this);//take existing?
        ui->tabWidgetControl->addTab(layer, QString("Layer %1").arg(k));
        layer->setRowCount(num_neur);
        layer->setColumnCount(num_neur);
        for(int i = 0; i < num_neur; i++)
            for(int j = 0; j < num_neur; j++)
            {
                auto item = layer->item(i, j);
                if(!item)
                {
                    item = new QTableWidgetItem();
                    layer->setItem(i, j, item);
                }
                item->setText(QString("%1").arg(W[k][i][j]));
            }
    }
}

void MainWindow::plot(double **X)
{
    ui->customPlot->clearGraphs();
    QVector<double> x(num_layers + 1), y(num_layers + 1);
    for(int i = 0; i < num_layers + 1; i++)
        x[i] = i;
    for(int i = 0; i < num_neur; i++)
    {
        for(int j = 0; j < num_layers + 1; j++)
            y[j] = X[j][i];
        ui->customPlot->addGraph();
        ui->customPlot->graph(i)->setData(x, y);
        ui->customPlot->graph(i)->setPen(QPen(Qt::GlobalColor(Qt::red + i)));
    }
    ui->customPlot->xAxis->setLabel("X");
    ui->customPlot->xAxis->setRange(0.0, num_layers);
    ui->customPlot->yAxis->setLabel("Y");
    ui->customPlot->yAxis->setRange(-5.0, 5.0);
    ui->customPlot->setInteraction(QCP::iRangeDrag, true);
    ui->customPlot->setInteraction(QCP::iRangeZoom, true);
    ui->customPlot->replot();
}
