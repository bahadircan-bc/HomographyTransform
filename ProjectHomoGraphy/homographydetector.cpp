#include "homographydetector.h"

using namespace cv;

HomographyDetector::HomographyDetector(QObject *parent,
                                       int maxFeatures,
                                       NormTypes normType,
                                       float goodMatchPercentage)
    : QObject{parent}
{
    this->maxFeatures = maxFeatures;
    detector = ORB::create(maxFeatures);
    extractor = ORB::create(maxFeatures);


    this->normType = normType;
    matcher = new BFMatcher(normType, true);

    this->goodMatchPercentage = goodMatchPercentage;

    qRegisterMetaType<cv::Mat>("cv::Mat&");
}

bool HomographyDetector::setRefMat(QString fileName)
{
    matRef = imread(fileName.toStdString());
    if(matRef.empty()) {qDebug() << "baseimage couldn't load"; return false;}
    cvtColor(matRef, matRefGray, COLOR_BGR2GRAY);
    if(matRefGray.empty()) {qDebug() << "baseimage couldn't convert"; return false;}
    return true;
}

bool HomographyDetector::setInputMat(QString fileName)
{
    matInput = imread(fileName.toStdString());
    if(matInput.empty()) {qDebug() << "compareimage couldn't load"; return false;}
    cvtColor(matInput, matInputGray, COLOR_BGR2GRAY);
    if(matInputGray.empty()) {qDebug() << "compareimage couldn't convert"; return false;}
    return true;

}

bool HomographyDetector::detectKeypointsAndDescriptors()
{
    //guard clauses to check if mat objects exist
    if(matRef.empty() || matRefGray.empty() || matInput.empty() || matInputGray.empty())
    {qDebug()<< "one of the input mats is empty"; return false;}

    //keypoints are detected and stored in vectors
    detector->detect(matRefGray, vkpRef);
    detector->detect(matInputGray, vkpInput);

    if(vkpRef.empty() || vkpInput.empty()) {qDebug() << "detect caused an error"; return false;}

    extractor->compute(matRefGray,vkpRef,descriptorsRef);
    extractor->compute(matInputGray,vkpInput,descriptorsInput);

    if(descriptorsRef.empty() || descriptorsInput.empty()){qDebug() << "compute caused an error"; return false;}

    return true;
}

bool HomographyDetector::findMathes()
{
    if(descriptorsRef.empty() || descriptorsInput.empty()) { qDebug() << "one of the descriptors is empty"; return false;}

    matcher->match(descriptorsRef, descriptorsInput, matches);

    if(matches.empty()) {qDebug() << "matches are empty"; return false;}
    return true;
}

bool HomographyDetector::siftMatches()
{
    if(matches.empty()) {qDebug() << "matches are empty"; return false;}

    std::sort(matches.begin(), matches.end());
    int numGoodMatches = matches.size()*goodMatchPercentage;
    matches.erase(matches.begin() + numGoodMatches, matches.end());

    return true;
}

bool HomographyDetector::warpHomography()
{
    if(matches.empty() || vkpRef.empty() || vkpInput.empty())
    {qDebug() << "one of the inputs is empty"; return false;}
    std::vector<Point2f> points1, points2;

    for( size_t i = 0; i < matches.size(); i++ )
    {
    points1.push_back( vkpRef[matches[i].queryIdx].pt );
    points2.push_back( vkpInput[matches[i].trainIdx].pt );
    }

    if(points1.empty() || points2.empty())
    {qDebug() << "could not pushback to points"; return false;}

    homography = findHomography(points2,points1,RANSAC);

    if(homography.empty()){qDebug() << "could not find homography"; return false;}

    if(matRef.empty() || matInput.empty()) {qDebug() << "one of the input mats is empty"; return false;}
    warpPerspective(matInput, matWarped,homography,matRef.size());

    if(matWarped.empty()) {qDebug() << "could not warp image"; return false;}

    return true;
}

void HomographyDetector::detectCalled()
{
    if(!detectKeypointsAndDescriptors())
        return;
    if(!findMathes())
        return;

    drawMatches(matRef,vkpRef,matInput,vkpInput,matches,matOutput);
    if(matOutput.empty()) {qDebug() << "could not draw matches"; return;}

    emit outputImageReadyAsMat(matOutput);

    if(!siftMatches())
        return;
    if(!warpHomography())
        return;

    emit outputWarpedImageReadyAsMat(matWarped);
}
