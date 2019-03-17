# OpenCV拾趣（七）——实现相机标定工具
## 本篇简介
在系列的[第五小节](https://www.jianshu.com/p/c39bd9d8cfaa)我们曾提到过，在需要与现实场景进行交互的应用中，例如在进行AR绘制、3D场景重建时，作为连接现实世界与图像世界的纽带，相机的参数起着至关重要的作用。而相机标定工具的目的，就是通过对标准模式图像的处理，计算出相机的各类参数。  
本小节我们就来使用QCvUtils基础包实现一个相机标定工具。  

## 原理简介
如系列第五小节所述，相机的标定数据可以分为两类：
- 内部参数(intrinsic parameters)：包括相机的焦距、相平面中心坐标和扭曲参数等相机本身属性参数。
- 外部参数(extrinsic parameters)：相机相对世界坐标系的位置信息，包括相对于三个坐标轴的旋转和相对于原点的平移。

具体而言，借用[OpenCV官方相机校准说明文档][1]的一张公式图：
![针孔相机坐标映射模型](https://docs.opencv.org/3.0-beta/_images/math/86122d34cf1d64d800fb3a8613f83bba441e9a9d.png)

公式描述了真实世界坐标（大写的XYZ）到图像坐标（uv）的映射。不同拍摄角度下的相机外部参数均不相同，需要在实际的场景中才能确定。而内部参数与拍摄场景无关，可以通过一组从标准模式（例如棋盘格）中采集的真实坐标和图像坐标，事先求解计算出来。我们本篇要实现的相机标定工具，就是要实现：
- 标定坐标数据的便捷采集   
以及
- 相机内部参数的自动计算和导出

具体的计算原理在这里就不作详细展开了，有兴趣的话可以参考官方文档的相关说明。接下来我们重点关注如何使用OpenCV的Calib3D工具包和我们的QCvUtils基础包实现这个标定工具。

## 实现棋盘格角点检测滤波器QCvEdgePtFilter
秉承上文所述，我们首先要实现标定坐标数据的便捷采集。对此，我们需要对视频流中出现的棋盘格进行处理。为此，需要实现的主要功能为：
- 根据棋盘格行、列数配置，检测采集到图像帧棋盘格的角点，并将检测到的角点坐标发送给内参计算模块   

按照先前已实现各类场景的思路，我们首先来实现供QCvCamView控件使用的角点检测滤波器QCvEdgePtFilter，声明如下：
```cpp
class QCvEdgePtFilter : public QCvMatFilter
{
    Q_OBJECT
  public:
    explicit QCvEdgePtFilter(QString name, QObject* parent = nullptr) : QCvMatFilter(name, parent) {}
    virtual ~QCvEdgePtFilter() { m_pointbuf.clear(); }

  public:
    void setColCnt(const int& colCnt) { m_colCnt = colCnt; }
    void setRowCnt(const int& rowCnt) { m_rowCnt = rowCnt; }

  signals:
    void imgPtsCaptured(const std::vector<cv::Point2f>& imgPts,
                        const int& colCnt, const int& rowCnt,
                        const cv::Size& imgSize);

  public slots:
    void onCapture();

  protected:
    virtual void execFilter(const cv::Mat& inMat, cv::Mat& outMat);

  private:
    int m_colCnt;
    int m_rowCnt;
    std::vector<cv::Point2f> m_pointbuf;
    cv::Size m_imgSize;
};
```
除了QCvMatFilter要求实现的执行滤波方法execFilter外，从声明中可以看出这个滤波器还实现了以下功能：
- 提供输入棋盘格行、列数量的入口，即各类setter方法
- 提供棋盘角点的发送出口，借助Qt的信号机制实现（imgPtsCaptured信号）
- 触发数据采集的槽onCapture

execFilter方法具体实现如下：
```cpp
void QCvEdgePtFilter::execFilter(const cv::Mat& inMat, cv::Mat& outMat)
{
    if (m_colCnt <= 1 || m_rowCnt <= 1)
    {
        qDebug() << "BOARD SIZE EMPTY!";
        outMat = inMat.clone();
    }

    cv::Mat frameGray;
    cv::cvtColor(outMat, frameGray, cv::COLOR_BGR2GRAY);
    //-- Detect corners
    m_pointbuf.clear();
    bool found = cv::findChessboardCorners(frameGray, cv::Size(m_colCnt, m_rowCnt),
                                           m_pointbuf, cv::CALIB_CB_ADAPTIVE_THRESH |
                                                           cv::CALIB_CB_FAST_CHECK);
    if (found)
    {
        cv::cornerSubPix(frameGray, m_pointbuf, cv::Size(11, 11),
                         cv::Size(-1, -1),
                         cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT,
                                          30, 0.1));
        m_imgSize = outMat.size();
        for (size_t i = 0; i < m_pointbuf.size(); i++)
        {
            cv::ellipse(outMat, m_pointbuf[i],
                        cv::Size(5, 5), 0, 0, 360,
                        cv::Scalar(255, 0, 255), 4, 8, 0);
        }
    }
}
```
这其中核心的处理就是OpenCV提供的findChessboardCorners方法，将灰度图像及棋盘格大小传入后，这个方法会检测出图中所有的角点，并将坐标按顺序存储到缓存数组m_pointbuf中。  
如果从采集到的图像帧中顺利检测到了棋盘格角点，滤波器还会进一步使用角点亚像素精细化方法（cornerSubPix）提升角点坐标精度，之后将计算得到的角点绘制在图像帧上。

完成了上面的实现，在需要采集标定坐标数据时，只需要将缓存数组中的数据发送出去就可以了：
```cpp
void QCvEdgePtFilter::onCapture()
{
    if(!m_pointbuf.empty())
    {
        emit imgPtsCaptured(m_pointbuf, m_colCnt, m_rowCnt, m_imgSize);
    }
}
```

*注： 需要特别说明的是，为了避免边界问题，OpenCV处理棋盘格图像时会舍弃最外圈的一层棋盘格，因此棋盘格左上角会从第二行第二列的格子开始计算。后文将依照此约定进行说明。*

## 实现相机标定器CamCalibrator
完成了标定坐标的采集，下一步自然是实现对所采集数据进行处理的模块，也就是下面所声明的相机标定器CamCalibrator类：
```cpp
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
```
这其中设置棋盘格真实宽高的两个setter方法和一些零散的工具方法就不在这里展开说明了，我们重点看一下实现核心功能的几个方法。首先是接收上文imgPtsCaptured信号的槽方法onImgPts：
```cpp
void CamCalibrator::onImgPts(const std::vector<cv::Point2f>& imgPts,
                             const int& colCnt, const int& rowCnt,
                             const cv::Size& imgSize)
{
    if (m_squareHeight >= 0 && m_squareWidth >= 0 && imgPts.size() > 0)
    {
        checkAndPushData(m_imgPts, imgPts);
        //calculate object points;
        std::vector<cv::Point3f> corners;
        for (int i = 0; i < rowCnt; i++)
        {
            for (int j = 0; j < colCnt; j++)
            {
                corners.push_back(cv::Point3f(float(j * m_squareHeight),
                                              float(i * m_squareWidth), 0));
            }
        }
        checkAndPushData(m_objPts, corners);
        runCalibration(imgSize);
        showParams();
    }
}
```
可以看到这个方法调用了其他三个方法，这三个方法对应了处理标定坐标数据的四个基本步骤：
1. 将采集到的图像坐标点添加到图像坐标缓存中（ checkAndPushData(m_imgPts, imgPts) ）；
2. 根据配置的棋盘格真实大小，依次计算角点在真实世界的坐标。这里为了方便计算，将真实世界的坐标系原点设定在了棋盘格左上角（即第二行第二列格子的左上角，之后不再赘述）。计算得到的结果将被添加到真实坐标缓存中（ checkAndPushData(m_objPts, corners) )；
3. 读取缓存中所有的坐标，执行标定计算
4. 将标定结果展示到界面上

这几个步骤中最重要的执行标定操作实现如下：
```cpp
void CamCalibrator::runCalibration(const cv::Size& imgSize)
{
    std::vector<cv::Mat> rvecs, tvecs;
    m_cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
    m_distCoeffs = cv::Mat::zeros(8, 1, CV_64F);
    m_rms = cv::calibrateCamera(m_objPts, m_imgPts,
                                imgSize, m_cameraMatrix, m_distCoeffs, rvecs, tvecs);
}
```
也就是使用了OpenCV提供的calibrateCamera方法。标定的计算结果将缓存到m_cameraMatrix和m_distCoeffs变量中，同时标定结果的误差参考值会被缓存到m_rms变量里。

而最终标定结果的展示，通过向主界面发射calibrated信号实现，这里就不作展开说明了。

相机标定器的另一个重要功能是标定结果的导出。这里我们使用了OpenCV提供的FileStorage工具，可以很方便地以统一的存储格式实现标定数据的读写，具体如下：
```cpp
void CamCalibrator::saveResults(QString fileName)
{
    cv::FileStorage fs(fileName.toStdString().c_str(), cv::FileStorage::WRITE);
    fs << "calibration_time" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toStdString().c_str();
    fs << "nframes" << (int) m_objPts.size();
    fs << "rms" << m_rms;
    fs << "camera_matrix" << m_cameraMatrix;
    fs << "distortion_coefficients" << m_distCoeffs;
}
```
使用这个方法存储得到的标定数据格式如下：
```json
{
    "calibration_time": "2018-03-08 19:21:13",
    "nframes": 11,
    "camera_matrix": {
        "type_id": "opencv-matrix",
        "rows": 3,
        "cols": 3,
        "dt": "d",
        "data": [ 8.4446356735928168e+02, 0.0, 3.1252015703842346e+02,
            0.0, 8.3814268020345060e+02, 2.5160161633730544e+02, 0.0,
            0.0, 1.0 ]
    },
    "distortion_coefficients": {
        "type_id": "opencv-matrix",
        "rows": 5,
        "cols": 1,
        "dt": "d",
        "data": [ 2.8536755267484618e-02, 8.9133754139324750e-01,
            9.8503195767350336e-04, 5.1203070506036933e-03,
            -3.0186480045338673e+00 ]
    }
}
```

## 界面实现
完成了两个核心功能模块的开发，最后我们通过界面将这些功能串起来。
整体界面设计如下：

其中左侧嵌入视频流控件，右上提供棋盘格配置的入口，右下为标定的各类操作按钮和标定结果展示的控件。具体界面布局相关的实现细节就不详细说明了，这里列举几个关键的实现：
首先是在视频流控件中添加角点检测滤波器，并将棋盘格行列数设置控件与滤波器提供的设置入口绑定：
```cpp
    QString edgePtFilterName = "EdgePt";
    QCvEdgePtFilter* filter = new QCvEdgePtFilter(edgePtFilterName);
    int boardCol = ui->editBoardCol->text().toInt();
    int boardRow = ui->editBoardRow->text().toInt();
    filter->setColCnt(boardCol);
    filter->setRowCnt(boardRow);
    filter->setEnabled(true);

    m_camView->appendFilter(filter);
    connect(ui->editBoardCol, &QLineEdit::textChanged, this, [this, filter](QString newCol) {
        filter->setColCnt(newCol.toInt());
    });
    connect(ui->editBoardRow, &QLineEdit::textChanged, this, [this, filter](QString newRow) {
        filter->setRowCnt(newRow.toInt());
    });
```
与此类似，将棋盘格真实宽度的设置控件与相机标定器的设置入口绑定：
```cpp
    CamCalibrator* calibrator = new CamCalibrator(this);
    calibrator->setSquareWidth(ui->editSquareWidth->text().toFloat());
    calibrator->setSquareHeight(ui->editSquareHeight->text().toFloat());
    connect(ui->editSquareHeight, &QLineEdit::textChanged, this, [this, calibrator](QString newH) {
        calibrator->setSquareHeight(newH.toFloat());
    });
    connect(ui->editSquareWidth, &QLineEdit::textChanged, this, [this, calibrator](QString newW) {
        calibrator->setSquareWidth(newW.toFloat());
    });
```
然后将滤波器发送数据的信号与标定器接收数据的槽连接起来：
```cpp
connect(filter, &QCvEdgePtFilter::imgPtsCaptured, calibrator, &CamCalibrator::onImgPts);
```
在点击采集按钮时，触发滤波器的采集操作：
```cpp
connect(ui->btnCapture, &QPushButton::clicked, this, [this, filter](){
       ... 
       // 省略界面控件和布局相关操作
       filter->onCapture();
    });
```
最后，将存储参数按钮绑定到标定器提供的存储方法：
```cpp
connect(ui->btnSave, &QPushButton::clicked, this, [this, calibrator](){
        m_camView->onStreamSwitch(false);
        QString timeStample = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
        QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save Calibration Results"),
                                                            QString("./%1.json").arg(timeStample),
                                                            tr("Config Files(*.json *.xml *.yml);;All Files(*)"));
        if (!saveFileName.isEmpty())
        {
            calibrator->saveResults(saveFileName);
        }
        m_camView->onStreamSwitch(true);
    });
```

## 测试标定效果
使用实现完成的标定工具测试效果如下图：

*注：因为笔者这里使用的是用硬纸板自制的简易标定版，所以计算得到的结果会有比较大的误差。实际场景中使用精度更高的标定板可以得到更精确的标定结果。*

有关相机标定工具的实现就说明到这里。下一节我们将使用这些标定参数实现一个较为复杂的应用。

[>>返回系列索引](http://www.jianshu.com/p/80762f0e9d00)


## 参考链接
[\[1\] OpenCV官方相机校准说明文档][1]   

[1]:https://docs.opencv.org/3.0-beta/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html