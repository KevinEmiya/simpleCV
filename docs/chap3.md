# OpenCV拾趣（三）——实现MatFilter接口
## 本篇简介
在上一节中，我们借助编译好的SDK实现了Qt和OpenCV间图片数据转化的工具，并以此为基础实现了一个简单的图片浏览工具。

[>>点击这里回顾上一节内容](http://www.jianshu.com/p/404d439aecc3)

本篇将继续添加便于后续开发的工具——MatFilter接口，并基于这个接口向上一节的图片浏览工具中添加边缘提取功能。

## 设计MatFilter接口
有了上一节的格式转化工具后，我们接下来的开发可以将关注点尽量放在OpenCV原生的图片数据结构cv::Mat上，避免在使用OpenCV提供的功能接口时反复进行QImage和cv::Mat之间的转化。

对此，我们可以设计一个通用的MatFilter接口，将图片数据的处理过程抽离出来，实现视图(View)和控制(Controller)的分离。此外，不同的控制逻辑之间也应该能够彼此分离，便于代码的管理和拓展——因此，不同的Filter应该支持组成一条Filter链，每个Filter都以前一个Filter的输出结果为输入。

基于这些思路，我们可以设计下面这样一个抽象基类QCvMatFilter：
```cpp
class QCvMatFilter : public QObject
{
    Q_OBJECT
  public:
    QCvMatFilter(QString name, QObject* parent = nullptr) : QObject(parent)
    {
        m_enabled = true;
        m_name = name;
    }

  public:
    QString name() { return m_name; }
    void setEnabled(bool enabled) { m_enabled = enabled; }

    void filter(const cv::Mat& inMat, cv::Mat& outMat)
    {
        if (m_enabled)
        {
            execFilter(inMat, outMat);
        }
        else
        {
            outMat = inMat.clone();
        }
    }

  protected:
    virtual void execFilter(const cv::Mat& mat, cv::Mat& outMat) = 0;

  private:
    QString m_name;
    bool m_enabled;
};
```
其中：
- filter()方法即为提供给外部调用的方法，其中调用的虚方法execFilter()，即为子类需要实现的部分；
- 设计两个属性：名字(m_name)和是否启用(m_enabled)是为了方便在Filter链中对特定的Filter进行管理。

## 实现案例：边缘检测
下面我们来实际使用一下上面设计的接口，向上一节实现的图片浏览器中添加提取图像边缘的功能。

首先实现QCvMatFilter的子类QCvEdgeDetectFilter：
```cpp
class QCvEdgeDetectFilter : public QCvMatFilter
{
  public:
    QCvEdgeDetectFilter(QString name, QObject* parent = nullptr);

  public:
    void setThresholds(int thres) { m_thres = thres;}

  protected：
    void execFilter(const cv::Mat& mat, cv::Mat& outMat);

  private:
    int m_thres;
};
```
其中execFilter()方法的具体实现如下：
```cpp
void QCvEdgeDetectFilter::execFilter(const cv::Mat& inMat, cv::Mat &outMat)
{
    // Canny Detector
    cv::Canny(inMat, outMat, m_thres, m_thres * 3);
}
```
可以看到通过使用OpenCV的Canny函数，很方便就实现了边缘检测功能。

这里简单介绍一下Canny边缘检测：

Canny边缘检测算法由澳大利亚科学家John F. Canny在1986年提出，至今仍是图像边缘检测算法中最常用的算法之一。秉承Canny自己提出的最优边缘检测方法特性，Canny边缘检测通过图像平滑、非极大值抑制和双阈值等方式，实现了低错误率、高准确率和高信噪比的边缘检测算法<sup>[\[1\]][1]</sup>。具体的原理在这里就不细说了，可以参考最下面参考链接里的相关内容。

这里需要特别说一下的是有关双阈值的设置。在Canny边缘检测中，设置了高低两个阈值，高阈值用于消除非边缘的噪声点，而低阈值用来更好地将检测到的边缘曲线连接成封闭的图形。官方推荐高低阈值比为1:2到1:3，所以在上面的实现里我们方便起见，直接将高阈值设置为了低阈值的3倍。

*注意：OpenCV的Canny函数会自动比较所输入的两个阈值大小，并将其中较大的作为高阈值使用。*

接下来，在上一节中我们实现的图片浏览器里，加入一个边缘检测按钮（btnEdge)和一个调节阈值的滑条（thresSlider），并实现下面的initFilters()方法，在界面初始化时调用：
```cpp
void ImageDlg::initFilters()
{
    QCvEdgeDetectFilter* filter = 
    new QCvEdgeDetectFilter("canny", this);
    connect(ui->btnEdge, &QPushButton::clicked, this， [this, filter](bool clicked) {
        m_extractingEdge = clicked;
        filter->filter(m_imgMat, m_edgeMat);
        showImage(clicked ? m_edgeMat : m_imgMat);
    });

    connect(ui->thresSlider, &QSlider::sliderMoved, this, [this, filter](int thres) {
        filter->setThresholds(thres);
        if (m_extractingEdge)
        {
            filter->filter(m_imgMat, m_edgeMat);
            showImage(m_edgeMat);
        }
    });
}
```
其中m_edgeMat为新添加的存储边缘图像的cv::Mat类型成员变量。可以看到这里通过调用filter()方法，将边缘检测的处理逻辑封装在了Filter类内部。

最终实现的边缘检测效果如下图所示：

本节有关MatFilter和边缘检测的内容就先介绍到这里。（[>>点击这里查看完整代码实现](https://github.com/KevinEmiya/simpleCV/tree/master/demos/ImageViewer)）

下一节会尝试借助OpenCV的视频捕获功能和本节所设计的MatFilter接口，实现一个Qt的视频显示控件。

[>>返回系列索引](http://www.jianshu.com/p/80762f0e9d00)

## 参考链接
[\[1\] Canny Edge Detector Wiki][1]   
[\[2\] Canny边缘检测及C++实现][2]

[1]:https://en.wikipedia.org/wiki/Canny_edge_detector
[2]:http://blog.csdn.net/dcrmg/article/details/52344902
