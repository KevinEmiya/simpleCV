#ifndef PATTERNTRACKER_H
#define PATTERNTRACKER_H

#include <QObject>

#include <opencv2/core.hpp>

#include "Pattern.h"
#include "component/QCvCamera.h"

using namespace cv;
using namespace std;

class PatternTracker : public QObject
{
    Q_OBJECT
  public:
    explicit PatternTracker(QObject* parent = nullptr) : QObject(parent) {}

  public:
    void setPoints2D(const vector<Point2f>& points2d) { m_points2d = points2d; }
    void setPose(Pose& pose) { m_pose = pose; }
    const vector<Point2f> ponits2d() const { return m_points2d; }

    void draw2DContour(Mat& bgImg);
    void draw3DCube(Mat& bgImg, const CameraIntrinsic& intrinsic);

  private:
    void showPose();

  private:
    Mat m_homography;
    vector<Point2f> m_points2d;
    Pose m_pose;
};

#endif // PATTERNTRACKER_H
