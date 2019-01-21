#include "filemanager.h"
#include "waterfall.h"

#include <limits>

#include <QtConcurrent>
#include <QPainter>
#include <QtMath>
#include <QVector>

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
    _containsMouse(false),
    _smooth(true),
    _updateTimer(new QTimer(this)),
    currentDrawIndex(displayWidth)
{
    // This is the max depth that ping returns
    setWaterfallMaxDepth(70);
    _DCRing.fill({static_cast<float>(_image.height()), 0, 0, 0}, displayWidth);
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
    _DCRing.fill({static_cast<float>(_image.height()), 0, 0, 0}, displayWidth);
    _image.fill(Qt::transparent);
}

void Waterfall::setGradients()
{
    WaterfallGradient thermal5Blue(QStringLiteral("Thermal blue"), {
        QColor("#05225f"),
        QColor("#6aa84f"),
        QColor("#ffff00"),
        QColor("#7f6000"),
        QColor("#5c0f08"),
    });
    _gradients.append(thermal5Blue);

    WaterfallGradient thermal5Black(QStringLiteral("Thermal black"), {
        Qt::black,
        QColor("#6aa84f"),
        QColor("#ffff00"),
        QColor("#7f6000"),
        QColor("#5c0f08"),
    });
    _gradients.append(thermal5Black);

    WaterfallGradient thermal5White(QStringLiteral("Thermal white"), {
        Qt::white,
        QColor("#6aa84f"),
        QColor("#ffff00"),
        QColor("#7f6000"),
        QColor("#5c0f08"),
    });
    _gradients.append(thermal5White);

    WaterfallGradient monochromeBlack(QStringLiteral("Monochrome black"), {
        Qt::black,
        Qt::white,
    });
    _gradients.append(monochromeBlack);

    WaterfallGradient monochromeWhite(QStringLiteral("Monochrome white"), {
        Qt::white,
        Qt::black,
    });
    _gradients.append(monochromeWhite);

    WaterfallGradient monochromeSepia(QStringLiteral("Monochrome sepia"), {
        QColor("#302113"),
        QColor("#e8c943"),
    });
    _gradients.append(monochromeSepia);

    loadUserGradients();

    for(auto &gradient : _gradients) {
        _themes.append(gradient.name());
    }
    qCDebug(waterfall) << "Gradients:" << _themes;
    emit themesChanged();
}

void Waterfall::loadUserGradients()
{
    auto fileInfoList = FileManager::self()->getFilesFrom(FileManager::Folder::Gradients);
    for(auto& fileInfo : fileInfoList) {
        qCDebug(waterfall) << fileInfo.fileName();
        QFile file(fileInfo.absoluteFilePath());
        WaterfallGradient gradient(file);
        if(!gradient.isOk()) {
            qCDebug(waterfall) << "Invalid gradient file:" << fileInfo.fileName();
            continue;
        }
        _gradients.append(gradient);
    }
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
                         QRect(first, _minDepthToDrawInPixels, displayWidth, _maxDepthToDrawInPixels));
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

void Waterfall::draw(const QVector<double>& points, float confidence, float initPoint, float length, float distance)
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
    static auto lastMaxDC = [this] {
        float maxDepth = 0;
        DCPack tempDC{0, 0, 0, 0};
        for(const auto& DC : qAsConst(this->_DCRing))
        {
            if(maxDepth < DC.length + DC.initialDepth && DC.initialDepth != static_cast<const float>(_image.height())) {
                maxDepth = DC.length + DC.initialDepth;
                tempDC = DC;
            }
        }
        return tempDC;
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
    static DCPack _maxDC;
    _maxDC = lastMaxDC();
    _minDepthToDraw = lastMinDepth();
    _maxDepthToDraw = _maxDC.initialDepth + _maxDC.length;
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
            painter.drawImage(QRect(0, 0, _image.width(),
                                    _image.height()*dynamicPixelsPerMeterScalar/lastDynamicPixelsPerMeterScalar),
                              old.scaled(_image.width(), _image.height()));
            painter.end();
        }

        lastDynamicPixelsPerMeterScalar = dynamicPixelsPerMeterScalar;
    } else {
        _maxDepthToDrawInPixels = (_maxDepthToDraw  - _minDepthToDraw)*_minPixelsPerMeter;
    }
    _minDepthToDrawInPixels = _minDepthToDraw*_minPixelsPerMeter;
    int virtualFloor = initPoint*_minPixelsPerMeter;
    int virtualHeight = length*_minPixelsPerMeter*dynamicPixelsPerMeterScalar;

    // Copy tail to head
    // TODO: can we get even better and allocate just once at initialization? ie circular buffering
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
    float factor = points.length()/((float)(virtualHeight));

    // Check if everything is correct before the draw
    if(floor(factor*virtualHeight) > points.length() || factor*virtualHeight < 0) {
        qCWarning(waterfall) << "Wrong factor !";
        qCDebug(waterfall).noquote() << QStringLiteral("virtualHeight: %1\t virtualFloor: %2\t factor: %3\t").
                                     arg(virtualHeight).arg(virtualFloor).arg(factor);
        return;
    }

    if(virtualFloor + virtualHeight > _image.height()
            || virtualFloor + virtualHeight < 0
            || virtualFloor < 0) {
        qCWarning(waterfall) << "Wrong Floor Height";
        qCDebug(waterfall).noquote() <<
                                     QStringLiteral("virtualFloor: %1\t virtualHeight: %2\t _minDepthToDrawInPixels: %3\t _maxDepthToDrawInPixels: %4")
                                     .arg(virtualFloor).arg(virtualHeight).arg(_minDepthToDrawInPixels).arg(_maxDepthToDrawInPixels);
        qCDebug(waterfall).noquote() <<
                                     QStringLiteral("initPoint: %1\t length: %2\t _minPixelsPerMeter: %3\t dynamicPixelsPerMeterScalar: %4")
                                     .arg(initPoint).arg(length).arg(_minPixelsPerMeter).arg(dynamicPixelsPerMeterScalar);
        return;
    }

    if(smooth()) {
        #pragma omp for
        for(int i = 0; i < points.length(); i++) {
            oldPoints[i] = points[i]*0.2 + oldPoints[i]*0.8;
        }

        #pragma omp for
        for(int i = 0; i < virtualHeight; i++) {
            _image.setPixelColor(currentDrawIndex, i + virtualFloor, valueToRGB(oldPoints[factor*i]));
        }
    } else {
        #pragma omp for
        for(int i = 0; i < virtualHeight; i++) {
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
    // The mouse is not inside the waterfall area, so set the depth under the mouse to an invalid value
    _mouseDepth = -1;
    _containsMouse = false;
    emit containsMouseChanged();
}

void Waterfall::hoverEnterEvent(QHoverEvent *event)
{
    Q_UNUSED(event)
    _containsMouse = true;
    emit containsMouseChanged();
}
