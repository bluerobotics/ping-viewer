#include <QQuickPaintedItem>
#include <QImage>

#include "waterfallgradient.h"

class Waterfall : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
    QImage image() {return _image;}

    Q_PROPERTY(float mouseDepth READ mouseDepth NOTIFY mouseDepthChanged)
    float mouseDepth() {return _mouseDepth;}
    Q_PROPERTY(float mouseStrength READ mouseStrength NOTIFY mouseStrengthChanged)
    float mouseStrength() {return _mouseStrength;}
    Q_PROPERTY(NOTIFY mouseLeave);

    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    const QString theme() {return _theme;}
    Q_PROPERTY(QStringList themes READ themes NOTIFY themesChanged)
    const QStringList themes() {return _themes;}

    Q_PROPERTY(bool smooth READ smooth WRITE setSmooth NOTIFY smoothChanged)
    bool smooth() {return _smooth;}
    void setSmooth(bool smooth) {_smooth = smooth; emit smoothChanged();}

    QList<WaterfallGradient> _gradients;
    WaterfallGradient _gradient;
    QImage _image;
    QPainter *_painter;
    float _mouseDepth;
    float _mouseStrength;
    bool _smooth;
    QString _theme;
    QList<QString> _themes;

    signals:
    void imageChanged();
    void mouseDepthChanged();
    void mouseStrengthChanged();
    void mouseLeave();
    void themeChanged();
    void themesChanged();
    void smoothChanged();

public:
    Waterfall(QQuickItem *parent = 0);
    void paint(QPainter *painer);
    void setImage(const QImage &image);
    void randomUpdate();
    void setGradients();
    void setTheme(QString theme);
    QColor valueToRGB(float point);
    float RGBToValue(QColor color);
    Q_INVOKABLE void draw(QList<double> points);
    void hoverMoveEvent(QHoverEvent *event);
    void hoverLeaveEvent(QHoverEvent *event);
};