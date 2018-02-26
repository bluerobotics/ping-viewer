#include <QLinearGradient>
#include <QtDebug>

#include "logger.h"

Q_DECLARE_LOGGING_CATEGORY(waterfallGradient)

class WaterfallGradient : public QLinearGradient
{
    QString _name;
public:

    WaterfallGradient(QString name = "", QList<QColor> colors = QList<QColor>());

    void setColors(const QList<QColor>& colors);
    void setName(const QString& name) ;
    QString name();
    QColor getColor(float value);
    float getValue(const QColor& color);
    bool colorsInRange(const QColor& color, const QColor& color1, const QColor& color2);
    QColor colorLinearInterpolation(float value, const QGradientStop& color1, const QGradientStop& color2);
    float valueLinearInterpolation(const QColor& color, const QGradientStop& color1, const QGradientStop& color2);
};