#include <QLinearGradient>
#include <QtDebug>

#include "logger.h"

Q_DECLARE_LOGGING_CATEGORY(waterfallGradient)

/**
 * @brief Manage gradients for waterfall
 *
 */
class WaterfallGradient : public QLinearGradient
{
    QString _name;
    bool _isOk = false;
public:
    /**
     * @brief Construct a new Waterfall Gradient object
     *
     * @param name
     * @param colors
     */
    WaterfallGradient(QString name = QString(), QVector<QColor> colors = QVector<QColor>());

    /**
     * @brief Construct a new Waterfall Gradient object with QFile
     *
     * @param file
     */
    WaterfallGradient(QFile &file);

    /**
     * @brief Check if gradient is valid
     *
     * @return true
     * @return false
     */
    bool isOk() { return _isOk; };

    /**
     * @brief Set the Colors object
     *
     * @param colors
     */
    void setColors(const QVector<QColor>& colors);

    /**
     * @brief Set the gradient name
     *
     * @param name
     */
    void setName(const QString& name) ;

    /**
     * @brief Return gradient name
     *
     * @return QString
     */
    QString name() const;

    /**
     * @brief Get color from float value 0-1
     *
     * @param value
     * @return QColor
     */
    QColor getColor(float value) const;

    /**
     * @brief Get value from color 0-0-0 to 255-255-255
     *
     * @param color
     * @return float
     */
    float getValue(const QColor& color) const;

    /**
     * @brief Check if color exist between color1 and color2
     *
     * @param color
     * @param color1
     * @param color2
     * @return true when color in between color1 and color2
     * @return false when color is not between color1 and color2
     */
    bool colorsInRange(const QColor& color, const QColor& color1, const QColor& color2) const;

    /**
     * @brief Get a QColor from a linear interpolation from color1 and color2 using the value
     *  where 1 is color2 and 0 color1
     *
     * @param value
     * @param color1
     * @param color2
     * @return QColor
     */
    QColor colorLinearInterpolation(float value, const QGradientStop& color1, const QGradientStop& color2) const;

    /**
     * @brief Get value from color interpolation
     *  It'll return 1 if color is equal to color2 or 0 if color is color1, otherwise will be done a linear interpolation
     *
     * @param color
     * @param color1
     * @param color2
     * @return float
     */
    float valueLinearInterpolation(const QColor& color, const QGradientStop& color1, const QGradientStop& color2) const;
};