# OpenCV拾趣（四）——视频显示控件基础功能
## 本篇简介
在上一节中，我们设计了便于后续开发的MatFilter接口，并以此为基础实现了基于Canny边缘检测的边缘提取。

[>>点击这里回顾上一节内容](http://www.jianshu.com/p/10886232f5e8)

本篇将借助OpenCV的视频采集功能设计一个便于Qt框架使用的视频显示控件。

# 设备概况
首先简单说明一下接下来的范例使用的设备和一些假定：
- 笔者使用的是罗技的C310 USB网络摄像头，根据官方的规格说明<sup>[\[1\]][1]</sup>，该款摄像头的分辨率为1280 x 720 px，最大fps为30；
- 接下来的开发如无特殊说明，均假定应用环境中**只连接了一台摄像头**。

在Ubuntu系统下，通过*lsusb*命令，可以看到如下设备信息：
```bash
Bus 003 Device 002: ID 046d:081b Logitech, Inc. Webcam C310
```
如果连接了多台摄像头，可以借助这个命令查看对应设备的编号，并对下面的实现进行调整。对于多台摄像头的情况，本篇暂不作详细讨论。

# 封装视频采集接口
OpenCV的VideoCapture模块<sup>[\[2\]][2]</sup>提供了十分简便的视频采集接口。在假定只连接了一台摄像头的情况下，使用下面三个方法即可实现摄像头的开启、读取和关闭：
```cpp
# 开启默认相机
bool VideoCapture::open(0)
# 读取当前帧
bool VideoCapture::read(OutputArray image)
# 关闭相机
void VideoCapture::release()
```
以这三个方法为基础，可以封装一个具备基本视频采集和显示功能的Qt控件QCvCamView，声明如下：
```cpp
class QCvCamView : public QWidget
{
    Q_OBJECT
public:
    explicit QCvCamView(QWidget *parent = 0);
    virtual ~QCvCamView();

public:
    // 获取当前帧
    cv::Mat currentFrame();

signals:
    // 开启相机错误
    void camOpenError();
    // 采集帧为空错误
    void emptyFrameError();
    // 设置fps错误
    void invalidFpsError();

public slots:
    // 开启/关闭视频流
    void onStreamSwitch(bool open);
    // 开启/关闭渲染
    void onRenderSwitch(bool open);
    // 设置fps
    void onFpsChanged(int fps);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    cv::VideoCapture* m_cap;
    bool m_isRendering;

    cv::Mat m_frame; //当前帧
    int m_fps;
    QTimer* m_updateTimer;
};
```
也就是将VideoCapture对象作为组件封装在这个控件类内，并对外提供各类错误信号和控制接口。这里详细说明一下比较重要的几个接口：

## 1. 视频流的开启和关闭：   
实现为槽，方便与外部信号进行连接。如果相机开启失败，发射开启错误信号。具体实现如下：
```cpp
void QCvCamView::onStreamSwitch(bool open)
{
    if (open && !m_cap->isOpened()) //open the camera
    {
        bool ret = m_cap->open(0);
        if (!ret || !m_cap->isOpened())
        {
            emit camOpenError();
            return;
        }
        update();
    }
    else if (!open && m_cap->isOpened()) //close the camera
    {
        m_cap->release();
        update();
    }
}
```
## 2. 渲染的开启和关闭：   
并非物理关闭摄像头，而是仅仅停止最新帧渲染的接口。实现这个接口是为了方便处理本控件的paint event和其他控件产生冲突时，暂停本控件显示的情况。因为具体的渲染实现在重载的paint event方法中，所以这里只需要设置一个标志位即可，实现如下：
```cpp
void QCvCamView::onRenderSwitch(bool open)
{
    m_isRendering = open;
}
```
## 3. 重载绘制事件：
通过重载控件的绘制事件，绘制最新采集到的视频图像——检查上面的渲染开启标志位和相机的开启状态，判定是否需要更新绘制。实现如下：
```cpp
void QCvCamView::paintEvent(QPaintEvent* event)
{
    if (m_isRendering && m_cap->isOpened())
    {
        QPainter painter(this);
        m_cap->read(m_frame);
        if (!m_frame.empty())
        {
            painter.setRenderHints(QPainter::Antialiasing, true); //抗锯齿
            painter.scale(rect().width() * 1.0 / m_frame.cols, rect().height() * 1.0 / m_frame.rows);
            painter.drawImage(0, 0, QCvDataUtils::cvMatToQImage(m_frame));
        }
        else
        {
            emit emptyFrameError();
        }
    }
}
```
## 4. FPS控制：
向控件中添加定时器（m_updateTimer），在定时器timeout时触发控件刷新：
```cpp
m_fps = 30;
m_updateTimer = new QTimer(this);
m_updateTimer->start(1000 / m_fps);
connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
```
在此基础上提供调整刷新频率的接口，即可实现对FPS的控制：
```cpp
void QCvCamView::onFpsChanged(int fps)
{
    if (fps <= 0)
    {
        emit invalidFpsError();
        return;
    }

    m_updateTimer->stop();
    m_fps = fps;
    update();
    m_updateTimer->start(1000 / fps);
}
```
当然，这里的控制无法突破设备本身的上限，例如笔者使用的设备最高FPS为30，因此设置为30以上的FPS没有实际意义。

# 实现简易拍照工具
如此，一个具备基本视频采集功能的控件就实现完成了。接下来我们试着使用上面的控件，实现一个简易的拍照工具。实现完成的效果如下图：

首先确定一下这个拍照工具的基本功能：
- 主对话框包含：
  - 一个QCvCamView控件用于显示实时采集的视频
  - 一个相机开关的按钮(btnOpenCam)
  - 一个拍照按钮（btnCap)
  - 一个调整渲染FPS的输入控件(fpsEdit)
  - 用于显示错误信息的状态标签（statusLabel）

