#include "waterfallplot.h"
#include "filemanager.h"

#include <limits>

#include <QPainter>
#include <QVector>
#include <QtConcurrent>
#include <QtMath>

PING_LOGGING_CATEGORY(waterfallplot, "ping.waterfallplot")

// Number of samples to display
uint16_t WaterfallPlot::_displayWidth = 500;

WaterfallPlot::WaterfallPlot(QQuickItem* parent)
    : Waterfall(parent)
    , _currentDrawIndex(_displayWidth)
    , _image(2048, 2500, QImage::Format_RGBA8888)
    , _maxDepthToDrawInPixels(0)
    , _minDepthToDrawInPixels(0)
    , _mouseDepth(0)
    , _painter(nullptr)
    , _updateTimer(new QTimer(this))
{
    // This is the max depth that ping returns
    setWaterfallMaxDepth(70);
    _DCRing.fill({static_cast<float>(_image.height()), 0, 0, 0}, _displayWidth);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    _image.fill(QColor(Qt::transparent));

    connect(_updateTimer, &QTimer::timeout, this, [&] { update(); });
    _updateTimer->setSingleShot(true);
    _updateTimer->start(50);

    connect(this, &Waterfall::mousePosChanged, this, &WaterfallPlot::updateMouseColumnData);
}

void WaterfallPlot::setWaterfallMaxDepth(float maxDepth)
{
    _waterfallDepth = maxDepth;
    _minPixelsPerMeter = _image.height() / _waterfallDepth;
}

void WaterfallPlot::paint(QPainter* painter)
{
    static QPixmap pix;
    if (painter != _painter) {
        _painter = painter;
    }

    static uint16_t first;

    if (_currentDrawIndex < _displayWidth) {
        first = 0;
    } else {
        first = _currentDrawIndex - _displayWidth;
    }

    // http://blog.qt.io/blog/2006/05/13/fast-transformed-pixmapimage-drawing/
    pix = QPixmap::fromImage(_image, Qt::NoFormatConversion);
    // Code for debug, draw the entire waterfall
    //_painter->drawPixmap(_painter->viewport(), pix, QRect(0, 0, _image.width(), _image.height()));
    _painter->drawPixmap(QRect(0, 0, width(), height()), pix,
        QRect(first, _minDepthToDrawInPixels, _displayWidth, _maxDepthToDrawInPixels));
}

void WaterfallPlot::setImage(const QImage& image)
{
    _image = image;
    emit imageChanged();
    setImplicitWidth(image.width());
    setImplicitHeight(image.height());
}

void WaterfallPlot::clear()
{
    qCDebug(waterfall) << "Cleaning waterfall and restarting internal variables";
    _maxDepthToDrawInPixels = 0;
    _minDepthToDrawInPixels = 0;
    _mouseDepth = 0;
    _DCRing.fill({static_cast<float>(_image.height()), 0, 0, 0}, _displayWidth);
    _image.fill(Qt::transparent);
}

