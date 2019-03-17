#include "PatternTracker.h"

#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <QDebug>

#include "util/QCvDataUtils.h"

#include <math.h>

void PatternTracker::draw3DCube(Mat& bgImg, const CameraIntrinsic& intrinsic)
{
    vector<Point3d> vispts3d;
    vector<Point2d> vispts2d;
    // axis
    vispts3d.push_back(Point3d(0, 0, 0));
    vispts3d.push_back(Point3d(1, 0, 0));
    vispts3d.push_back(Point3d(0, 1, 0));
    vispts3d.push_back(Point3d(0, 0, 1));
    // cube
    double cubeSize = 0.5;
    vispts3d.push_back(Point3d(-cubeSize, -cubeSize, cubeSize));
    vispts3d.push_back(Point3d(cubeSize, -cubeSize, cubeSize));
    vispts3d.push_back(Point3d(cubeSize, cubeSize, cubeSize));
    vispts3d.push_back(Point3d(-cubeSize, cubeSize, cubeSize));
    vispts3d.push_back(Point3d(-cubeSize, -cubeSize, 0));
    vispts3d.push_back(Point3d(cubeSize, -cubeSize, 0));
    vispts3d.push_back(Point3d(cubeSize, cubeSize, 0));
    vispts3d.push_back(Point3d(-cubeSize, cubeSize, 0));
    // project 3D->2D
    projectPoints(vispts3d, m_pose.rotation, m_pose.translation,
                  intrinsic.cameraMat, intrinsic.distortCoeff, vispts2d);
    // draw cube
    vector<Point> face{vispts2d[5], vispts2d[4], vispts2d[7],
                       vispts2d[11], vispts2d[10], vispts2d[9]};
    vector<vector<Point>> faces{face};
    fillPoly(bgImg, faces, Scalar(200, 128, 128, 100));
    Scalar cubeColor = Scalar(200, 200, 200);
    for (int i = 4; i <= 10; i++)
    {
        if (i <= 7)
        {
            cv::line(bgImg, vispts2d[i], vispts2d[i + 4], cubeColor, 3);
        }
        if (i != 7)
        {
            cv::line(bgImg, vispts2d[i], vispts2d[i + 1], cubeColor, 3);
        }
    }
    cv::line(bgImg, vispts2d[7], vispts2d[4], cubeColor, 3);
    cv::line(bgImg, vispts2d[11], vispts2d[8], cubeColor, 3);
    // draw axis
    cv::line(bgImg, vispts2d[0], vispts2d[1], Scalar(255, 0, 0), 3);
    cv::line(bgImg, vispts2d[0], vispts2d[2], Scalar(0, 255, 0), 3);
    cv::line(bgImg, vispts2d[0], vispts2d[3], Scalar(0, 0, 255), 3);
}

void PatternTracker::draw2DContour(Mat& bgImg)
{
    if (!m_points2d.empty())
    {
        size_t i;
        for (i = 0; i < m_points2d.size() - 1; i++)
        {
            cv::line(bgImg, m_points2d[i], m_points2d[i + 1], Scalar(0, 0, 255), 3);
        }
        cv::line(bgImg, m_points2d[i], m_points2d[0], Scalar(0, 0, 255), 3);
    }
    else
    {
        qWarning() << "No 2D contour available!";
    }
}
