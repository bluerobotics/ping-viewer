#include <QQuickPaintedItem>
#include <QImage>

#include "logger.h"
#include "waterfallgradient.h"

Q_DECLARE_LOGGING_CATEGORY(waterfall)

class Waterfall : public QQuickPaintedItem
{
    Q_OBJECT

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
     * @brief Get signal strength from mouse position
     *
     * @return float
     */
    float mouseStrength() {return _mouseStrength;}
    Q_PROPERTY(float mouseStrength READ mouseStrength NOTIFY mouseStrengthChanged)

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
    void setSmooth(bool smooth) {_smooth = smooth; setAntialiasing(_smooth); emit smoothChanged();}
    Q_PROPERTY(bool smooth READ smooth WRITE setSmooth NOTIFY smoothChanged)

    QList<WaterfallGradient> _gradients;
    WaterfallGradient _gradient;
    QImage _image;
    QPainter *_painter;
    float _mouseDepth;
    float _mouseStrength;
    bool _smooth;
    bool _update;
    QString _theme;
    QList<QString> _themes;
    static uint16_t displayWidth;
    uint16_t currentDrawIndex;

signals:
    void imageChanged();
    void mouseDepthChanged();
    void mouseMove();
    void mouseStrengthChanged();
    void mouseLeave();
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
     */
    Q_INVOKABLE void draw(const QList<double>& points);

    /**
     * @brief Function that deals when the mouse is inside the waterfall
     *
     * @param event
     */
    void hoverMoveEvent(QHoverEvent *event);

    /**
     * @brief Function that deals when the mouse is outside the waterfall
     *
     * @param event
     */
    void hoverLeaveEvent(QHoverEvent *event);
};
