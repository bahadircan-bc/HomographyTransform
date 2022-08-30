#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QThread>

#include <opencv2/opencv.hpp>
#include "homographydetector.h"

//#include <QImage>
//#include <QPixmap>

#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QFileDialog* fileDialog;

    QGraphicsScene* sceneRef;
    QGraphicsScene* sceneInput;
    QGraphicsScene* sceneOutput;
    QGraphicsScene* sceneWarped;
    QGraphicsPixmapItem* pixmapItemRef;
    QGraphicsPixmapItem* pixmapItemInput;
    QGraphicsPixmapItem* pixmapItemOutput;
    QGraphicsPixmapItem* pixmapItemWarped;

    HomographyDetector* homographyDetector;
    QThread* threadHomography;

    void setupScenes();
    void configureFileDialog();
    void setBindings();

public slots:
    void refImageOpenFileDialogTriggered();
    void inputImageOpenFileDialogTriggered();
    void displayOutputImage(cv::Mat&);
    void displayWarpedImage(cv::Mat&);
    void updateViewSizes();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
