//
// Created by user on 31.01.2023.
//

#ifndef PROTOSLOGVIEWER_MAIN_H
#define PROTOSLOGVIEWER_MAIN_H

#define LogVieverColor_white QRgb(0xBBBBBB)
#define LogVieverColor_dark QRgb(0x45494A)
#define LogVieverColor_middle QRgb(0x545657)
#define LogVieverColor_middle2 QRgb(0x7C7E80)
#define LogVieverColor_grid QRgb(0x616161)
#define LogVieverColor_subgrid QRgb(0x4F5152)
#define LogVieverColor_bottomTick QRgb(0xE3E6E8)
#define LogVieverColor_selectedGraph QRgb(0xbd8e3e)

namespace LogViewerItems{
    enum Type: int{
        Regular,
        Container,
        Root
    };
    enum KeyType: int{
        DateTime = 0,
        mSecs = 1,
        Count = 2
    };

    inline static double prepareKeyData(const QVariant& data, KeyType keyType){
        switch (keyType) {
            case DateTime:
                return QCPAxisTickerDateTime::dateTimeToKey(data.toDateTime().toUTC());
            case mSecs:
            case Count:
            default:
                return data.toDouble();
        }
    }
}

#endif //PROTOSLOGVIEWER_MAIN_H
