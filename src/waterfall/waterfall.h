#pragma once

#include <QImage>
#include <QQuickPaintedItem>

#include "logger.h"
#include "ringvector.h"
#include "waterfallgradient.h"

Q_DECLARE_LOGGING_CATEGORY(waterfall)

/**
 * @brief Waterfall widget
 *
 */
class Waterfall : public QQuickPaintedItem {
    Q_OBJECT
public:
    /**
     * @brief Construct a new Waterfall object
     *
     * @param parent
     */
    Waterfall(QQuickItem* parent = nullptr);

    /**
     * @brief This is used by the qml pain event
     *  This paint the waterfall in qml
     *
     * @param painter
     */
    virtual void paint(QPainter* painter) = 0;

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
     * @brief Function that deals when the mouse is inside the waterfall
     *
     * @param event
     */
    void hoverMoveEvent(QHoverEvent* event);

    /**
     * @brief Function that executes when the mouse leaves the waterfall area
     *
     * @param event
     */
    void hoverLeaveEvent(QHoverEvent* event);

    /**
     * @brief Function that executes when the mouse enters the waterfall area
     *
     * @param event
     */
    void hoverEnterEvent(QHoverEvent* event);

    /**
     * @brief Clear waterfall and restart all parameters
     *
     */
    virtual Q_INVOKABLE void clear() = 0;

    /**
     * @brief Return mouse position
     *
     * @return QPoint
     */
    QPoint mousePos() { return _mousePos; }
    Q_PROPERTY(QPoint mousePos READ mousePos NOTIFY mousePosChanged)

    /**
     * @brief Return true if the mouse is currently inside the waterfall area
     *
     * @return bool
     */
    bool containsMouse() { return _containsMouse; }
    Q_PROPERTY(bool containsMouse READ containsMouse NOTIFY containsMouseChanged)

    /**
     * @brief Get theme name used in the waterfall
     *  Check WaterfallGradient
     *
     * @return const QString
     */
    const QString theme() { return _theme; }
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)

    /**
     * @brief Get all themes available
     *
     * @return const QStringList
     */
    const QStringList themes() { return _themes; }
    Q_PROPERTY(QStringList themes READ themes NOTIFY themesChanged)

    /**
     * @brief Check if smooth is on
     *
     * @return true
     * @return false
     */
    bool smooth() { return _smooth; }

    /**
     * @brief Set the smooth state
     *
     * @param smooth
     */
    void setSmooth(bool smooth)
    {
        _smooth = smooth;
        emit smoothChanged();
    }
    Q_PROPERTY(bool smooth READ smooth WRITE setSmooth NOTIFY smoothChanged)

    /**
     * @brief Set antialiasing proprieties
     *
     * @param antialiasing
     */
    void setAliasing(bool antialiasing)
    {
        setAntialiasing(antialiasing);
        emit antialiasingChanged();
    }
    Q_PROPERTY(bool antialiasing READ antialiasing WRITE setAliasing NOTIFY antialiasingChanged)

signals:
    void antialiasingChanged();

    void mouseConfidenceChanged();
    // TODO: mouseMove should be renamed
    void mouseMove();
    void mousePosChanged();
    void containsMouseChanged();
    void themeChanged();
    void themesChanged();
    void smoothChanged();

protected:
    bool _containsMouse;
    WaterfallGradient _gradient;
    static QList<WaterfallGradient> _gradients;
    QPoint _mousePos;
    bool _smooth;
    QString _theme;
    QStringList _themes;

private:
    Q_DISABLE_COPY(Waterfall)

    /**
     * @brief Set all gradients used for the themes
     *
     */
    void setGradients();

    /**
     * @brief Load user gradients
     *
     */
    void loadUserGradients();
};
