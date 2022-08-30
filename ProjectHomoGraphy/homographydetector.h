#ifndef HOMOGRAPHYDETECTOR_H
#define HOMOGRAPHYDETECTOR_H

#include <QObject>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core.hpp>

#include <QDebug>

using namespace cv;

class HomographyDetector : public QObject
{
    Q_OBJECT
public:
    explicit HomographyDetector(QObject *parent = nullptr,
                                int maxFeatures = 500,
                                NormTypes normType = NORM_HAMMING,
                                float goodMatchPercentage = 0.15f);

    int maxFeatures;
    float goodMatchPercentage;
    //trying surf instead of orb
    Ptr<FeatureDetector> detector;
    Ptr<DescriptorExtractor> extractor;

    cv::BFMatcher* matcher;
    NormTypes normType;

    bool setRefMat(QString fileName);
    bool setInputMat(QString fileName);

public slots:
    void detectCalled();

signals:
    void outputImageReady(QImage);
    void outputImageReadyAsMat(cv::Mat&);
    void outputWarpedImageReady(QImage);
    void outputWarpedImageReadyAsMat(cv::Mat&);

private:
    Mat matRef;
    Mat matRefGray;
    std::vector<KeyPoint> vkpRef;
    Mat descriptorsRef;

    Mat matInput;
    Mat matInputGray;
    std::vector<KeyPoint> vkpInput;
    Mat descriptorsInput;

    std::vector<DMatch> matches;
    Mat matOutput;

    Mat homography;
    Mat matWarped;

    bool detectKeypointsAndDescriptors();
    bool findMathes();
    bool siftMatches();
    bool warpHomography();
};

#endif // HOMOGRAPHYDETECTOR_H
