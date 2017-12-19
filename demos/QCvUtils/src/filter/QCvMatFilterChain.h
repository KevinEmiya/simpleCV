#ifndef QCVMATFILTERCHAIN_H
#define QCVMATFILTERCHAIN_H

#include <QList>
#include <QObject>
#include <QString>

#include "QCvMatFilter.h"

class QCvMatFilterChain : public QObject
{
    Q_OBJECT
public:
    QCvMatFilterChain(QObject* parent = 0);
    virtual ~QCvMatFilterChain();

public:
    void append(QCvMatFilter* filter);
    void setEnabled(QString filterName, bool enabled);

    cv::Mat execFilter(cv::Mat& mat);

private:
    QList<QCvMatFilter*> m_filters;
};

#endif // QCVMATFILTERCHAIN_H
