#include <QLinearGradient>
#include <QtDebug>

class WaterfallGradient : public QLinearGradient
{
    QString _name;
public:

    WaterfallGradient(QString name = "", QList<QColor> colors = QList<QColor>());

    void setColors(QList<QColor> colors);
    void setName(QString name) ;
    QString name();
    QColor getColor(float value);
    float getValue(QColor color);
    bool colorsInRange(QColor color, QColor color1, QColor color2);
    QColor colorLinearInterpolation(float value, QGradientStop color1, QGradientStop color2);
    float valueLinearInterpolation(QColor color, QGradientStop color1, QGradientStop color2);
};