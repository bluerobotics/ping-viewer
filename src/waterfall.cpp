#include "waterfall.h"

#include <limits>

#include <QtConcurrent>
#include <QPainter>
#include <QtMath>
#include <QList>

PING_LOGGING_CATEGORY(waterfall, "ping.waterfall")

// Number of samples to display
uint16_t Waterfall::displayWidth = 500;

Waterfall::Waterfall(QQuickItem *parent):
    QQuickPaintedItem(parent),
    _image(2048, 2500, QImage::Format_RGBA8888),
    _painter(nullptr),
    _maxDepthToDrawInPixels(0),
    _minDepthToDrawInPixels(0),
    _mouseDepth(0),
    _mouseStrength(0),
    _smooth(true),
    _updateTimer(new QTimer(this)),
    currentDrawIndex(displayWidth)
{
    // This is the max depth that ping returns
    setWaterfallMaxDepth(70);
    _DCRing.fill({static_cast<const float>(_image.height()), 0, 0, 0}, displayWidth);
    setAntialiasing(_smooth);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    _image.fill(QColor(Qt::transparent));
    setGradients();
    setTheme("Thermal 5");

    connect(_updateTimer, &QTimer::timeout, this, [&] {update();});
    _updateTimer->setSingleShot(true);
    _updateTimer->start(50);
}

