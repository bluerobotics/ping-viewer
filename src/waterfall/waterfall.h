#pragma once

#include <QQuickPaintedItem>
#include <QImage>

#include "logger.h"
#include "ringvector.h"
#include "waterfallgradient.h"

Q_DECLARE_LOGGING_CATEGORY(waterfall)

/**
 * @brief Waterfall widget
 *
 */
class Waterfall : public QQuickPaintedItem
{
    Q_OBJECT
public:
    /**
     * @brief Clear waterfall and restart all parameters
     *
     */
    Q_INVOKABLE void clear();

    /**
     * @brief Return waterfall image
     *
     * @return QImage
     */
    QImage image() {return _image;}
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)

    /**
     * @brief Get depth from mouse position
     *
     * @return float
     */
    float mouseDepth() {return _mouseDepth;}
    Q_PROPERTY(float mouseDepth READ mouseDepth NOTIFY mouseDepthChanged)

    /**
     * @brief Get signal confidence from mouse position column
     *
     * @return float
     */
    float mouseColumnConfidence() {return _mouseColumnConfidence;}
    Q_PROPERTY(float mouseColumnConfidence READ mouseColumnConfidence NOTIFY mouseColumnConfidenceChanged)

    /**
     * @brief Get signal deepth from mouse position column
     *
     * @return float
     */
    float mouseColumnDepth() {return _mouseColumnDepth;}
    Q_PROPERTY(float mouseColumnDepth READ mouseColumnDepth NOTIFY mouseColumnDepthChanged)

    /**
     * @brief Return mouse position
     *
     * @return QPoint
     */
    QPoint mousePos() {return _mousePos;}
    Q_PROPERTY(QPoint mousePos READ mousePos NOTIFY mousePosChanged)

    /**
     * @brief Return true if the mouse is currently inside the waterfall area
     *
     * @return bool
     */
    bool containsMouse() {return _containsMouse;}
    Q_PROPERTY(bool containsMouse READ containsMouse NOTIFY containsMouseChanged)

    /**
     * @brief Get theme name used in the waterfall
     *  Check WaterfallGradient
     *
     * @return const QString
     */
    const QString theme() {return _theme;}
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)

    /**
     * @brief Get all themes available
     *
     * @return const QStringList
     */
    const QStringList themes() {return _themes;}
    Q_PROPERTY(QStringList themes READ themes NOTIFY themesChanged)

    /**
     * @brief Check if smooth is on
     *
     * @return true
     * @return false
     */
    bool smooth() {return _smooth;}

    /**
     * @brief Set the smooth state
     *
     * @param smooth
     */
    void setSmooth(bool smooth) {_smooth = smooth; emit smoothChanged();}
    Q_PROPERTY(bool smooth READ smooth WRITE setSmooth NOTIFY smoothChanged)

    /**
     * @brief Set antialiasing proprieties
     *
     * @param antialiasing
     */
    void setAliasing(bool antialiasing) {setAntialiasing(antialiasing); emit antialiasingChanged();}
    Q_PROPERTY(bool antialiasing READ antialiasing WRITE setAliasing NOTIFY antialiasingChanged)

    /**
     * @brief Return max depth in waterfall at the moment in meters
     *
     */
    Q_INVOKABLE float getMaxDepthToDraw() {return _maxDepthToDraw;}
    Q_PROPERTY(float maxDepthToDraw READ getMaxDepthToDraw NOTIFY maxDepthToDrawChanged)

    /**
     * @brief Return min depth in waterfall at the moment in meters
     *
     */
    Q_INVOKABLE float getMinDepthToDraw() {return _minDepthToDraw;}
    Q_PROPERTY(float minDepthToDraw READ getMinDepthToDraw NOTIFY minDepthToDrawChanged)

    QVector<WaterfallGradient> _gradients;
    WaterfallGradient _gradient;
    QImage _image;
    QPainter *_painter;
    float _minPixelsPerMeter;
    float _maxDepthToDraw;
    float _maxDepthToDrawInPixels;
    float _minDepthToDraw;
    float _minDepthToDrawInPixels;
    float _mouseColumnConfidence;
    float _mouseColumnDepth;
    float _mouseDepth;
    bool _containsMouse;
    QPoint _mousePos;
    bool _smooth;
    QTimer* _updateTimer;
    QString _theme;
    QStringList _themes;
    static uint16_t displayWidth;
    uint16_t currentDrawIndex;
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

signals:
    void antialiasingChanged();
    void imageChanged();
    void minDepthToDrawChanged();
    void maxDepthToDrawChanged();
    void mouseDepthChanged();
    void mouseColumnConfidenceChanged();
    void mouseColumnDepthChanged();
    // TODO: mouseMove should be renamed
    void mouseMove();
    void mousePosChanged();
    void containsMouseChanged();
    void themeChanged();
    void themesChanged();
    void smoothChanged();

public:
    /**
     * @brief Construct a new Waterfall object
     *
     * @param parent
     */
    Waterfall(QQuickItem *parent = nullptr);

    /**
     * @brief This is used by the qml pain event
     *  This paint the waterfall in qml
     *
     * @param painer
     */
    void paint(QPainter *painer);

    /**
     * @brief Set the waterfall Image
     *
     * @param image
     */
    void setImage(const QImage &image);

    /**
     * @brief Set all gradients used for the themes
     *
     */
    void setGradients();

    /**
     * @brief Set the waterfall max depth in meters
     *
     * @param maxDepth
     */
    Q_INVOKABLE void setWaterfallMaxDepth(float maxDepth);

    /**
     * @brief Change the theme used in the waterfall
     *
     * @param theme name
     */
    void setTheme(const QString& theme);

    /**
     * @brief Transform a power value 0-1 to color
     *
     * @param point
     * @return QColor
     */
    QColor valueToRGB(float point);

    /**
     * @brief Transform color to a power value
     *
     * @param color
     * @return float
     */
    float RGBToValue(const QColor& color);

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
     * @brief Function that deals when the mouse is inside the waterfall
     *
     * @param event
     */
    void hoverMoveEvent(QHoverEvent *event);

    /**
     * @brief Function that executes when the mouse leaves the waterfall area
     *
     * @param event
     */
    void hoverLeaveEvent(QHoverEvent *event);

    /**
     * @brief Function that executes when the mouse enters the waterfall area
     *
     * @param event
     */
    void hoverEnterEvent(QHoverEvent *event);

private:
    Q_DISABLE_COPY(Waterfall)
    /**
     * @brief Load user gradients
     *
     */
    void loadUserGradients();
};
