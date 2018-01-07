#include <QQuickPaintedItem>
#include <QImage>

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

    QImage _image;
    QPainter *_painter;
    float _mouseDepth;
    float _mouseStrength;

    signals:
    void imageChanged();
    void mouseDepthChanged();
    void mouseStrengthChanged();
    void mouseLeave();

public:
    Waterfall(QQuickItem *parent = 0);
    void paint(QPainter *painer);
    void setImage(const QImage &image);
    void randomUpdate();
    QColor valueToRGB(float point);
    float RGBToValue(QColor color);
    Q_INVOKABLE void draw(QList<double> points);
    void hoverMoveEvent(QHoverEvent *event);
    void hoverLeaveEvent(QHoverEvent *event);
};