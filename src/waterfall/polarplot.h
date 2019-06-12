#pragma once

#include <QQuickPaintedItem>
#include <QImage>

#include "logger.h"
#include "ringvector.h"
#include "waterfall.h"
#include "waterfallgradient.h"

Q_DECLARE_LOGGING_CATEGORY(waterfall)

/**
 * @brief Polar widget
 *
 */
class PolarPlot : public Waterfall
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new PolarPlot object
     *
     * @param parent
     */
    PolarPlot(QQuickItem *parent = nullptr);

    /**
     * @brief This is used by the qml paint event
     *  This paint the a polar waterfall in qml
     *
     * @param painter
     */
    void paint(QPainter *painter) final override;

    /**
     * @brief Set the polar Image
     *
     * @param image
     */
    void setImage(const QImage &image);

    /**
     * @brief Draw a list of points in the waterfall
     *
     * @param points
     * @param angle
     * @param initPoint
     * @param length
     */
    Q_INVOKABLE void draw(const QVector<double>& points, float angle = 0, float initPoint = 0, float length = 100);

    /**
     * @brief Clear waterfall and restart all parameters
     *
     */
    Q_INVOKABLE void clear() final override;

    /**
     * @brief Return waterfall image
     *
     * @return QImage
     */
    QImage image() {return _image;}
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)

    /**
     * @brief Get signal confidence from mouse position column
     *
     * @return float
     */
    float mouseSampleConfidence() {return _mouseSampleConfidence;}
    Q_PROPERTY(float mouseSampleConfidence READ mouseSampleConfidence NOTIFY mouseSampleConfidenceChanged)

    /**
     * @brief Get signal depth from mouse position sample
     *
     * @return float
     */
    float mouseSampleDepth() {return _mouseSampleDepth;}
    Q_PROPERTY(float mouseSampleDepth READ mouseSampleDepth NOTIFY mouseSampleDepthChanged)

signals:
    void imageChanged();
    void mouseSampleConfidenceChanged();
    void mouseSampleDepthChanged();

private:
    Q_DISABLE_COPY(PolarPlot)

    /**
     * @brief Load user gradients
     *
     */
    void loadUserGradients();

    /**
     * @brief Update mouse column information
     *
     */
    void updateMouseColumnData();

    QImage _image;
    float _mouseSampleConfidence;
    float _mouseSampleDepth;
    QPainter *_painter;
    static uint16_t _angularResolution;
    QTimer* _updateTimer;

    /**
     * @brief Depth and Confidence package
     *
     */
    struct DCPack {
        float initialDepth;
        float length;
        float confidence;
        float distance;
    };

    QVector<DCPack> _DCRing;
};
