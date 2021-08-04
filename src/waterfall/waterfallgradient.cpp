#include <QFileInfo>
#include <QLinearGradient>
#include <QRegularExpression>
#include <QtDebug>

#include "waterfallgradient.h"

PING_LOGGING_CATEGORY(waterfallGradient, "ping.waterfallGradient")

WaterfallGradient::WaterfallGradient(const QString& name, const QVector<QColor>& colors)
    : _name(name)
{
    setColors(colors);

    _isOk = colors.length() > 1 && !name.isEmpty();
    if (!_isOk) {
        qCWarning(waterfallGradient) << "Invalid color or name";
    }

    qRegisterMetaType<WaterfallGradient*>("WaterfallGradient*");
}

WaterfallGradient::WaterfallGradient(QFile& file)
{
    /*
        1. Filenames need to have .txt extension.
        2. Filenames will be used as gradient name.
        3. Filenames with underscores will be replaced with spaces.
        4. Lines that do not start with # will not be processed.
        4. Color values need to follow: http://doc.qt.io/qt-5/qcolor.html#setNamedColor
        5. First value will represent 0.0
        6. The last value will represent 1.0
        7. The value of any other color will be 1.0*((color position) - 1)/(number of colors)
    */

    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(waterfallGradient) << "It's not possible to open file:" << file.fileName();
        return;
    }
    QTextStream textStream(&file);
    QVector<QColor> colors;
    int maxNumberOfLines = 50;
    while (!textStream.atEnd() && maxNumberOfLines--) {
        QString line = file.readLine().toLower();
        line = line.remove('\n');
        line = line.remove('\r');
        if (line.isEmpty()) {
            continue;
        }
        qCDebug(waterfallGradient) << "Check line:" << line;
        for (auto i : {2, 5, 7, 8, 11}) {
            /*
                Check for:
                #RGB
                #RRGGBB
                #AARRGGBB
                #RRRGGGBBB
                #RRRRGGGGBBBB
            */
            QRegularExpression regex(QStringLiteral("^#\\S[0-9,a-f]{%1}\\b").arg(i));
            QRegularExpressionMatch match = regex.match(line);
            if (match.hasMatch()) {
                qCDebug(waterfallGradient) << "New color:" << match.capturedTexts();
                colors.append(match.capturedTexts()[0]);
                break;
            }
        }
    }
    const QString name = QFileInfo(file).fileName().split('.')[0].replace('_', ' ');
    qCDebug(waterfallGradient) << "Creating gradient" << name << "with colors:" << colors;
    _isOk = colors.length() > 1 && !name.isEmpty();
    if (!_isOk) {
        qCWarning(waterfallGradient) << "Invalid color or name";
        qCDebug(waterfallGradient) << "Name:" << name << "\tColors:" << colors;
        return;
    }
    setColors(colors);
    setName(name);
}

void WaterfallGradient::setColors(const QVector<QColor>& colors)
{
    float numberOfColors = colors.length() - 1;
    for (int i = 0; i < colors.size(); i++) {
        setColorAt(i / numberOfColors, colors[i]);
    }
}

void WaterfallGradient::setName(const QString& name) { _name = name; }

QString WaterfallGradient::name() const { return _name; }

QColor WaterfallGradient::getColor(float value) const
{
    if ((value < 0 || value > 1) && !qIsNaN(value)) {
        qCWarning(waterfallGradient) << "Color position must be specified in the range 0 to 1.";
        return QColor();
    }

    const auto& stopsValues = stops();
    if (stopsValues.length() < 2) {
        qCWarning(waterfallGradient) << "Need more than one stop value.";
        return QColor();
    }

    for (int i(0); i < stopsValues.length() - 1; i++) {
        if (stopsValues[i].first <= value && stopsValues[i + 1].first >= value) {
            return colorLinearInterpolation(value, stopsValues[i], stopsValues[i + 1]);
        }
    }

    return QColor(0, 0, 0);
}

float WaterfallGradient::getValue(const QColor& color) const
{
    if (color.spec() == QColor::Invalid) {
        qCWarning(waterfallGradient) << "Invalid color.";
        return 0.0f;
    }

    const auto& stopsValues = stops();
    for (int i(0); i < stopsValues.length() - 1; i++) {
        if (colorsInRange(color, stopsValues[i].second, stopsValues[i + 1].second)) {
            return valueLinearInterpolation(color, stopsValues[i], stopsValues[i + 1]);
        }
    }

    return 0.0f;
}

