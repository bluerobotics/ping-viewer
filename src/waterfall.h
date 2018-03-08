#include <QQuickPaintedItem>
#include <QImage>

#include "logger.h"
#include "waterfallgradient.h"

Q_DECLARE_LOGGING_CATEGORY(waterfall)

class Waterfall : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
    QImage image() {return _image;}

    Q_PROPERTY(float mouseDepth READ mouseDepth NOTIFY mouseDepthChanged)
    float mouseDepth() {return _mouseDepth;}
    Q_PROPERTY(float mouseStrength READ mouseStrength NOTIFY mouseStrengthChanged)
    float mouseStrength() {return _mouseStrength;}

    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    const QString theme() {return _theme;}
    Q_PROPERTY(QStringList themes READ themes NOTIFY themesChanged)
    const QStringList themes() {return _themes;}

    Q_PROPERTY(bool smooth READ smooth WRITE setSmooth NOTIFY smoothChanged)
    bool smooth() {return _smooth;}
    void setSmooth(bool smooth) {_smooth = smooth; setAntialiasing(_smooth); emit smoothChanged();}

    QList<WaterfallGradient> _gradients;
    WaterfallGradient _gradient;
    QImage _image;
    QPainter *_painter;
    float _mouseDepth;
    float _mouseStrength;
    bool _smooth;
    bool _update;
    QString _theme;
    QList<QString> _themes;
    static uint16_t displayWidth;
    uint16_t currentDrawIndex;

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
    void setTheme(const QString& theme);
    QColor valueToRGB(float point);
    float RGBToValue(const QColor& color);
    Q_INVOKABLE void draw(const QList<double>& points);
    void hoverMoveEvent(QHoverEvent *event);
    void hoverLeaveEvent(QHoverEvent *event);
};