void Waterfall::clear()
{
    qCDebug(waterfall) << "Cleaning waterfall and restarting internal variables";
    _maxDepthToDrawInPixels = 0;
    _minDepthToDrawInPixels = 0;
    _mouseDepth = 0;
    _mouseStrength = 0;
    _DCRing.fill({static_cast<const float>(_image.height()), 0, 0, 0}, displayWidth);
    _image.fill(Qt::transparent);
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
    _minPixelsPerMeter = _image.height()/_waterfallDepth;
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
    pix = QPixmap::fromImage(_image, Qt::NoFormatConversion);
    // Code for debug, draw the entire waterfall
    //_painter->drawPixmap(_painter->viewport(), pix, QRect(0, 0, _image.width(), _image.height()));
    _painter->drawPixmap(QRect(0, 0, width(), height()), pix,
                         QRect(first, _minDepthToDrawInPixels, displayWidth, _maxDepthToDrawInPixels - _minDepthToDrawInPixels));
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

void Waterfall::draw(const QList<double>& points, float depth, float confidence, float initPoint, float distance)
{
    // Declare oldImage variable to do image spins
    static QImage old = _image;
    // Declare oldPoints variable to do some filter
    static QList<double> oldPoints = points;

    // This ring vector will store variables of the last n samples for user access
    _DCRing.append({initPoint, depth, confidence, distance});

    /**
     * @brief Get lastMaxDepth from the last n samples
     */
    static auto lastMaxDepth = [this] {
        float maxDepth = 0;
        for(const auto& DC : qAsConst(this->_DCRing))
        {
            maxDepth = maxDepth < DC.depth ? DC.depth : maxDepth;
        }
        return maxDepth;
    };

    /**
     * @brief Get lastMinDepth from the last n samples
     */
    static auto lastMinDepth = [this] {
        float minDepth = std::numeric_limits<float>::max();
        for(const auto& DC : qAsConst(this->_DCRing))
        {
            minDepth = minDepth > DC.initialDepth ? DC.initialDepth : minDepth;
        }
        return minDepth;
    };

    _minDepthToDraw = lastMinDepth();
    _maxDepthToDraw = lastMaxDepth();
    emit minDepthToDrawChanged();
    emit maxDepthToDrawChanged();

    // Calculate new resolution for small distances range
    static float lastDynamicPixelsPerMeterScalar = 0;
    float dynamicPixelsPerMeterScalar = 1.0;
    // Fix min resolution to be 400 pixels
    if((_maxDepthToDraw - _minDepthToDraw)*_minPixelsPerMeter < 400) {
        dynamicPixelsPerMeterScalar = 400/((_maxDepthToDraw - _minDepthToDraw)*_minPixelsPerMeter);
        if(!lastDynamicPixelsPerMeterScalar) {
            lastDynamicPixelsPerMeterScalar = dynamicPixelsPerMeterScalar;
        }
        _maxDepthToDrawInPixels = 400;

        // Rescale everything when resolution changes
        if(lastDynamicPixelsPerMeterScalar != dynamicPixelsPerMeterScalar) {
            //Swap is faster
            _image.swap(old);
            _image.fill(Qt::transparent);
            QPainter painter(&_image);
            // Clean everything and start from zero
            painter.fillRect(_image.rect(), Qt::transparent);
            painter.drawImage(QRect(0, 0, _image.width(), _image.height()*dynamicPixelsPerMeterScalar/lastDynamicPixelsPerMeterScalar),
                              old.scaled(_image.width(), _image.height()));
            painter.end();
        }

        lastDynamicPixelsPerMeterScalar = dynamicPixelsPerMeterScalar;
    } else {
        _maxDepthToDrawInPixels = floor(_maxDepthToDraw*_minPixelsPerMeter);
    }
    _minDepthToDrawInPixels = floor(_minDepthToDraw*_minPixelsPerMeter);

    int virtualFloor = floor(initPoint*_minPixelsPerMeter);
    int virtualHeight = floor(depth*_minPixelsPerMeter*dynamicPixelsPerMeterScalar);

    if(virtualHeight <= virtualFloor || _image.height() < virtualHeight || 0 > virtualFloor) {
        qCWarning(waterfall) << "Invalid Height and Floor:" << virtualHeight << virtualFloor;
        return;
    }

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
                          old, QRect(old.width() - displayWidth, 0, displayWidth, old.height()),
                          Qt::NoFormatConversion);
        painter.end();

        // Start painting from the beginning
        currentDrawIndex = displayWidth;
    }

    // Do up/downsampling
    float factor = points.length()/((float)(virtualHeight - virtualFloor));

    if(smooth()) {
        #pragma omp for
        for(int i = 0; i < points.length(); i++) {
            oldPoints[i] = points[i]*0.2 + oldPoints[i]*0.8;
        }

        #pragma omp for
        for(int i = 0; i < virtualHeight - virtualFloor; i++) {
            _image.setPixelColor(currentDrawIndex, i + virtualFloor, valueToRGB(oldPoints[factor*i]));
        }
    } else {
        #pragma omp for
        for(int i = 0; i < virtualHeight - virtualFloor; i++) {
            _image.setPixelColor(currentDrawIndex, i + virtualFloor, valueToRGB(points[factor*i]));
        }
    }
    currentDrawIndex++; // This can get to be an issue at very fast update rates from ping

    // Fix max update in 20Hz at max
    if(!_updateTimer->isActive()) {
        _updateTimer->start(50);
    }
}

void Waterfall::hoverMoveEvent(QHoverEvent *event)
{
    event->accept();
    auto pos = event->pos();
    _mousePos = pos;
    emit mousePosChanged();

    static uint16_t first;
    if (currentDrawIndex < displayWidth) {
        first = 0;
    } else {
        first = currentDrawIndex - displayWidth;
    }

    int widthPos = pos.x()*displayWidth/width();
    pos.setX(pos.x()*displayWidth/width() + first);
    pos.setY(pos.y()*(_maxDepthToDrawInPixels-_minDepthToDrawInPixels)/(float)height());

    // signal strength
    _mouseStrength = RGBToValue(_image.pixelColor(pos));

    // depth
    _mouseDepth = pos.y()/(float)_minPixelsPerMeter;
    emit mouseMove();

    const auto& depthAndConfidence = _DCRing[displayWidth - widthPos];
    _mouseColumnConfidence = depthAndConfidence.confidence;
    _mouseColumnDepth = depthAndConfidence.distance;
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
