#pragma once

#include <QImage>
#include <QQuickPaintedItem>

#include "waterfallgradient.h"

Q_DECLARE_LOGGING_CATEGORY(gradientscale)

/**
 * @brief GradientScale widget
 *
 */
class GradientScale : public QQuickPaintedItem {
    Q_OBJECT
public:
    /**
     * @brief Construct a new GradientScale object
     *
     * @param parent
     */
    GradientScale(QQuickItem* parent = nullptr);

    /**
     * @brief This is used by the qml pain event
     *
     * @param painter
     */
    void paint(QPainter* painter) override final;

    /**
     * @brief Set the Waterfall Gradient object
     *
     * @param gradient
     */
    void setWaterfallGradient(WaterfallGradient* gradient);
    Q_PROPERTY(WaterfallGradient* waterfallGradient WRITE setWaterfallGradient)

private:
    Q_DISABLE_COPY(GradientScale)

    QImage _image;
};
