//
// Created by user on 16.02.2023.
//

#include "MsecTicker.h"

MsecTicker::MsecTicker() :
        mTimeFormat(QLatin1String("mm:ss:zzz"))
{
}

void MsecTicker::setTimeFormat(const QString& format)
{
    mTimeFormat = format;
}

QString MsecTicker::getTickLabel(double tick, const QLocale& locale, QChar formatChar, int precision) {
    if(tick < 0) return QString("00:00:000");
    return QDateTime::fromMSecsSinceEpoch(tick).toUTC().toString(mTimeFormat);
}