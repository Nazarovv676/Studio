#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QPalette>

class Settings
{
public:
    static QSettings settings;

    static QPalette greyPalette;//палитры для приложения
    static QPalette blackPalette;
    static QPalette whitePalette;

    Settings();

    static void updatePalette();
};

#endif // SETTINGS_H
