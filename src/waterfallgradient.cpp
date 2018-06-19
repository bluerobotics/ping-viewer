#include <QLinearGradient>
#include <QtDebug>

#include "waterfallgradient.h"

PING_LOGGING_CATEGORY(waterfallGradient, "ping.waterfallGradient")

WaterfallGradient::WaterfallGradient(QString name, QList<QColor> colors):
    _name(name)
{
    setColors(colors);
}

void WaterfallGradient::setColors(const QList<QColor>& colors)
{
    float stepSize = 1.0/(colors.length() - 1);
    float step = 0;
    for(const auto& color : colors) {
        setColorAt(stepSize*step++, color);
    }
}

void WaterfallGradient::setName(const QString& name)
{
    _name = name;
}

QString WaterfallGradient::name()
{
    return _name;
}

QColor WaterfallGradient::getColor(float value)
{
    if((value < 0 || value > 1) && !qIsNaN(value)) {
        qCWarning(waterfallGradient) << "Color position must be specified in the range 0 to 1.";
        return QColor();
    }

    auto stopsValues = stops();
    if(stopsValues.length() < 2) {
        qCWarning(waterfallGradient) << "Need more than one stop value.";
        return QColor();
    }

    for(int i(0); i < stopsValues.length() - 1; i++) {
        if(stopsValues[i].first <= value && stopsValues[i+1].first >= value) {
            return colorLinearInterpolation(value, stopsValues[i], stopsValues[i+1]);
        }
    }

    return QColor(0, 0, 0);
}

float WaterfallGradient::getValue(const QColor& color)
{
    if(color.spec() == QColor::Invalid) {
        qCWarning(waterfallGradient) << "Invalid color.";
        return 0.0f;
    }

    auto stopsValues = stops();
    for(int i(0); i < stopsValues.length() - 1; i++) {
        if(colorsInRange(color, stopsValues[i].second, stopsValues[i+1].second)) {
            return valueLinearInterpolation(color, stopsValues[i], stopsValues[i+1]);
        }
    }

    return 0.0f;
}

bool WaterfallGradient::colorsInRange(const QColor& color, const QColor& color1, const QColor& color2)
{
    QColor maxColor;
    maxColor.setRed(color1.red() < color2.red() ? color2.red() : color1.red());
    maxColor.setGreen(color1.green() < color2.green() ? color2.green() : color1.green());
    maxColor.setBlue(color1.blue() < color2.blue() ? color2.blue() : color1.blue());
    QColor minColor;
    minColor.setRed(color1.red() > color2.red() ? color2.red() : color1.red());
    minColor.setGreen(color1.green() > color2.green() ? color2.green() : color1.green());
    minColor.setBlue(color1.blue() > color2.blue() ? color2.blue() : color1.blue());

    bool rOk = color.red() >= minColor.red() && color.red() <= maxColor.red();
    bool gOk = color.green() >= minColor.green() && color.green() <= maxColor.green();
    bool bOk = color.blue() >= minColor.blue() && color.blue() <= maxColor.blue();

    return rOk && gOk && bOk;
}

QColor WaterfallGradient::colorLinearInterpolation(float value, const QGradientStop& color1, const QGradientStop& color2)
{
    float minimum = color1.first;
    float maximum = color2.first;
    float ratio = (value-minimum)/(maximum-minimum);
    float r = ratio*color2.second.red() + (1 - ratio)*color1.second.red();
    float g = ratio*color2.second.green() + (1 - ratio)*color1.second.green();
    float b = ratio*color2.second.blue() + (1 - ratio)*color1.second.blue();

    return QColor(r, g, b);
}

float WaterfallGradient::valueLinearInterpolation(const QColor& color, const QGradientStop& color1, const QGradientStop& color2)
{
    float ratio = 0;
    float value = 0;
    float minimum = 0;
    float maximum = 0;
    float testR = qAbs(color2.second.red() - color1.second.red());
    float testG = qAbs(color2.second.green() - color1.second.green());
    float testB = qAbs(color2.second.blue() - color1.second.blue());
    if(testR >= testG && testR >= testB) {
        maximum = qMax(color2.second.red(), color1.second.red());
        minimum = qMin(color2.second.red(), color1.second.red());
        ratio = float(color.red())/(maximum - minimum);
        if(color2.second.red() < color1.second.red()) {
            ratio = 1 - ratio;
        }
    } else if(testG >= testR && testG >= testB) {
        maximum = qMax(color2.second.green(), color1.second.green());
        minimum = qMin(color2.second.green(), color1.second.green());
        ratio = float(color.green())/(maximum - minimum);
        if(color2.second.green() < color1.second.green()) {
            ratio = 1 - ratio;
        }
    } else if(testB >= testG && testB >= testR) {
        maximum = qMax(color2.second.blue(), color1.second.blue());
        minimum = qMin(color2.second.blue(), color1.second.blue());
        ratio = float(color.blue())/(maximum - minimum);
        if(color2.second.blue() < color1.second.blue()) {
            ratio = 1 - ratio;
        }
    }

    value = ratio*color2.first + (1 - ratio)*color1.first;

    return qMin(qMax(value, 0.0f), 1.0f);
}