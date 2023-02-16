//
// Created by user on 16.02.2023.
//

#ifndef SALOGVIEWER_MSECTICKER_H
#define SALOGVIEWER_MSECTICKER_H

#include <QtCore/QString>
#include <qcustomplot/qcustomplot.h>

class MsecTicker : public QCPAxisTicker{
    Q_GADGET
public:
    MsecTicker();
    QString timeFormat() const {return mTimeFormat;}
    void setTimeFormat(const QString& format);
protected:
    QString mTimeFormat;
    virtual QString getTickLabel(double tick, const QLocale& locale, QChar formatChar, int precision) Q_DECL_OVERRIDE;
};


#endif //SALOGVIEWER_MSECTICKER_H
