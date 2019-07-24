#include "utils_mac.h"

#include <QApplication>
#include <QPalette>

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

namespace Utils {

// This function was taken and modified from
// https://www.mail-archive.com/interest@qt-project.org/msg31116.html
#ifdef Q_OS_MAC
QColor NSColorToQColor(const NSColor *color)
{
    QColor convertedColor;
    NSString *colorSpace = [color colorSpaceName];
    if (colorSpace == NSDeviceCMYKColorSpace) {
        CGFloat cyan, magenta, yellow, black, alpha;
        [color getCyan:&cyan magenta:&magenta yellow:&yellow black:&black alpha:&alpha];
        convertedColor.setCmykF(cyan, magenta, yellow, black, alpha);
    } else {
        NSColor *tmpColor;
        tmpColor = [color colorUsingColorSpaceName:NSDeviceRGBColorSpace];
        CGFloat red, green, blue, alpha;
        [tmpColor getRed:&red green:&green blue:&blue alpha:&alpha];
        convertedColor.setRgbF(red, green, blue, alpha);
    }
    return convertedColor;
}

QColor getAppleControlAccentColor()
{
    if (@available(macOS 10.14, *)) {
        // macOS 10.14 or later code path
        return NSColorToQColor([NSColor controlAccentColor]);
    } else {
        return QApplication::palette().color(QPalette::Active, QPalette::Highlight);
    }
}
#endif

}
