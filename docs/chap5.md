# OpenCV拾趣（五）——视频帧过滤和相机标定信息管理



# 添加视频帧过滤管理
在实际应用中比较常见的一种应用场景是：对采集到的图像帧进行处理，并将处理的结果实时展示在所采集的帧上。这时，我们在上一节中设计的MatFilter接口就可以发挥作用了——在上面实现的基础控件中添加视频帧过滤管理相关的功能，即可很方便地实现对所采集图像帧的实时处理和展示。

首先，在类声明中添加如下声明：
```cpp
public:
    void appendFilter(QCvMatFilter* filter);
    void setFilterEnabled(QString name, bool enabled);

protected:
    virtual void execFilters(cv::Mat& inMat, cv::Mat& outMat);

private:
    QList<QCvMatFilter*> m_filters;
```
也就是添加了一个过滤链和对应的管理方法。具体实现和说明如下：
## 1. 添加Filter
在过滤链的末端添加一个Filter：
```cpp
void QCvCamView::appendFilter(QCvMatFilter* filter)
{
    filter->setParent(this);
    m_filters.append(filter);
}
```