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
    , _backgroundImage(1, 1, QImage::Format_RGBA8888)
    , _distances(_angularResolution, 0)
    , _image(2500, 2500, QImage::Format_RGBA8888)
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
    connect(this, &QQuickItem::widthChanged, this, &PolarPlot::updateMask);
    connect(this, &QQuickItem::heightChanged, this, &PolarPlot::updateMask);
    connect(this, &PolarPlot::sectorSizeDegreesChanged, this, &PolarPlot::updateMask);
}

void PolarPlot::updateMask()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    _polarBackgroundMask.clear();
#else
    QPainterPath newPath;
    _polarBackgroundMask = newPath;
#endif
    // Create a centered pizza path that has dynamic angles
    _polarBackgroundMask.moveTo({width() / 2, height() / 2});
    _polarBackgroundMask.arcTo({0, 0, width(), height()}, -_sectorSizeDegrees / 2 + 90, _sectorSizeDegrees);
}

void PolarPlot::clear()
{
    qCDebug(polarplot) << "Cleaning waterfall and restarting internal variables";
    _image.fill(Qt::transparent);
    _distances.fill(0, _angularResolution);
    _backgroundImage.fill(valueToRGB(0));
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
    _painter->setClipPath(_polarBackgroundMask);
    _painter->drawImage(QRect(0, 0, width(), height()), _backgroundImage);
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
    static const QPoint center(_image.width() / 2, _image.height() / 2);
    static const float degreeToRadian = M_PI / 180.0f;
    static const float gradianToRadian = M_PI / 200.0f;
    static QColor pointColor;
    static float step;
    static float angleStep;

    const float actualAngle = angle * gradianToRadian;
    const float halfSection = sectorSize * degreeToRadian / 2;

    if (_sectorSizeDegrees != sectorSize) {
        _sectorSizeDegrees = sectorSize;
        emit sectorSizeDegreesChanged();
    }

    // TODO: Need a better way to deal with dynamic steps, maybe doing `draw(data, angle++)` with `angleGrad` loop
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

    const float linearFactor = points.size() / (float)center.x();
    for (int i = 1; i < center.x(); i++) {
        if (i < center.x() * length / _maxDistance) {
            pointColor = valueToRGB(points[static_cast<int>(i * linearFactor - 1)]);
        } else {
            pointColor = QColor(0, 0, 0, 0);
        }
        step = ceil(i * 3 * angleGrad * gradianToRadian);
        // The math and logic behind this loop is done in a way that the interaction is done with ints
        for (int currentStep = 0; currentStep <= step; currentStep++) {
            float deltaRadian = (angleGrad * gradianToRadian / (float)step) * (currentStep - step / 2);
            float calculatedAngle = deltaRadian + actualAngle;
            angleStep = calculatedAngle - M_PI_2;

            // Check if we are outside part of the chart
            if (calculatedAngle > halfSection && calculatedAngle < 2 * M_PI - halfSection) {
                continue;
            }
            _image.setPixelColor(center.x() + i * cos(angleStep), center.y() + i * sin(angleStep), pointColor);
        }
    }

    // Fix max update in 20Hz at max
    if (!_updateTimer.isActive()) {
        _updateTimer.start(50);
    }
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
