#ifndef QCVCAMERA_H
#define QCVCAMERA_H

#include <QObject>
#include <QString>

#include <opencv2/core.hpp>

struct CameraIntrinsic
{
    cv::Mat cameraMat;
    cv::Mat distortCoeff;
};

struct Pose
{
    Pose()
    {
    }

    Pose(const cv::Mat& rotation, const cv::Mat& translation)
    {
        this->rotation = rotation;
        this->translation = translation;
    }
    cv::Mat rotation; // in Quaternion
    cv::Mat translation;
};

class QCvCamera : public QObject
{
    Q_OBJECT
  public:
    explicit QCvCamera(QObject* parent = nullptr);

  public:
    void setIntrinsic(const CameraIntrinsic& intrinsic) { m_intrinsic = intrinsic; }
    const CameraIntrinsic& intrinsic() const { return m_intrinsic; }
    const Pose& extrinsic() const { return m_extrinsic; }
    bool isIntrinsicValid() const;

    bool loadCalibrationData(const QString& fileName);

  public slots:
    void onExtrinsicChanged(const cv::Mat& rotation, const cv::Mat& translation)
    {
        m_extrinsic.rotation = rotation;
        m_extrinsic.translation = translation;
    }

  private:
    CameraIntrinsic m_intrinsic;
    Pose m_extrinsic;
};

#endif // QCVCAMERA_H
