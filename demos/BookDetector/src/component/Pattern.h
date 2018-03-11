#ifndef PATTERN_H
#define PATTERN_H

#include <opencv2/core.hpp>
#include <vector>

using namespace cv;
using namespace std;

class Pattern
{
  public:
    Pattern(const Size& size, const Mat& data,
            const vector<KeyPoint>& keypoints,
            const Mat& descriptors,
            const vector<Point2f>& points2d,
            const vector<Point3f>& points3d) : m_size(size), m_data(data),
                                         m_keypoints(keypoints), m_descriptors(descriptors),
                                         m_points2d(points2d), m_points3d(points3d)
    {
    }

  public:
    const Size& size() const { return m_size; }
    const Mat& data() const { return m_data; }
    const vector<KeyPoint>& keypoints() const { return m_keypoints; }
    const Mat& descriptors() const { return m_descriptors; }
    const vector<Point2f>& points2d() const { return m_points2d; }
    const vector<Point3f>& points3d() const { return m_points3d; }

  private:
    Size m_size; //image size
    Mat m_data;  //original image
    vector<KeyPoint> m_keypoints;
    Mat m_descriptors;

    vector<Point2f> m_points2d;
    vector<Point3f> m_points3d;
};

#endif // PATTERN_H