- 在点击拍照按钮后，弹出一个预览对话框，并提供存储(btnSave)和放弃(btnAbort)两个按钮。点击存储按钮后，弹出保存文件对话框。

依照上面的思路，首先实现主对话框，在对话框初始化时生成QCvCamView对象，并连接相关的信号与槽：
```cpp
QStackedLayout* layout = new QStackedLayout(ui->frame);
m_camView = new QCvCamView(ui->frame);
m_camView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
layout->addWidget(m_camView);

// 将相机开关按钮的点击信号与视频控件的视频流开关槽连接
connect(ui->btnOpenCam, &QPushButton::clicked, m_camView, &QCvCamView::onStreamSwitch);
// 将相机开关按钮的点击信号与由本对话框自身提供的槽连接，
// 用于处理本对话框内的一些显示逻辑
connect(ui->btnOpenCam, &QPushButton::clicked, this, &CamDlg::onBtnOpenClicked);
// 将采集按钮与本对话框的采集槽连接
connect(ui->btnCap, &QPushButton::clicked, this, &CamDlg::onCapFrame);
// 将视频控件的错误信号与本对话框提供的错误信息处理槽连接
connect(m_camView, &QCvCamView::camOpenError, this, &CamDlg::onCamOpenError);
connect(m_camView, &QCvCamView::emptyFrameError, this, &CamDlg::onEmptyFrameError);

// 初始化FPS控件
QRegExp fpsReg("^100|[1-9]{0,1}[0-9]$"); //将可填入的数值限制在0-100之间
ui->fpsEdit->setValidator(new QRegExpValidator(fpsReg, this));
// 连接FPS控件编辑信号与功能槽
connect(ui->fpsEdit, SIGNAL(textChanged(QString)), this, SLOT(onFpsChanged(QString)));
```
这里的ui->frame是预留给视频控件的容器。可以看到只需要将开关按钮的点击事件和视频控件的视频流开关槽连接，就能很方便地实现对视频流的控制。这里的onBtnOpenClicked、onCamOpenError、onEmptyFrameError三个槽只实现了简单的状态标签文字变更，就不进一步详细展示了，在这里重点说一下onFpsChanged和onCapFrame这两个槽的实现：
```cpp
void CamDlg::onFpsChanged(QString fpsStr)
{
    if(!fpsStr.isEmpty())
    {
        m_camView->onFpsChanged(fpsStr.toInt());
    }
}
```
可以看到FPS控制槽只是简单地将QString字串转化为了整形发送给了视频控件的FPS控制槽。
```cpp
void CamDlg::onCapFrame()
{
    cv::Mat currentFrame = m_camView->currentFrame();
    if(!currentFrame.empty())
    {
        m_camView->onRenderSwitch(false);
        m_capDlg->show();
        m_capDlg->setFrame(currentFrame);
    }
    else
    {
        ui->statusLabel->setText("Current frame is empty! Please try later...");
        m_statusTimer->start(5000);
    }
}
```
其中m_capDlg为拍照预览对话框的实例，作为成员变量以组件的形式添加到主对话框内。借助视频控件的currentFrame()方法获取当前采集到的帧，并将其设置到预览对话框内。预览对话框细节上的实现在这里就不多赘述了，这里主要看两个方法，首先是上面onCapFrame（）槽中调用的设置预览帧的方法setFrame（）：
```cpp
void CapDlg::setFrame(const cv::Mat& frame)
{
    QImage img = QCvDataUtils::cvMatToQImage(frame);
    ui->labelImg->setPixmap(QPixmap::fromImage(img).scaled(ui->labelImg->width(),
                                                           ui->labelImg->height()));
    m_currentFrame = frame;
}
```
可以看出这里的实现比较简单，一方面使用了第二节中实现的cv::Mat和QImage的转化工具，很方便地就完成了当前帧在界面上的显示；另一方面将预览帧的原始数据缓存到成员变量m_currentFrame中，**以确保存储的图片质量不受显示控件大小的影响**。   
最后就是保存预览帧的方法实现了：
```cpp
void CapDlg::onBtnSave()
{
    QString timeStample = QDateTime::currentDateTime()
                            .toString("yyyyMMddHHmmss");
    QString saveFileName = QFileDialog::getSaveFileName(this, 
                                    tr("Save Current Frame"), 
                        QString("./%1.jpg").arg(timeStample), 
                    tr("Images(*.png *.jpg);;All Files (*)"));
    if (!saveFileName.isEmpty())
    {
        QImage img = QCvDataUtils::cvMatToQImage(m_currentFrame);
        img.save(saveFileName);
        hide();
        emit closed();
    }
}
```
简而言之就是调用了QFileDialog自带的保存文件对话框，然后将缓存的预览帧保存为文件即可。拍照预览界面的最终实现效果如下：

本节有关视频采集控件基础功能的实现就先介绍到这里。（[>>点击这里查看完整代码实现](https://github.Com/KevinEmiya/simpleCV/tree/master/demos/CameraViewer))

下一节将在此基础上为控件添加视频帧过滤和相机标定信息的管理功能。

[>>返回系列索引](http://www.jianshu.com/p/80762f0e9d00)

# 参考链接：
[\[1\] 罗技C310规格说明][1]   
[\[2\] OpenCV VideoCapture Class Reference][2]

[1]:https://www.logitech.com.cn/zh-cn/product/hd-webcam-c310#specification-tabular
[2]:https://docs.opencv.org/3.3.1/d8/dfe/classcv_1_1VideoCapture.html#details