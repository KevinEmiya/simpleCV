# OpenCV拾趣（二）——Qt嵌入OpenCV
## 本篇简介
在上一节中，我们完成了OpenCV SDK的构建，本节就来简单使用一下构建好的SDK，并为后续的开发做一些基本的准备。

[>>点击这里回顾上一节内容](http://www.jianshu.com/p/1d42805dfb02)

这个系列后续的开发会使用Qt作为基本开发框架，以充分利用Qt在图形界面上的优势，实现简便快捷的开发。

本节计划达成的目标：

* 在Qt开发环境中引入OpenCV
* 实现OpenCV图像数据到Qt图像数据的转化工具
* 实现一个简单的图片浏览工具

## 搭建开发环境
首先明确一下接下来的例子使用的开发环境：

* 操作系统：Ubuntu 16.04
* OpenCV版本：3.3.1
* Qt版本：5.10.0
* IDE：Qt Creator 4.5.0

Qt Creator作为Qt自家的IDE，对Qt本身的支持是自不用说的。那么如何将OpenCV添加到开发环境里呢？

OpenCV官方很贴心的提供了pkg-config的配置——如果是按照上一节规划的目录结构编译的话，可以在sdk/opencv_release/lib/pkgconfig目录下找到opencv.pc这个文件。而qmake默认的CONFIG选项里就集成了对pkg-config的支持。因此，只需要在项目配置(.pro或.pri文件)中添加OpenCV pkg-config的配置，即可方便地将OpenCV集成到开发环境中。

因为这部分设置是后续很多项目都会用到的公共设置，我们不妨将它提取成一个公共的工具项目QCvUtils，并将项目设置存储为QCvUtils.pri文件。将这个工具项目放置在*与sdk目录平级的demos目录*下，并编写和OpenCV pkg-config相关的配置如下：
```bash
PKG_CONFIG = PKG_CONFIG_PATH=$$PWD/../../sdk/opencv_release/lib/pkgconfig pkg-config
CONFIG  += link_pkgconfig
PKGCONFIG += opencv
```
其中第一行是为了避免每次在新建项目时都必须配置PKG_CONFIG_PATH这个环境变量。如果使用其他的目录规划，将PKG_CONFIG_PATH变量指向到包含opencv.pc文件的对应目录即可。

## 图像数据转化

因为OpenCV和Qt都实现了自己的图像数据结构(cv::Mat和QImage)，因此在使用Qt显示OpenCV的图像数据时，需要对图像数据进行转化。

在接下来的开发中Qt主要作为界面工具，因此不太会出现将处理过的QImage提供给OpenCV API使用的情况。所以，这里仅实现从cv::Mat到QImage的转化。在QCvUtils项目中添加一个工具类QCvDataUtils，并定义一个静态方法：
```cpp
static QImage cvMatToQImage(const cv::Mat& mat);
```

cv::Mat作为OpenCV的基础数据结构之一，提供了大量的矩阵类型(cv::Mat::type），简明起见，这里先实现三种常见图像矩阵类型(8位RGBA、RGB和灰度)的转化：
```cpp
QImage QCvDataUtils::cvMatToQImage(const cv::Mat& mat)
{
    const unsigned char* data = mat.data;
    int width = mat.cols;
    int height = mat.rows;
    int bytesPerLine = static_cast<int>(mat.step);
    switch (mat.type())
    {
        // 8bit, ARGB
        case CV_8UC4:
        {
            QImage image(data, width, height, bytesPerLine,
                         QImage::Format_ARGB32);
            return image;
        }
        // 8bit, BGR
        case CV_8UC3:
        {
            QImage image(data, width, height, bytesPerLine,
                         QImage::Format_RGB888);
            //swap blue and red channel
            return image.rgbSwapped();
        }
        // 8bit, Grayshale
        case CV_8UC1:
        {
            QImage image(data, width, height, bytesPerLine,
                         QImage::Format_Grayscale8);
            return image;
        }
        default:
        {
            // Unsupported format
            qWarning() << "Unsupported cv::Mat tpye: " << mat.type()
                       << ", Empty QImage will be returned!";
            return QImage();
        }
    }
}
```
值得注意的是，在转化8位3通道的图像时，OpenCV和QImage使用的红蓝通道是相反的，需要使用rgbSwapped方法互换一下。

*注：如果使用的是5.5之前的Qt，因为还没有实现Format_Grayscale8这个类型，处理单通道灰度图会复杂一些，具体可以参考[参考链接1](https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/)中的实现。*

## 实现图片浏览

完成数据转化的封装后，接下来实现一个简单的图片浏览工具来测试一下封装好的工具。大致思路如下：
1. 借助Qt的QFileDialog控件对文件进行过滤，并获取用户选中的图片路径；
2. 使用OpenCV的imread加载图片为cv::Mat；
3. 使用QCvDataUtils::cvMatToQImage方法将cv::Mat转化为QImage；
4. 将QImage显示在界面上预留的QLabel区域。

*注：图片浏览功能本身不需要额外借助OpenCV来实现，这里的实现一方面是作为测试，另一方面也是为后面的图像处理研究做准备。*

和Qt界面及控件有关的实现细节就不在这里详细罗列了，这里只展示一下获取到图片路径后，使用OpenCV API加载图片并转化为QImage显示的关键方法showImage：

```cpp
void ImageDlg::showImage(QString imgPath)
{
    ui->imgLabel->setText("");
    cv::Mat imgMat = cv::imread(imgPath.toStdString().c_str());
    QImage img = QCvDataUtils::cvMatToQImage(imgMat);
    ui->imgLabel->setPixmap(QPixmap::fromImage(img));
    resize(width() - ui->imgLabel->width() + img.width(),
           height() - ui->imgLabel->height() + img.height());
}
```
最终实现完成的效果如下图所示：
![scr_img_viewer.png](http://upload-images.jianshu.io/upload_images/9260265-52e89bae886d548b.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

本节有关Qt集成OpenCV的内容就先介绍到这里。（[>>点击这里查看完整代码实现](https://github.com/KevinEmiya/simpleCV/tree/master/demos/ImageViewer)）

下一节会尝设计一个便于后续开发处理的MatFilter接口，并使用这个接口将图像的边缘提取功能添加到本节实现的图片浏览器中。

[>>返回系列索引](http://www.jianshu.com/p/80762f0e9d00)

## 参考链接
1. [Converting Between cv::Mat and QImage or QPixmap](https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/): OpenCV和Qt图像数据相互转化的详细教程