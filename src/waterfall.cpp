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
    _image(2048, 2000, QImage::Format_RGBA8888),
    _painter(nullptr),
    _mouseDepth(0),
    _mouseStrength(0),
    _smooth(true),
    _update(true),
    currentDrawIndex(displayWidth)
{
    // This is the max depth that ping returns
    setWaterfallMaxDepth(48.903);
    _DCRing.fill({0, 0}, displayWidth);
    setAntialiasing(_smooth);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    _image.fill(QColor(Qt::transparent));
    setGradients();
    setTheme("Thermal 5");

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [&] {if(_update) update(); _update = false;});
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

void Waterfall::setWaterfallMaxDepth(float maxDepth)
{
    _waterfallDepth = maxDepth;
    _pixelsPerMeter = _image.height()/_waterfallDepth;
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
    // Code for debug, draw the entire waterfall
    //_painter->drawPixmap(_painter->viewport(), pix, QRect(0, 0, _image.width(), _image.height()));
    _painter->drawPixmap(_painter->viewport(), pix, QRect(first, 0, displayWidth, _maxDepthToDrawInPixels));
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

void Waterfall::draw(const QList<double>& points, float depth, float confidence)
{
    static QImage old = _image;
    static QList<double> oldPoints = points;
    _DCRing.append({depth, confidence});

    int virtualHeight = floor(_pixelsPerMeter*depth);

    static auto lastMaxDepth = [this] {
        float maxDepth = 0;
        for(const auto& DC : qAsConst(this->_DCRing))
        {
            maxDepth = maxDepth < DC.depth ? DC.depth : maxDepth;
        }
        return maxDepth;
    };

    _maxDepthToDraw = lastMaxDepth();
    emit maxDepthToDrawChanged();
    _maxDepthToDrawInPixels = floor(_maxDepthToDraw*_pixelsPerMeter);

    // Copy tail to head
    // TODO can we get even better and allocate just once at initialization? ie circular buffering
    if (currentDrawIndex >= _image.width()) {
        //Swap is faster
        _image.swap(old);
        _image.fill(Qt::transparent);
        QPainter painter(&_image);
        // Clean everything and start from zero
        painter.fillRect(_image.rect(), Qt::transparent);
        painter.drawImage(QRect(0, 0, displayWidth, _image.height()),
                          old, QRect(old.width() - displayWidth, 0, displayWidth, old.height()));
        painter.end();

        // Start painting from the beginning
        currentDrawIndex = displayWidth;
    }

    // Do up/downsampling
    float factor = points.length()/((float)virtualHeight);

    if(smooth()) {
        #pragma omp for
        for(int i = 0; i < points.length(); i++) {
            oldPoints[i] = points[i]*0.2 + oldPoints[i]*0.8;
        }

        #pragma omp for
        for(int i = 0; i < virtualHeight; i++) {
            _image.setPixelColor(currentDrawIndex, i, valueToRGB(oldPoints[factor*i]));

        }
    } else {
        #pragma omp for
        for(int i = 0; i < virtualHeight; i++) {
            _image.setPixelColor(currentDrawIndex, i, valueToRGB(points[factor*i]));

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

    int widthPos = pos.x()*displayWidth/width();
    pos.setX(pos.x()*displayWidth/width() + first);
    pos.setY(pos.y()*_maxDepthToDrawInPixels/(float)height());

    // signal strength
    _mouseStrength = RGBToValue(_image.pixelColor(pos));

    // depth
    _mouseDepth = pos.y()/(float)_pixelsPerMeter;
    emit mouseMove();

    const auto& depthAndConfidence = _DCRing[displayWidth - widthPos];
    _mouseColumnConfidence = depthAndConfidence.confidence;
    _mouseColumnDepth = depthAndConfidence.depth;
    emit mouseColumnConfidenceChanged();
    emit mouseColumnDepthChanged();
}

void Waterfall::hoverLeaveEvent(QHoverEvent *event)
{
    Q_UNUSED(event)
    emit mouseLeave();
    _mouseStrength = -1;
    _mouseDepth = -1;
}
