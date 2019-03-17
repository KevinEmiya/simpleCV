# OpenCV拾趣（六）——视频流人脸跟踪
## 本篇简介
从本小节开始，我们将尝试使用基础框架搭建篇完成的各类工具，实现一些实战向的应用场景。  
本篇作为实战篇的第一个案例，首先来看看如何实现视频流人脸跟踪这个较为常见的功能。

## 原理简介
实现对人脸时别/跟踪，需要解决的主要问题就是将人脸的部分与场景中其他的部分区分开来。这就涉及到两方面的问题：   
1. 如何提取人脸的特征
2. 如何使用提取的特征对场景中的对象进行分类

对此，一种常见的实现方式为Haar特征检测提取特征 + 级联分类器（Cascading Classifier）进行分类的组合。而OpenCV中也提供了相应的分类器训练及分类工具。关于Haar特征提取和级联分类器的原理，在这里就不详细说明了，有兴趣的话可以参考文末参考连接中的相关资料[\[1\]][1]和[\[2\]][2]。

此外，简明起见，接下来的实现将只关注如何使用已训练好的分类器进行人脸检测，如何对分类器进行训练不在本文的讨论范围内。

那么接下来我们就来看一看如何使用我们实现好的QCvCamView控件和OpenCV提供的分类器工具实现视频流人脸跟踪。

## 实现人脸检测滤波器
首先，秉承QCvCamView中通过*滤波链*来实现视频流处理的思路（参考：[扩展视频流控件](https://www.jianshu.com/p/c39bd9d8cfaa)），我们需要实现一个人脸检测滤波器，声明如下：
```cpp
class QCvFaceDetectFilter : public QCvMatFilter
{
  public:
    QCvFaceDetectFilter(QString name) : QCvMatFilter(name) {}

  public:
    bool load(QString fileName);
    bool isClassifierValid();

  protected:
    virtual void execFilter(const cv::Mat& inMat, cv::Mat& outMat);

  protected:
    cv::CascadeClassifier m_classifier;
};
```
这个滤波器除了实现了滤波器通用的execFilter方法外，还提供了读取分类器配置和判断分类器是否有效的公共方法。OpenCV提供的级联分类器作为组件以成员变量的形式在这里声明。   
*注：要使用这个分类器，需要引用opencv2/objdetect.hpp头文件*

读取分类器配置的具体实现如下：
```cpp
bool QCvFaceDetectFilter::load(QString fileName)
{
    return m_classifier.load(fileName.toStdString().c_str());
}
```
也就是直接使用了OpenCV提供的读取方法。

滤波处理的具体实现如下：
```cpp
void QCvFaceDetectFilter::execFilter(const cv::Mat& inMat, cv::Mat &outMat)
{
    outMat = inMat.clone();
    if (m_classifier.empty())
    {
        return;
    }

    std::vector<cv::Rect> faces;
    cv::Mat frameGray;
    cv::cvtColor(outMat, frameGray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(frameGray, frameGray);
    //-- Detect faces
    m_classifier.detectMultiScale(frameGray, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE,
                                  cv::Size(30, 30));
    for (size_t i = 0; i < faces.size(); i++)
    {
        cv::Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
        cv::ellipse(outMat, center,
                    cv::Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360,
                    cv::Scalar(255, 0, 255), 4, 8, 0);
    }
}
```
上面的实现中，首先借助cvtColor方法将RGB图像转化为灰度图像，并通过直方图匹配（eualizeHist）对灰度图像进行了预处理，以提升光线较暗时识别的准确率。   
之后，将预处理后的灰度图像传入级联分类器的多级检测方法（detectMultiScale）进行分类，并将结果矩形保存到faces数据中。   
最后，在RGB图像上将检测到的人脸结果绘制为椭圆形。这样基于级联分类器的人脸检测滤波器就实现完成了。

*注：这里的实现参考了[OpenCV官方级联分类器教程][3]中的实现，并进行了一定简化。*

## 界面实现

整体界面设计如下：

灰色部分即为嵌入的QCvCamView控件，下方两个按钮分别为加载分类器的按钮和开始人脸检测的按钮。其中开始检测的按钮在没有完成分类器加载前处于不可用状态。

基于上述界面设计，我们初始化QCvCamView控件，并将上面实现好的人脸检测滤波器添加到滤波链中：
```cpp
    m_camView = new QCvCamView(ui->frame);
    m_faceFilter = new QCvFaceDetectFilter("face");
    m_camView->appendFilter(m_faceFilter);
```
然后将配置好的视频流控件添加到布局中即可。这里限于篇幅就不贴布局相关的代码了。   
接下来我们来实现两个功能按钮。首先是加载分类器的按钮，比较关键的实现步骤如下：
```cpp
void FaceVideoDlg::onBtnLoad()
{
    // 省略界面布局及控件相关的操作
    ...

    QString dataDir = QDir::currentPath() + "/../../../../sdk/opencv_release/share/OpenCV/haarcascades";
    if (!QDir(dataDir).exists())
    {
        dataDir = ".";
    }
    QString classifierName = QFileDialog::getOpenFileName(this,
                                                          "Please Select a Cascade Classifier",
                                                          dataDir,
                                                          "XML Files (*.xml)");
    if (!classifierName.isEmpty())
    {
        bool isValid = m_faceFilter->load(classifierName);
        // 省略界面布局及控件相关的操作
        ...
    }
    else
    {
        // 省略界面布局及控件相关的操作
        ...
    }
}
```
上面的实现分为三个步骤：
- 读取分类器。在上面的实现中，默认路径指向了OpenCV官方提供的训练结果样例路径（参考[第一节](https://www.jianshu.com/p/1d42805dfb02)有关SDK目录的说明），方便后续测试。
- 加载分类器。这个步骤委托给了人脸检测滤波器处理。
- 根据加载结果，执行各控件的启用/禁用操作。

另一方面，开始人脸检测的按钮实现就相对比较简单了，只需要启动视频流控件即可：
```cpp
void FaceVideoDlg::onBtnDetect(bool clicked)
{
    m_camView->onStreamSwitch(clicked);
}
```

## 测试检测效果
整个界面实现完成后，我们来测试下检测的效果。首先点击加载分类器按钮，选择官方样例中的haarcascade_frontalface_alt2.xml训练器，然后点击开始检测按钮，效果如下：

有关视频流人脸检测的实现就说明到这里。

[>>返回系列索引](http://www.jianshu.com/p/80762f0e9d00)


## 参考链接
[\[1\] Haar-like Feature Wikipedia][1]   
[\[2\] Cascading Classfier Wikipedia][2]  
[\[3\] OpenCV官方级联分类器教程][3]  

[1]:https://en.wikipedia.org/wiki/Haar-like_feature
[2]:https://en.wikipedia.org/wiki/Cascading_classifiers
[3]:https://docs.opencv.org/3.3.1/db/d28/tutorial_cascade_classifier.html