#include "gradientscale.h"
#include <QPainter>

PING_LOGGING_CATEGORY(gradientscale, "ping.gradientscale")

GradientScale::GradientScale(QQuickItem* parent)
    : QQuickPaintedItem(parent)
    , _image(100, 1, QImage::Format_RGBA8888)
{
    _image.fill(Qt::black);
}

void GradientScale::paint(QPainter* painter)
{
    if (!painter) {
        return;
    }
    painter->drawImage(QRect(0, 0, width(), height()), _image);
}

void GradientScale::setWaterfallGradient(WaterfallGradient* gradient)
{
    if (!gradient) {
        return;
    }

    for (int i {0}; i < _image.width(); i++) {
        _image.setPixelColor(i, 0, gradient->getColor(i / static_cast<float>(_image.width())));
    }
    update();
}
