#include "waterfall.h"

#include <QPainter>
#include <QTimer>
#include <QtMath>
#include <QList>

Waterfall::Waterfall(QQuickItem *parent):
    QQuickPaintedItem(parent),
    _image(1000, 200, QImage::Format_RGBA8888),
    _painter(nullptr),
    _mouseDepth(0),
    _mouseStrength(0)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    _image.fill(QColor(Qt::transparent));
}

void Waterfall::paint(QPainter *painter)
{
    if(painter != _painter) {
        _painter = painter;
    }

    _painter->drawImage(0, 0, _image.scaled(width(), height()));
}

void Waterfall::setImage(const QImage &image)
{
    _image = image;
    emit imageChanged();
    setImplicitWidth(image.width());
    setImplicitHeight(image.height());
    update();
}

QColor Waterfall::valueToRGB(float point)
{
    static const float minimum = 0.0f;
    static const float maximum = 1.0f;
    float ratio = 2 * (point - minimum) / (maximum - minimum);
    float r = qMax(0.0f, (ratio - 1.0f));
    float b = qMax(0.0f, (1.0f - ratio));
    float g = (1.0f - b - r);
    r *= 255;
    g *= 255;
    b *= 255;
    return QColor(r, g, b);
}

float Waterfall::RGBToValue(QColor color)
{
    if(color == QColor(0, 0, 0, 0))
        return 0.0f;
    return ((color.red()/255.f-color.blue()/255.f) + 1.0f)/2.0f;
}

void Waterfall::draw(QList<double> points)
{
    static QImage old;
    old = _image.copy(1, 0, _image.width() - 1, _image.height());
    QPainter painter(&_image);
    painter.drawImage(0, 0, old);
    painter.end();

    for(int i(0); i < _image.height(); i++) {
        _image.setPixelColor(_image.width() - 1, i, valueToRGB(points[i]));
    }

    update();
}

void Waterfall::randomUpdate()
{
    static uint counter = 0;
    counter++;
    QList <double> points;
    points.reserve(_image.height());
    const float numPoints = _image.height();
    const float stop1 = numPoints / 2.0 - 10 * qSin(counter / 10.0);
    const float stop2 = 3 * numPoints / 5.0 + 6 * qCos(counter / 5.5);
    for (int i(0); i < numPoints; i++) {
        float point;
        if (i < stop1) {
            point = 0.1 * (qrand()%255)/255;
        } else if (i < stop2) {
            point = (-4.0 / qPow((stop2-stop1), 2.0)) * qPow((i - stop1 - ((stop2-stop1) / 2.0)), 2.0)  + 1.0;
        } else {
            point = 0.45 * (qrand()%255)/255;
        }

        points.append(point);
    }
    draw(points);
}

void Waterfall::hoverMoveEvent(QHoverEvent *event)
{
    event->accept();
    auto pos = event->pos();
    pos.setX(pos.x()*_image.width()/width());
    pos.setY(pos.y()*_image.height()/height());

    // signal strength
    _mouseStrength = RGBToValue(_image.pixelColor(pos));
    // depth
    _mouseDepth = pos.y();
}

void Waterfall::hoverLeaveEvent(QHoverEvent *event)
{
    Q_UNUSED(event)
    emit mouseLeave();
    _mouseStrength = -1;
    _mouseDepth = -1;
}