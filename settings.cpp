#include "settings.h"

Settings::Settings()
{

}

void Settings::updatePalette()
{
    greyPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    greyPalette.setColor(QPalette::WindowText, Qt::white);
    greyPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    greyPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    greyPalette.setColor(QPalette::Text, Qt::white);
    greyPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    greyPalette.setColor(QPalette::ButtonText, Qt::white);
    greyPalette.setColor(QPalette::BrightText, Qt::red);
    greyPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    greyPalette.setColor(QPalette::Highlight, QColor("#00d5ff"));
    greyPalette.setColor(QPalette::HighlightedText, Qt::black);

    whitePalette.setColor(QPalette::Window, QColor(230, 232, 240));
    whitePalette.setColor(QPalette::WindowText, Qt::black);
    whitePalette.setColor(QPalette::Base, QColor(208, 210, 217));
    whitePalette.setColor(QPalette::AlternateBase, QColor(208, 210, 217));
    whitePalette.setColor(QPalette::Text, Qt::black);
    whitePalette.setColor(QPalette::Button, QColor(196, 197, 204));
    whitePalette.setColor(QPalette::ButtonText, Qt::black);
    whitePalette.setColor(QPalette::BrightText, Qt::red);
    whitePalette.setColor(QPalette::Link, QColor(219, 137, 240));
    whitePalette.setColor(QPalette::Highlight, QColor(120, 255, 241));
    whitePalette.setColor(QPalette::HighlightedText, Qt::black);
}
