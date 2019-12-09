#include "polarplot.h"
#include "filemanager.h"

#include <limits>

#include <QPainter>
#include <QVector>
#include <QtConcurrent>
#include <QtMath>

PING_LOGGING_CATEGORY(polarplot, "ping.polarplot")

// Number of samples to display
uint16_t PolarPlot::_angularResolution = 400;

PolarPlot::PolarPlot(QQuickItem* parent)
    : Waterfall(parent)
    , _distances(_angularResolution, 0)
    , _image(400, 1200, QImage::Format_RGBA8888)
    , _maxDistance(0)
    , _painter(nullptr)
    , _sectorSizeDegrees(0)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    _image.fill(QColor(Qt::transparent));

    connect(&_updateTimer, &QTimer::timeout, this, [&] { update(); });
    _updateTimer.setSingleShot(true);
    _updateTimer.start(50);

    connect(this, &Waterfall::mousePosChanged, this, &PolarPlot::updateMouseColumnData);
    connect(this, &Waterfall::themeChanged, this, &PolarPlot::clear);
}

void PolarPlot::clear()
{
    qCDebug(polarplot) << "Cleaning waterfall and restarting internal variables";
    _image.fill(Qt::transparent);
    _distances.fill(0, _angularResolution);
    _maxDistance = 0;
}

void PolarPlot::paint(QPainter* painter)
{
    static QPixmap pix;
    if (painter != _painter) {
        _painter = painter;
    }

    // http://blog.qt.io/blog/2006/05/13/fast-transformed-pixmapimage-drawing/
    pix = QPixmap::fromImage(_image, Qt::NoFormatConversion);
    _painter->drawPixmap(QRect(0, 0, width(), height()), pix, QRect(0, 0, _image.width(), _image.height()));
}

void PolarPlot::setImage(const QImage& image)
{
    _image = image;
    emit imageChanged();
    setImplicitWidth(image.width());
    setImplicitHeight(image.height());
}

void PolarPlot::draw(
    const QVector<double>& points, float angle, float initPoint, float length, float angleGrad, float sectorSize)
{
    static const int maxGradian = 400;
    const float sectorSizeGradian = sectorSize * 200.0f / 180.0f;

    if (_sectorSizeDegrees != sectorSize) {
        _sectorSizeDegrees = sectorSize;
        emit sectorSizeDegreesChanged();
    }

    // TODO: Need a better way to deal with dynamic steps, maybe doing `draw(data, angle++)` with `angleGrad` loop
    while (angle < 0) {
        angle += maxGradian;
    }
    _distances[static_cast<int>(angle) % _angularResolution] = initPoint + length;

    float maxDistance = 0;
    for (const auto distance : _distances) {
        if (distance > maxDistance) {
            maxDistance = distance;
        }
    }

    if (maxDistance != _maxDistance) {
        _maxDistance = maxDistance;
        emit maxDistanceChanged();
    }

    // The sensor can provide less than 1200 points, the scale factor will scale the samples if necessary
    float scale = static_cast<float>(points.length()) / _image.height();
    for (int angleRange = -angleGrad / 2.0f; angleRange <= angleGrad / 2.0f; angleRange++) {
        // We know that the max and min angle range for ping360 is [0-400)
        int newAngle = static_cast<int>(angle + angleRange + maxGradian) % maxGradian;

        // Check if we are inside the sector
        if (newAngle > sectorSizeGradian / 2.0f && newAngle < maxGradian - sectorSizeGradian / 2.0f) {
            continue;
        }

        for (int index = 0; index < _image.height(); index++) {
            _image.setPixelColor(static_cast<int>(newAngle), index, valueToRGB(points[index * scale]));
        }
    }

    // Fix max update in 20Hz at max
    if (!_updateTimer.isActive()) {
        _updateTimer.start(50);
    }

    emit imageChanged();
}

void PolarPlot::updateMouseColumnData()
{
    static const float rad2grad = 200.0f / M_PI;
    static const float grad2deg = 180.0f / 200.0f;
    const QPointF center(width() / 2, height() / 2);

    /**
     * @brief delta is a normalized value with x,y ∈ [-1, 1]
     * Where (0, 0) represents the center of the circle of `radius = 1`
     *
     *  radius_{real} = \frac{max(height, width)}{2}
     *  \delta = point/radius_{real}
     */
    const QPointF delta = 2 * (_mousePos - center) / std::min(width(), height());

    // Check if mouse is inside circle
    if (hypotf(delta.x(), delta.y()) > 1) {
        _containsMouse = false;
        emit containsMouseChanged();
        return;
    }

    // Calculate the angle in degrees
    int grad = static_cast<int>(atan2f(-delta.x(), delta.y()) * rad2grad + 200) % 400;
    _mouseSampleAngle = grad * grad2deg;

    // Calculate mouse distance in meters
    _mouseSampleDistance = std::hypotf(delta.x(), delta.y()) * _maxDistance;

    emit mouseSampleAngleChanged();
    emit mouseSampleDistanceChanged();
}
