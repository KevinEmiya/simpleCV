#ifndef CAMCALIBRATOR_H
#define CAMCALIBRATOR_H

#include <QObject>
#include <opencv2/imgproc.hpp>
#include <vector>

class CamCalibrator : public QObject
{
    Q_OBJECT
  public:
    explicit CamCalibrator(QObject* parent = nullptr);

  public:
    void setSquareWidth(const float& width) { m_squareWidth = width; }
    void setSquareHeight(const float& height) { m_squareHeight = height; }

  signals:
    void calibrated(QString result);

  public slots:
    void onImgPts(const std::vector<cv::Point2f>& imgPts,
                  const int& colCnt, const int& rowCnt,
                  const cv::Size& imgSize);
    void reset();
    void saveResults(QString fileName);

  private:
    template <typename T>
    void checkAndPushData(std::vector<T>& vector, const T& data);
    void runCalibration(const cv::Size& imgSize);

    void showParams();

  private:
    float m_squareWidth;
    float m_squareHeight;

    std::vector<std::vector<cv::Point2f>> m_imgPts;
    std::vector<std::vector<cv::Point3f>> m_objPts;

    cv::Mat m_cameraMatrix;
    cv::Mat m_distCoeffs;
    double m_rms;
};

#endif // CAMCALIBRATOR_H
