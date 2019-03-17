# OpenCV拾趣（五）——扩展视频控件
## 本篇简介
在上一节中，我们实现了具备基本功能的视频控件，并以此为基础实现了一个简易的拍照小工具。

[>>点击这里回顾上一节内容](http://www.jianshu.com/p/d5a2a7b19522)

在本节中，我们将对视频控件进行扩展，使其具备图像滤波和标定信息的管理功能。

## 添加视频滤波管理

在实际应用中比较常见的一种应用场景是：对采集到的图像帧进行处理，并将处理的结果实时展示在所采集的帧上。这时，我们在[第三节](https://www.jianshu.com/p/10886232f5e8)中设计的MatFilter接口就可以发挥作用了——在基础视频控件中添加视频滤波管理相关的功能，即可很方便地实现对所采集图像帧的实时处理和展示。秉承第三节的设计思路，视滤波的管理大致包含下面的功能：
- 提供滤波器的容器，即第三节中提到过的*滤波链*，采集到的视频帧将按照视频链的顺序以此进滤波，并将最终结果显示在控件上；
- 添加、启用、禁用特滤波器的接口。

依照上面的思路，在视频控件类QCvCamView中添加如下声明：
```cpp
public:
    void appendFilter(QCvMatFilter* filter);
    void setFilterEnabled(QString name, bool enabled);

protected:
    virtual void execFilters(cv::Mat& inMat, cv::Mat& outMat);

private:
    QList<QCvMatFilter*> m_filters;
```
具体实现如下：
### 添加Filter
滤波链的末端添加一个Filter：
```cpp
void QCvCamView::appendFilter(QCvMatFilter* filter)
{
    filter->setParent(this);
    m_filters.append(filter);
}
```
这里将新添加的滤波器parent设为本对象，利用Qt自身的对象回收机制，确保生成的滤波器对象内存在控件销毁时也会被释放。
### 启用/禁用某滤波器
```cpp
void QCvCamView::setFilterEnabled(QString name, bool enabled)
{
    foreach (QCvMatFilter* filter, m_filters)
    {
        if (filter->name() == name)
        {
            filter->setEnabled(enabled);
        }
    }
}
```
也就是从滤波链中找出名称(name)相同的滤波器并设置使能(enable)标志位。
### 执行视频帧滤波
```cpp
void QCvCamView::execFilters(const cv::Mat& inMat, cv::Mat& outMat)
{
    outMat = inMat.clone();
    foreach (QCvMatFilter* filter, m_filters)
    {
        filter->filter(outMat, outMat);
    }
}
```
然后在控件的绘制事件(paintEvent）里，在VideoCapture对象读取了当前帧之后，调用上面的execFilters方法即可实现视频帧滤波的执行。

视频帧的滤波管理功能到这里就实现完成了。

## 添加标定数据管理
在需要与现实场景进行交互的应用中，例如在进行AR绘制、3D场景重建时，作为连接现实世界与图像世界的纽带，相机的参数起着至关重要的作用。因此，我们下一步的扩展即为添加相机标定数据的管理。
相机标定数据可分为两类：
- 内部参数(intrinsic parameters)：包括相机的焦距、相平面中心坐标和扭曲参数等相机本身属性参数。
- 外部参数(extrinsic parameters)：相机相对世界坐标系的位置信息，包括相对于三个坐标轴的旋转和相对于原点的平移。
基于此，我们首先实现一个封装了这些信息的数据类QCvCamera，其中定义了内参和外参两种数据结构：
```cpp
// 内部参数
struct CameraIntrinsic
{
    cv::Mat cameraMat;
    cv::Mat distortCoeff;
};

// 外部参数
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
```
其中，内参分为相机矩阵cameraMat和扭曲参数向量distortCoeff，是为了方便使用OpenCV本身提供的标定功能。
基于此,相机标定数据的数据类声明如下：
```cpp
class QCvCamera : public QObject
{
    Q_OBJECT
  public:
    explicit QCvCamera(QObject* parent = nullptr);

  public:
    void setIntrinsic(const CameraIntrinsic& intrinsic);
    const CameraIntrinsic& intrinsic() const;
    const QList<Pose>& poses() const;
    bool isIntrinsicValid() const;
    bool loadCalibrationData(const QString& fileName);

  public slots:
    void addPose(const cv::Mat& rotation, const cv::Mat& translation);

  private:
    CameraIntrinsic m_intrinsic;
    QList<Pose> m_poses;
```
简单说明一下：
- 对于同一个相机而言，不考虑老旧损坏等特殊情况的话，内参数据是基本不会发生变化的；而随着相机位置和角度的调整，外参会经常发生变化，因此这里提供了一个存放多组外参的列表作为这个数据类的成员组件；
- 除了基础的getter和setter之外，还有一个从文件中读取标定信息的方法：
```cpp
bool QCvCamera::loadCalibrationData(const QString& fileName)
{
    if (!fileName.isEmpty())
    {
        cv::FileStorage fs(fileName.toStdString().c_str(), cv::FileStorage::READ);
        if (fs.isOpened())
        {
            cv::Mat camMat = cv::Mat_<double>::zeros(3, 3);
            cv::Mat distortMat = cv::Mat_<double>::zeros(5, 1);
            cv::FileNode camMatNode = fs["camera_matrix"];
            if (!camMatNode.empty() && camMatNode.isMap())
            {
                camMatNode >> camMat;
            }
            cv::FileNode distortNode = fs["distortion_coefficients"];
            if (!distortNode.empty() && distortNode.isMap())
            {
                distortNode >> distortMat;
            }
            if (!camMat.empty() && !distortMat.empty())
            {
                m_intrinsic.cameraMat = camMat;
                m_intrinsic.distortCoeff = distortMat;
                return true;
            }
            else
            {
                qWarning() << "Intrinsic config data not valid!";
                return false;
            }
        }
        else
        {
            qWarning() << "Could not open intrinsic config file!";
            return false;
        }
    }
    else
    {
        qWarning() << "Invalid file name!";
        return false;
    }
}
```
这里使用了OpenCV的持久化工具FileStorage，借助它可以很方便地将cv::Mat格式的数据存储到xml、yml或json格式的文件中，或从相应格式的文件中将数据加载为cv::Mat格式。上面的实现中从标定文件里获取名为"camera_matrix"和"distortion_coefficients"的矩阵作为相机矩阵和扭曲参数向量。

将标定数据类封装完成后，向视频控件添加对应的管理功能实际很简单：将标定数据类以成员组件的形式添加到控件类中。考虑到对图像帧的处理大多在各类滤波器中，因此可以在滤波器的基类QCvMatFilter中也添加标定数据类，以便在进行视频滤波时可以直接获取到相机的标定数据。

基于这个思路，首先在QCvMatFilter类中添加QCvCamera成员组件和setter方法（setCamera方法），这里就不详细展示了；

然后在QCvCamView初始化时创建一个QCvCamera对象，并在appendFilter方法中，为每个新添加进来的滤波器设置标定组件（调用filter的setCamera方法）;

最后,为视频控件类添加更新标定信息的updateCalibrarion()方法，传入的参数为标定文件的文件名：
```cpp
bool QCvCamView::updateCalibrarion(QString fileName)
{
    if (!fileName.isEmpty())
    {
        return m_camera->loadCalibrationData(fileName);
    }
    else
    {
        return false;
    }
}
```
因为所有滤波器使用的都是和视频控件相同的标定数据对象，所以上面这个方法里标定数据的更新会传递到所有的滤波器中。

如此，相机标定数据的管理功能就实现完成了。对于视频控件的功能扩展也就先进行到这里。

## 基础框架搭建小结
至此，本系列基础框架的搭建也基本告一段落了。

为了让框架搭建的解说更为直观，除了本节之外，每一节都附上了一个简单的实现案例。本节限于篇幅，就不进行具体使用案例的讲解了，在simpleCV仓库的[demos/QCvCommonFilters](https://github.com/KevinEmiya/simpleCV/tree/master/demos/QCvCommonFilters)项目中实现了一些较为通用的滤波器，包括高斯模糊和直方图适配等，有兴趣的朋友可以自行将这些滤波器添加到控件的滤波链中查看滤波效果。

接下来，我们将开始这个系列的第二部分，使用我们搭建的这个基础框架，实现一些较为复杂的案例。

[>>返回系列索引](http://www.jianshu.com/p/80762f0e9d00)