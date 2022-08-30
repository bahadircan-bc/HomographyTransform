#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , fileDialog(new QFileDialog)
    , sceneRef(new QGraphicsScene), sceneInput(new QGraphicsScene), sceneOutput(new QGraphicsScene), sceneWarped(new QGraphicsScene)
    , pixmapItemRef(new QGraphicsPixmapItem), pixmapItemInput(new QGraphicsPixmapItem), pixmapItemOutput(new QGraphicsPixmapItem), pixmapItemWarped(new QGraphicsPixmapItem)
    , threadHomography(new QThread)
{
    ui->setupUi(this);


    //move detector to a seperate thread
    homographyDetector = new HomographyDetector(nullptr);
    homographyDetector->moveToThread(threadHomography);
    threadHomography->start();

    //item->scene->view
    setupScenes();

    //file dialog settings
    configureFileDialog();

    //bindings
    setBindings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupScenes()
{
    sceneRef->addItem(pixmapItemRef);
    ui->graphicsViewRefImage->setScene(sceneRef);

    sceneInput->addItem(pixmapItemInput);
    ui->graphicsViewInputImage->setScene(sceneInput);

    sceneOutput->addItem(pixmapItemOutput);
    ui->graphicsViewOutputImage->setScene(sceneOutput);

    sceneWarped->addItem(pixmapItemWarped);
    ui->graphicsViewWarpedImage->setScene(sceneWarped);
}

void MainWindow::configureFileDialog()
{
    QStringList filters;
    filters << "Image files (*.png *.xpm *.jpg *.bmp *.jpeg)"
            << "Text files (*.txt)"
            << "Any files (*)";
    fileDialog->setNameFilters(filters);
    fileDialog->setFileMode(QFileDialog::ExistingFile);
}

void MainWindow::setBindings()
{
    connect(ui->actionRef_Image, &QAction::triggered, this, &MainWindow::refImageOpenFileDialogTriggered);
    connect(ui->actionInput_Image, &QAction::triggered, this, &MainWindow::inputImageOpenFileDialogTriggered);
    connect(ui->pushButtonDebug, &QPushButton::released, homographyDetector, &HomographyDetector::detectCalled);
    connect(homographyDetector, &HomographyDetector::outputImageReadyAsMat, this, &MainWindow::displayOutputImage);
    connect(homographyDetector, &HomographyDetector::outputWarpedImageReadyAsMat, this, &MainWindow::displayWarpedImage);
    connect(ui->splitter, &QSplitter::splitterMoved, this, &MainWindow::updateViewSizes);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::updateViewSizes);
}

void MainWindow::refImageOpenFileDialogTriggered()
{
    QString fileName;
    qDebug() << "base Image file dialog triggered";
    //select file with dialog
    if(fileDialog->exec())
        fileName = fileDialog->selectedFiles()[0];

    //read image to pixmap and show it in the view
    pixmapItemRef->setPixmap(QPixmap(fileName));
    pixmapItemRef->update();

    homographyDetector->setRefMat(fileName);

    ui->graphicsViewRefImage->setSceneRect(pixmapItemRef->boundingRect());
    ui->graphicsViewRefImage->fitInView(pixmapItemRef->boundingRect(),Qt::KeepAspectRatio);
    ui->graphicsViewRefImage->update();
}

void MainWindow::inputImageOpenFileDialogTriggered()
{
    QString fileName;
    qDebug() << "compare Image file dialog triggered";
    if(fileDialog->exec())
        fileName = fileDialog->selectedFiles()[0];

    pixmapItemInput->setPixmap(QPixmap(fileName));
    pixmapItemInput->update();

    homographyDetector->setInputMat(fileName);

    ui->graphicsViewInputImage->setSceneRect(pixmapItemInput->boundingRect());
    ui->graphicsViewInputImage->fitInView(pixmapItemInput->boundingRect(),Qt::KeepAspectRatio);
    ui->graphicsViewInputImage->update();
}

void MainWindow::displayOutputImage(Mat &matResult)
{
    QImage imgIn= QImage((uchar*) matResult.data, matResult.cols, matResult.rows, matResult.step, QImage::Format_BGR888);
    pixmapItemOutput->setPixmap(QPixmap::fromImage(imgIn));
    pixmapItemOutput->update();

    ui->graphicsViewOutputImage->setSceneRect(pixmapItemOutput->boundingRect());
    ui->graphicsViewOutputImage->fitInView(sceneOutput->sceneRect(), Qt::KeepAspectRatio);
    ui->graphicsViewOutputImage->update();
}

void MainWindow::displayWarpedImage(Mat &matWarped)
{
    QImage imgIn= QImage((uchar*) matWarped.data, matWarped.cols, matWarped.rows, matWarped.step, QImage::Format_BGR888);
    pixmapItemWarped->setPixmap(QPixmap::fromImage(imgIn));
    pixmapItemWarped->update();

    ui->graphicsViewWarpedImage->setSceneRect(pixmapItemWarped->boundingRect());
    ui->graphicsViewWarpedImage->fitInView(sceneWarped->sceneRect(),Qt::KeepAspectRatio);
    ui->graphicsViewWarpedImage->update();
}

void MainWindow::updateViewSizes()
{
    ui->graphicsViewRefImage->fitInView(pixmapItemRef->boundingRect(),Qt::KeepAspectRatio);
    ui->graphicsViewInputImage->fitInView(pixmapItemInput->boundingRect(),Qt::KeepAspectRatio);
    ui->graphicsViewOutputImage->fitInView(pixmapItemOutput->boundingRect(), Qt::KeepAspectRatio);
    ui->graphicsViewWarpedImage->fitInView(pixmapItemWarped->boundingRect(),Qt::KeepAspectRatio);
}