void WaterfallPlot::draw(const QVector<double>& points, float confidence, float initPoint, float length, float distance)
{
    /*
        initPoint: The lowest point of the last sample in meters
        length: The length of the last sample in meters
        _minPixelsPerMeter: waterfall max pixel height divided by max depth
            _minPixelsPerMeter = _image.height()/_waterfallDepth;
        lastMaxDC: Returns the last DC structure with max depth
        lastMinDepth: Returns the minimum point in the chart
        _minDepthToDraw: Minimum depth point, populated by lastMinDepth
        _maxDepthToDraw: Maximum depth point, calculated from lastMaxDC
        dynamicPixelsPerMeterScalar: Calculate the delta between number of pixels per meter
            dynamicPixelsPerMeterScalar = 400/((_maxDepthToDraw - _minDepthToDraw)*_minPixelsPerMeter);

        old (oldest sample)     new (last sample)
        |                       |
        +-----------------------+  - _minDepthToDrawInPixels
        |         |-----|       |
        |         |-----|       |
        |         |-------------+  - virtualFloor
        |         |-------------+  - virtualHeight
        |         ||      |---| |
        |         ||       |-|  |
        |         ||       |-|  |
        |         ||       |-|  |
        |         +|       |-|  |
        |         |        ++   |
        |         |         |   |
        |         |         |   |
        |         |         |   |
        |         |         |   |
        |                   |   |
        |                   |   |
        |                   |   |
        +-----------------------+  - _maxDepthToDrawInPixels = 400px

        _minDepthToDrawInPixels: The lowest pixel that will appears for the user
            _minDepthToDrawInPixels = (_minDepthToDraw*_minPixelsPerMeter*dynamicPixelsPerMeterScalar);
        _maxDepthToDrawInPixels: Is defined to be 400 pixels
        virtualFloor: It's the lowest pixel position to start drawing the last sample
            virtualFloor = (initPoint*_minPixelsPerMeter*dynamicPixelsPerMeterScalar);
        virtualFloor: It's the highest delta pixel position (from virtualFloor) to finish drawing the last sample
            virtualHeight = ((length + initPoint - _minDepthToDraw)*_minPixelsPerMeter*dynamicPixelsPerMeterScalar);
    */

    // Declare oldImage variable to do image spins
    static QImage old = _image;
    // Declare oldPoints variable to do some filter
    static QVector<double> oldPoints = points;

    // This ring vector will store variables of the last n samples for user access
    _DCRing.append({initPoint, length, confidence, distance});

    /**
     * @brief Get lastMaxDepth from the last n samples
     */
    auto lastMaxDC = [&] {
        float maxDepth = 0;
        DCPack tempDC {0, 0, 0, 0};
        for (const auto& DC : qAsConst(_DCRing)) {
            if (maxDepth < DC.length + DC.initialDepth && DC.initialDepth != static_cast<float>(_image.height())) {
                maxDepth = DC.length + DC.initialDepth;
                tempDC = DC;
            }
        }
        return tempDC;
    };

    /**
     * @brief Get lastMinDepth from the last n samples
     */
    auto lastMinDepth = [&] {
        float minDepth = std::numeric_limits<float>::max();
        for (const auto& DC : qAsConst(_DCRing)) {
            minDepth = minDepth > DC.initialDepth ? DC.initialDepth : minDepth;
        }
        return minDepth;
    };

    /**
     * @brief Do a fast scale of image, but without changing the default size
     *
     * dst is the rectangle that will be used to draw old in `image`.
     * src is the rectangle that will be used as source to be drawed in `image`,
     *        the default value is old.rect().
     *
     */
    auto redrawImage = [&](const QRect& dst, QRect src = QRect()) {
        // Use old as default rect
        if (src.isEmpty()) {
            src = old.rect();
        }

        // Swap is faster
        _image.swap(old);
        _image.fill(Qt::transparent);
        QPainter painter(&_image);
        // Clean everything and start from zero
        painter.fillRect(_image.rect(), Qt::transparent);
        // QRect(0, 0, _image.width(), _image.height()*dynamicPixelsPerMeterScalar), old
        painter.drawImage(dst, old, src);
        painter.end();
    };

    static DCPack _maxDC;
    _maxDC = lastMaxDC();
    _minDepthToDraw = lastMinDepth();
    _maxDepthToDraw = _maxDC.initialDepth + _maxDC.length;
    emit minDepthToDrawChanged();
    emit maxDepthToDrawChanged();

    static bool inDynamic = false;
    static float dynamicPixelsPerMeterScalar = 1.0;
    // If the points/resolution is **NOT** bigger than 1pixel/point
    if ((_maxDepthToDraw - _minDepthToDraw) * _minPixelsPerMeter < 200) {
        if (!inDynamic) {
            inDynamic = true;
            dynamicPixelsPerMeterScalar = 200 / _minPixelsPerMeter;
            redrawImage(QRect(0, 0, _image.width(), _image.height() * dynamicPixelsPerMeterScalar));
        }
    } else {
        // If the points/resolution is bigger than 1pixel/point
        if (inDynamic) {
            redrawImage(QRect(0, 0, _image.width(), _image.height() / dynamicPixelsPerMeterScalar));
        }
        inDynamic = false;
        dynamicPixelsPerMeterScalar = 1;
    }
    _minDepthToDrawInPixels = _minDepthToDraw * _minPixelsPerMeter;
    _maxDepthToDrawInPixels = (_maxDepthToDraw - _minDepthToDraw) * _minPixelsPerMeter * dynamicPixelsPerMeterScalar;
    int virtualFloor = initPoint * _minPixelsPerMeter;
    int virtualHeight = length * _minPixelsPerMeter * dynamicPixelsPerMeterScalar;

    // Copy tail to head
    // TODO: can we get even better and allocate just once at initialization? ie circular buffering
    if (_currentDrawIndex >= _image.width()) {
        redrawImage(QRect(0, 0, _displayWidth, _image.height()),
            QRect(old.width() - _displayWidth, 0, _displayWidth, old.height()));

        // Start painting from the beginning
        _currentDrawIndex = _displayWidth;
    }

    // Do up/downsampling
    float factor = points.length() / static_cast<float>(virtualHeight);

    // Check if everything is correct before the draw
    if (floor(factor * virtualHeight) > points.length() || factor * virtualHeight < 0) {
        qCWarning(waterfallplot) << "Wrong factor !";
        qCDebug(waterfallplot).noquote() << QStringLiteral("virtualHeight: %1\t virtualFloor: %2\t factor: %3\t")
                                                .arg(virtualHeight)
                                                .arg(virtualFloor)
                                                .arg(factor);
        return;
    }

    if (virtualFloor + virtualHeight > _image.height() || virtualFloor + virtualHeight < 0 || virtualFloor < 0) {
        qCWarning(waterfallplot) << "Wrong Floor Height";
        qCDebug(waterfallplot).noquote() << QStringLiteral(
            "virtualFloor: %1\t virtualHeight: %2\t _minDepthToDrawInPixels: %3\t _maxDepthToDrawInPixels: %4")
                                                .arg(virtualFloor)
                                                .arg(virtualHeight)
                                                .arg(_minDepthToDrawInPixels)
                                                .arg(_maxDepthToDrawInPixels);
        qCDebug(waterfallplot).noquote() << QStringLiteral(
            "initPoint: %1\t length: %2\t _minPixelsPerMeter: %3\t dynamicPixelsPerMeterScalar: %4")
                                                .arg(initPoint)
                                                .arg(length)
                                                .arg(_minPixelsPerMeter)
                                                .arg(dynamicPixelsPerMeterScalar);
        return;
    }

    if (smooth()) {
#pragma omp for
        for (int i = 0; i < points.length(); i++) {
            oldPoints[i] = points[i] * 0.2 + oldPoints[i] * 0.8;
        }

#pragma omp for
        for (int i = 0; i < virtualHeight; i++) {
            _image.setPixelColor(_currentDrawIndex, i + virtualFloor, valueToRGB(oldPoints[factor * i]));
        }
    } else {
#pragma omp for
        for (int i = 0; i < virtualHeight; i++) {
            _image.setPixelColor(_currentDrawIndex, i + virtualFloor, valueToRGB(points[factor * i]));
        }
    }
    _currentDrawIndex++; // This can get to be an issue at very fast update rates from ping

    // Fix max update in 20Hz at max
    if (!_updateTimer->isActive()) {
        _updateTimer->start(50);
    }
}

void WaterfallPlot::updateMouseColumnData()
{
    static uint16_t first;
    if (_currentDrawIndex < _displayWidth) {
        first = 0;
    } else {
        first = _currentDrawIndex - _displayWidth;
    }

    int widthPos = _mousePos.x() * _displayWidth / width();
    _mousePos.setX(_mousePos.x() * _displayWidth / width() + first);
    _mousePos.setY(_mousePos.y() * (_maxDepthToDrawInPixels - _minDepthToDrawInPixels) / height());

    // depth
    _mouseDepth = _mousePos.y() / (float)_minPixelsPerMeter;
    emit mouseMove();

    const auto& depthAndConfidence = _DCRing[_displayWidth - widthPos];
    _mouseColumnConfidence = depthAndConfidence.confidence;
    _mouseColumnDepth = depthAndConfidence.distance;
    emit mouseColumnConfidenceChanged();
    emit mouseColumnDepthChanged();
}
