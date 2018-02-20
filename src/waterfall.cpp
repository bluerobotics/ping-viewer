#include "waterfall.h"

#include <QPainter>
#include <QtMath>
#include <QList>

PING_LOGGING_CATEGORY(waterfall, "ping.waterfall")

Waterfall::Waterfall(QQuickItem *parent):
    QQuickPaintedItem(parent),
    _image(1000, 200, QImage::Format_RGBA8888),
    _painter(nullptr),
    _mouseDepth(0),
    _mouseStrength(0),
    _smooth(true)
{
    setAntialiasing(true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    _image.fill(QColor(Qt::transparent));
    setGradients();
    setTheme("Thermal 5");
}

void Waterfall::setGradients()
{
    WaterfallGradient thermal5Grad(QStringLiteral("Thermal 5"), {
        Qt::blue,
        Qt::cyan,
        Qt::green,
        Qt::yellow,
        Qt::red,
    });
    _gradients.append(thermal5Grad);

    WaterfallGradient thermal6Grad(QStringLiteral("Thermal 6"), {
        Qt::black,
        Qt::blue,
        Qt::cyan,
        Qt::green,
        Qt::yellow,
        Qt::red,
    });
    _gradients.append(thermal6Grad);

    WaterfallGradient thermal7Grad(QStringLiteral("Thermal 7"), {
        Qt::black,
        Qt::blue,
        Qt::cyan,
        Qt::green,
        Qt::yellow,
        Qt::red,
        Qt::white,
    });
    _gradients.append(thermal7Grad);

    WaterfallGradient monochrome(QStringLiteral("Monochrome"), {
        Qt::black,
        Qt::white,
    });
    _gradients.append(monochrome);

    WaterfallGradient ocean(QStringLiteral("Ocean"), {
        QColor(48,12,64),
		QColor(86,30,111),
		QColor(124,85,135),
		QColor(167,114,130),
		QColor(206,154,132),
    });
    _gradients.append(ocean);

    WaterfallGradient transparent(QStringLiteral("Transparent"), {
        QColor(20, 0, 120),
		QColor(200, 30, 140),
		QColor(255, 100, 0),
		QColor(255, 255, 40),
		QColor(255, 255, 255),
    });
    _gradients.append(transparent);

    WaterfallGradient fishfinder(QStringLiteral("Fishfinder"), {
        QColor(0, 0, 60),
        QColor(61, 6, 124),
        QColor(212, 45, 107),
        QColor(255, 102, 0),
        QColor(255, 227, 32),
        Qt::white,
    });
    _gradients.append(fishfinder);

    WaterfallGradient rainbow(QStringLiteral("Rainbow"), {
        Qt::black,
        Qt::magenta,
        Qt::blue,
        Qt::cyan,
        Qt::darkGreen,
        Qt::yellow,
        Qt::red,
        Qt::white,
    });
    _gradients.append(rainbow);

    for(auto &gradient : _gradients) {
        _themes.append(gradient.name());
    }
    qCDebug(waterfall) << "Gradients:" << _themes;
    emit themesChanged();
}

void Waterfall::setTheme(QString theme)
{
    _theme = theme;
    for(auto &gradient : _gradients) {
        if(gradient.name() == theme) {
            _gradient = gradient;
            break;
        }
    }
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
    return _gradient.getColor(point);
}

float Waterfall::RGBToValue(QColor color)
{
    return _gradient.getValue(color);
}

void Waterfall::draw(QList<double> points)
{
    if(points.isEmpty()) {
        randomUpdate();
        return;
    }

    static QImage old;
    static QList<double> oldPoints = points;
    old = _image.copy(1, 0, _image.width() - 1, _image.height());
    QPainter painter(&_image);
    painter.drawImage(0, 0, old);
    painter.end();

    if(smooth()) {
        for(int i(0); i < points.length(); i++) {
            oldPoints[i] = points[i]*0.2 + oldPoints[i]*0.8;
        }

        for(int i(0); i < _image.height(); i++) {
            _image.setPixelColor(_image.width() - 1, i, valueToRGB(oldPoints[i]));

        }
    } else {
        for(int i(0); i < _image.height(); i++) {
            _image.setPixelColor(_image.width() - 1, i, valueToRGB(points[i]));

        }
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
            point = 0.1 * (qrand()%256)/255;
        } else if (i < stop2) {
            point = (-4.0 / qPow((stop2-stop1), 2.0)) * qPow((i - stop1 - ((stop2-stop1) / 2.0)), 2.0)  + 1.0;
        } else {
            point = 0.45 * (qrand()%256)/255;
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