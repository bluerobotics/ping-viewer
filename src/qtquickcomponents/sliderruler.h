#pragma once

#include <QQuickPaintedItem>

/**
 * @brief Display a ruler with ticks and labels
 */
class SliderRuler : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(double from READ from WRITE setFrom NOTIFY fromChanged)
    Q_PROPERTY(double to READ to WRITE setTo NOTIFY toChanged)
    Q_PROPERTY(bool showText READ showText WRITE setShowText NOTIFY showTextChanged)
    Q_PROPERTY(QPointF beginLine READ beginLine NOTIFY beginLineChanged)
    Q_PROPERTY(QPointF endLine READ endLine NOTIFY endLineChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:

    /**
     * @brief Constructs an empty SliderRuler.
     *  The SliderRuler can be used as a background item for sliders.
     */
    SliderRuler(QQuickItem *parent = nullptr);

    /**
     * @brief Return component color
     *
     * @return QColor
     */
    QColor color() const { return m_controlColor; };

    /**
     * @brief returns the amount of ticks.
     * @return the amount of ticks.
     */
    int count() const;

    /**
     * @brief returns if texts are being shown or not.
     * @return true if texts are being shown and false otherwise.
     */
    bool showText() const;

    /**
     * @brief The value of the first tick
     *
     * @return double representing the value of the first tick
     */
    double from() const;

    /**
     * @brief The value of the last tick
     *
     * @return double representing the value of the last tick
     */
    double to() const;

    /**
     * @brief The first point in the rectangle that the line uses to drawn
     *
     * @return The first point in the rectangle that the line uses to drawn
     */
    QPointF beginLine() const;

    /**
     * @brief The last point in the rectangle that the line uses to drawn
     *
     * @return The last point in the rectangle that the line uses to drawn
     */
    QPointF endLine() const;

    /**
     * @brief Called when a geometryChange ocours.
     * Recalculates the Line.
     */
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

public Q_SLOTS:
    /**
     * @brief Set the component color
     *
     * @param color
     */
    void setColor(const QColor& color);

    /**
     * @brief Set the amount of ticks
     *
     * Set the amount of ticks. if unused it's automatically calculated based on from / to.
     */
    void setCount(int count);

    /**
     * @brief Set if text should be visible or not.
     */
    void setShowText(bool showText);

    /**
     * @brief set the value of the last tick
     */
    void setTo(double to);

    /**
     * @brief set the value of the first tick
     */
    void setFrom(double from);

protected:
    /**
     * @brief refreshes the visuals
     */
    virtual void paint(QPainter *painter) override;

Q_SIGNALS:
    /**
     * @brief emitted when the color changes
     *
     * @param color
     */
    void colorChanged(QColor color);

    /**
     * @brief emitted when the showText changes
     */
    void showTextChanged(bool showText);

    /**
     * @brief emitted when the count changes
     */
    void countChanged(int count);

    /**
     * @brief emitted when the to changes
     */
    void toChanged(double to);

    /**
     * @brief emitted when the from changes
     */
    void fromChanged(double from);

    /**
     * @brief emitted when the beginLine changes
     */
    void beginLineChanged(const QPointF &beginLine);

    /**
     * @brief emitted when the endLine changes
     */
    void endLineChanged(const QPointF &endLine);

private:
    double m_from;
    double m_to;
    QPointF m_beginLine;
    QPointF m_endLine;
    int m_count;
    bool m_showText;
    QColor m_controlColor;
};
