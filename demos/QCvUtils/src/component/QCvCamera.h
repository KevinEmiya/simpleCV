#ifndef QCVCAMERA_H
#define QCVCAMERA_H

#include <QObject>
#include <QString>
#include <QList>

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
    const QList<Pose>& poses() const { return m_poses; }
    bool isIntrinsicValid() const;

    bool loadCalibrationData(const QString& fileName);

  public slots:
    void addPose(const cv::Mat& rotation, const cv::Mat& translation)
    {
        Pose pose(rotation, translation);
        m_poses.append(pose);
    }

  private:
    CameraIntrinsic m_intrinsic;
    QList<Pose> m_poses;
};

#endif // QCVCAMERA_H
