#include "MainWin.h"
#include <QApplication>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
//    MainWin w;
//    w.show();

//    return a.exec();

    cv::Mat img = cv::imread("/home/kevin_yang/图片/3675083.png");
    cv::namedWindow("test");
    cv::imshow("test", img);
    cv::waitKey();
}