bool WaterfallGradient::colorsInRange(const QColor& color, const QColor& color1, const QColor& color2)
{
    QColor maxColor;
    maxColor.setAlpha(color1.alpha() < color2.alpha() ? color2.alpha() : color1.alpha());
    maxColor.setRed(color1.red() < color2.red() ? color2.red() : color1.red());
    maxColor.setGreen(color1.green() < color2.green() ? color2.green() : color1.green());
    maxColor.setBlue(color1.blue() < color2.blue() ? color2.blue() : color1.blue());
    QColor minColor;
    minColor.setAlpha(color1.alpha() > color2.alpha() ? color2.alpha() : color1.alpha());
    minColor.setRed(color1.red() > color2.red() ? color2.red() : color1.red());
    minColor.setGreen(color1.green() > color2.green() ? color2.green() : color1.green());
    minColor.setBlue(color1.blue() > color2.blue() ? color2.blue() : color1.blue());

    bool aOk = color.alpha() >= minColor.alpha() && color.alpha() <= maxColor.alpha();
    bool rOk = color.red() >= minColor.red() && color.red() <= maxColor.red();
    bool gOk = color.green() >= minColor.green() && color.green() <= maxColor.green();
    bool bOk = color.blue() >= minColor.blue() && color.blue() <= maxColor.blue();

    return rOk && gOk && bOk;
}

QColor WaterfallGradient::colorLinearInterpolation(
    const float value, const QGradientStop& color1, const QGradientStop& color2)
{
    float minimum = color1.first;
    float maximum = color2.first;
    float ratio = (value - minimum) / (maximum - minimum);
    float a = ratio * color2.second.alpha() + (1 - ratio) * color1.second.alpha();
    float r = ratio * color2.second.red() + (1 - ratio) * color1.second.red();
    float g = ratio * color2.second.green() + (1 - ratio) * color1.second.green();
    float b = ratio * color2.second.blue() + (1 - ratio) * color1.second.blue();

    return QColor(r, g, b, a);
}

float WaterfallGradient::valueLinearInterpolation(
    const QColor& color, const QGradientStop& color1, const QGradientStop& color2)
{
    float ratio = 0;
    float value = 0;
    float minimum = 0;
    float maximum = 0;
    float testA = qAbs(color2.second.alpha() - color1.second.alpha());
    float testR = qAbs(color2.second.red() - color1.second.red());
    float testG = qAbs(color2.second.green() - color1.second.green());
    float testB = qAbs(color2.second.blue() - color1.second.blue());
    if (testR >= testG && testR >= testB) {
        maximum = qMax(color2.second.red(), color1.second.red());
        minimum = qMin(color2.second.red(), color1.second.red());
        ratio = float(color.red()) / (maximum - minimum);
        if (color2.second.red() < color1.second.red()) {
            ratio = 1 - ratio;
        }
    } else if (testG >= testR && testG >= testB) {
        maximum = qMax(color2.second.green(), color1.second.green());
        minimum = qMin(color2.second.green(), color1.second.green());
        ratio = float(color.green()) / (maximum - minimum);
        if (color2.second.green() < color1.second.green()) {
            ratio = 1 - ratio;
        }
    } else if (testB >= testG && testB >= testR) {
        maximum = qMax(color2.second.blue(), color1.second.blue());
        minimum = qMin(color2.second.blue(), color1.second.blue());
        ratio = float(color.blue()) / (maximum - minimum);
        if (color2.second.blue() < color1.second.blue()) {
            ratio = 1 - ratio;
        }
    } else {
        maximum = qMax(color2.second.alpha(), color1.second.alpha());
        minimum = qMin(color2.second.alpha(), color1.second.alpha());
        ratio = float(color.alpha()) / (maximum - minimum);
        if (color2.second.alpha() < color1.second.alpha()) {
            ratio = 1 - ratio;
        }
    }

    value = ratio * color2.first + (1 - ratio) * color1.first;

    return qMin(qMax(value, 0.0f), 1.0f);
}
