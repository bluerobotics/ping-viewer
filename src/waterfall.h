#include <QQuickPaintedItem>
#include <QImage>

class Waterfall : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
    QImage image() {return _image;}

    QImage _image;
    QPainter *_painter;

    signals:
    void imageChanged();

public:
    Waterfall(QQuickItem *parent = 0);
    void paint(QPainter *painer);
    void setImage(const QImage &image);
    void randomUpdate();
    QColor valueToRGB(float point);
    Q_INVOKABLE void draw(QList<double> points);
};