#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->runButton, &QPushButton::clicked, ui->canvasWidget, &CanvasWidget::runConvexHull);
    connect(ui->clearButton, &QPushButton::clicked, ui->canvasWidget, &CanvasWidget::clearCanvas);
}

MainWindow::~MainWindow()
{
    delete ui;
}
