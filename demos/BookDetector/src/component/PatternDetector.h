#ifndef PATTERNDETECTOR_H
#define PATTERNDETECTOR_H

#include <QObject>

#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/xfeatures2d.hpp>

#include "Pattern.h"

using namespace cv;
using namespace std;

class PatternDetector : public QObject
{
    Q_OBJECT
  public:
    explicit PatternDetector(QObject* parent = nullptr);
    virtual ~PatternDetector();

  public:
    void train(const Mat& img, Mat& featureImg);
    bool findPatternFromScene(const Mat& sceneImg);

    const Mat& homography() const { return m_homography; }

  private:
    void extractFeature(const Mat& img, vector<KeyPoint>& keypoints, Mat& descriptors);
    void getMatches(const Mat &descriptors, vector<DMatch> &matches);
    Pattern* generatePattern(const Mat& img);
    bool refineMatchesWithHomography(const vector<KeyPoint>& queryKeypoints, vector<DMatch>& matches, Mat& homography);
    bool refineHomography(const Mat& sceneImg);
    void showHomography();

  private:
    Pattern* m_pattern; //pattern from muster image
    Ptr<FeatureDetector> m_detector;
    Ptr<DescriptorMatcher> m_matcher;

    Mat m_queryDescriptors;
    Mat m_homography;

};

#endif // PATTERNDETECTOR_H
