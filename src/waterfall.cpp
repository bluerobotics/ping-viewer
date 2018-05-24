#include "waterfall.h"

#include <QtConcurrent>
#include <QPainter>
#include <QtMath>
#include <QList>

PING_LOGGING_CATEGORY(waterfall, "ping.waterfall")

// Number of samples to display
uint16_t Waterfall::displayWidth = 500;

Waterfall::Waterfall(QQuickItem *parent):
    QQuickPaintedItem(parent),
    _image(2048, 200, QImage::Format_RGBA8888),
    _painter(nullptr),
    _mouseDepth(0),
    _mouseStrength(0),
    _smooth(true),
    _update(true),
    currentDrawIndex(displayWidth)
{
    setAntialiasing(_smooth);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    _image.fill(QColor(Qt::transparent));
    setGradients();
    setTheme("Thermal 5");

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [&] {if(_update) update(); _update = false;});
    timer->start(50);
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

void Waterfall::setTheme(const QString& theme)
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
    static QPixmap pix;
    if(painter != _painter) {
        _painter = painter;
    }

    static uint16_t first;

    if (currentDrawIndex < displayWidth) {
        first = 0;
    } else {
        first = currentDrawIndex - displayWidth;
    }

    // http://blog.qt.io/blog/2006/05/13/fast-transformed-pixmapimage-drawing/
    pix = QPixmap::fromImage(_image);
    _painter->drawPixmap(_painter->viewport(), pix, QRect(first, 0, displayWidth, _image.height()));
}

void Waterfall::setImage(const QImage &image)
{
    _image = image;
    emit imageChanged();
    setImplicitWidth(image.width());
    setImplicitHeight(image.height());
}

QColor Waterfall::valueToRGB(float point)
{
    return _gradient.getColor(point);
}

float Waterfall::RGBToValue(const QColor& color)
{
    return _gradient.getValue(color);
}

void Waterfall::draw(const QList<double>& points)
{
    if(points.isEmpty()) {
        randomUpdate();
        return;
    }

    static QImage old;
    static QList<double> oldPoints = points;

    // Copy tail to head
    // TODO can we get even better and allocate just once at initialization? ie circular buffering
    if (currentDrawIndex >= _image.width()) {
        old = _image.copy(_image.width() - displayWidth, 0, displayWidth, _image.height());
        QPainter painter(&_image);
        painter.drawImage(0, 0, old);
        painter.end();
        currentDrawIndex = displayWidth;
    }

    if(smooth()) {
        #pragma omp for
        for(int i = 0; i < points.length(); i++) {
            oldPoints[i] = points[i]*0.2 + oldPoints[i]*0.8;
        }
        #pragma omp for
        for(int i = 0; i < _image.height(); i++) {
            _image.setPixelColor(currentDrawIndex, i, valueToRGB(oldPoints[i]));

        }
    } else {
        #pragma omp for
        for(int i = 0; i < _image.height(); i++) {
            _image.setPixelColor(currentDrawIndex, i, valueToRGB(points[i]));

        }
    }
    currentDrawIndex++; // This can get to be an issue at very fast update rates from ping
    _update = true;
}

void Waterfall::hoverMoveEvent(QHoverEvent *event)
{
    event->accept();
    auto pos = event->pos();

    static uint16_t first;
    if (currentDrawIndex < displayWidth) {
        first = 0;
    } else {
        first = currentDrawIndex - displayWidth;
    }

    pos.setX(pos.x()*displayWidth/width() + first);
    pos.setY(pos.y()*_image.height()/height());

    // signal strength
    _mouseStrength = RGBToValue(_image.pixelColor(pos));
    // depth
    _mouseDepth = pos.y();
    emit mouseMove();
}

void Waterfall::hoverLeaveEvent(QHoverEvent *event)
{
    Q_UNUSED(event)
    emit mouseLeave();
    _mouseStrength = -1;
    _mouseDepth = -1;
}
