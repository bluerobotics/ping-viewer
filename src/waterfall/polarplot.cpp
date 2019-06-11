#include "filemanager.h"
#include "polarplot.h"

#include <limits>

#include <QtConcurrent>
#include <QPainter>
#include <QtMath>
#include <QVector>

PING_LOGGING_CATEGORY(polarplot, "ping.polarplot")

// Number of samples to display
uint16_t PolarPlot::_angularResolution = 400;

PolarPlot::PolarPlot(QQuickItem *parent)
    :Waterfall(parent)
    ,_image(2500, 2500, QImage::Format_RGBA8888)
    ,_painter(nullptr)
    ,_updateTimer(new QTimer(this))
{
    // This is the max depth that ping returns
    _DCRing.fill({static_cast<float>(_image.height()), 0, 0, 0}, _angularResolution);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    _image.fill(QColor(Qt::transparent));

    connect(_updateTimer, &QTimer::timeout, this, [&] {update();});
    _updateTimer->setSingleShot(true);
    _updateTimer->start(50);

    connect(this, &Waterfall::mousePosChanged, this, &PolarPlot::updateMouseColumnData);
}

void PolarPlot::clear()
{
    qCDebug(polarplot) << "Cleaning waterfall and restarting internal variables";
    _DCRing.fill({static_cast<float>(_image.height()), 0, 0, 0}, _angularResolution);
    _image.fill(Qt::transparent);
}

void PolarPlot::paint(QPainter *painter)
{
    static QPixmap pix;
    if(painter != _painter) {
        _painter = painter;
    }

    // http://blog.qt.io/blog/2006/05/13/fast-transformed-pixmapimage-drawing/
    pix = QPixmap::fromImage(_image, Qt::NoFormatConversion);
    _painter->drawPixmap(_painter->viewport(), pix, QRect(0, 0, _image.width(), _image.height()));
}

void PolarPlot::setImage(const QImage &image)
{
    _image = image;
    emit imageChanged();
    setImplicitWidth(image.width());
    setImplicitHeight(image.height());
}

void PolarPlot::draw(const QVector<double>& points, float confidence, float initPoint, float length, float distance)
{
    static float currentDrawAngle(0);
    static const QPoint center(_image.width()/2, _image.height()/2);
    static const float d2r = M_PI/180.0;
    static QColor pointColor;
    static float step;
    static float angleStep;
    float actualAngle = currentDrawAngle*d2r;

    // This ring vector will store variables of the last n samples for user access
    static const float rad2grad = _angularResolution/(2*M_PI);
    _DCRing[int(actualAngle*rad2grad) % _angularResolution] = {initPoint, length, confidence, distance};

    const float linearFactor = points.size()/(float)center.x();
    for(int i = 0; i < center.x(); i++) {
        pointColor = valueToRGB(points[static_cast<int>(i*linearFactor)]);
        step = ceil(i*2*d2r)*1.15;
        for(float u = -0.25; u <= 0.25; u += 1/step) {
            angleStep = u*d2r+actualAngle - M_PI_2;
            _image.setPixelColor(center.x() + i*cos(angleStep), center.y() + i*sin(angleStep), pointColor);
        }
    }

    currentDrawAngle += 0.5;
    // Fix max update in 20Hz at max
    if(!_updateTimer->isActive()) {
        _updateTimer->start(50);
    }
}

void PolarPlot::updateMouseColumnData()
{
    static const QPoint center(width()/2, height()/2);
    static const float rad2grad = 200/M_PI;

    const QPoint delta = _mousePos - center;
    // Check if mouse is inside circle
    if(hypotf(delta.x(), delta.y()) > std::min(width(), height())/2) {
        _containsMouse = false;
        emit containsMouseChanged();
        return;
    }

    // Calculate grad from mouse position
    const QPoint deltaScaled(delta.x()*_image.width()/width(), delta.y()*_image.height()/height());
    int grad = static_cast<int>(atan2f(-deltaScaled.x(), deltaScaled.y())*rad2grad + 200) % 400;

    const auto& depthAndConfidence = _DCRing[grad];
    _mouseSampleConfidence = depthAndConfidence.confidence;
    _mouseSampleDepth = depthAndConfidence.distance;
    emit mouseSampleConfidenceChanged();
    emit mouseSampleDepthChanged();
}
