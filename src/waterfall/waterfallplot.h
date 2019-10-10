#pragma once

#include <QImage>
#include <QQuickPaintedItem>

#include "logger.h"
#include "ringvector.h"
#include "waterfall.h"
#include "waterfallgradient.h"

Q_DECLARE_LOGGING_CATEGORY(waterfall)

/**
 * @brief Waterfall widget
 *
 */
class WaterfallPlot : public Waterfall {
    Q_OBJECT

public:
    /**
     * @brief Construct a new WaterfallPlot object
     *
     * @param parent
     */
    WaterfallPlot(QQuickItem* parent = nullptr);

    /**
     * @brief This is used by the qml pain event
     *  This paint the waterfall in qml
     *
     * @param painter
     */
    void paint(QPainter* painter) final override;

    /**
     * @brief Set the waterfall Image
     *
     * @param image
     */
    void setImage(const QImage& image);

    /**
     * @brief Set the waterfall max depth in meters
     *
     * @param maxDepth
     */
    Q_INVOKABLE void setWaterfallMaxDepth(float maxDepth);

    /**
     * @brief Draw a list of points in the waterfall
     *
     * @param points
     * @param confidence
     * @param initPoint
     * @param length
     * @param distance
     */
    Q_INVOKABLE void draw(const QVector<double>& points, float confidence = 0, float initPoint = 0, float length = 50,
        float distance = 0);

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
    QImage image() { return _image; }
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)

    /**
     * @brief Get depth from mouse position
     *
     * @return float
     */
    float mouseDepth() { return _mouseDepth; }
    Q_PROPERTY(float mouseDepth READ mouseDepth NOTIFY mouseDepthChanged)

    /**
     * @brief Get signal confidence from mouse position column
     *
     * @return float
     */
    float mouseColumnConfidence() { return _mouseColumnConfidence; }
    Q_PROPERTY(float mouseColumnConfidence READ mouseColumnConfidence NOTIFY mouseColumnConfidenceChanged)

    /**
     * @brief Get signal depth from mouse position column
     *
     * @return float
     */
    float mouseColumnDepth() { return _mouseColumnDepth; }
    Q_PROPERTY(float mouseColumnDepth READ mouseColumnDepth NOTIFY mouseColumnDepthChanged)

    /**
     * @brief Return max depth in waterfall at the moment in meters
     *
     */
    Q_INVOKABLE float getMaxDepthToDraw() { return _maxDepthToDraw; }
    Q_PROPERTY(float maxDepthToDraw READ getMaxDepthToDraw NOTIFY maxDepthToDrawChanged)

    /**
     * @brief Return min depth in waterfall at the moment in meters
     *
     */
    Q_INVOKABLE float getMinDepthToDraw() { return _minDepthToDraw; }
    Q_PROPERTY(float minDepthToDraw READ getMinDepthToDraw NOTIFY minDepthToDrawChanged)

signals:
    void imageChanged();
    void maxDepthToDrawChanged();
    void minDepthToDrawChanged();
    void mouseColumnConfidenceChanged();
    void mouseColumnDepthChanged();
    void mouseDepthChanged();

private:
    Q_DISABLE_COPY(WaterfallPlot)

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

    uint16_t _currentDrawIndex;
    static uint16_t _displayWidth;
    QImage _image;
    float _maxDepthToDraw;
    float _maxDepthToDrawInPixels;
    float _minDepthToDraw;
    float _minDepthToDrawInPixels;
    float _minPixelsPerMeter;
    float _mouseColumnConfidence;
    float _mouseColumnDepth;
    float _mouseDepth;
    QPainter* _painter;
    QTimer* _updateTimer;
    float _waterfallDepth;

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

    RingVector<DCPack> _DCRing;
};
