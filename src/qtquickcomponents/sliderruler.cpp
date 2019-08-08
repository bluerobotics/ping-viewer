#include "sliderruler.h"
#include "stylemanager.h"

// std
#include <cmath>

// Qt
#include <QFontMetrics>
#include <QPainter>
#include <QPalette>

SliderRuler::SliderRuler(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_count(-1)
    , m_showText(true)
{
}

void SliderRuler::setCount(int count)
{
    if (count != m_count) {
        m_count = count;
        emit countChanged(m_count);
        update();
    }
}

int SliderRuler::count() const
{
    return m_count;
}

void SliderRuler::setShowText(bool showText)
{
    if (showText != m_showText) {
        m_showText = showText;
        emit showTextChanged(m_showText);
        update();
    }
}

bool SliderRuler::showText() const
{
    return m_showText;
}

void SliderRuler::setTo(double to)
{
    if (!qFuzzyCompare(to, m_to)) {
        m_to = to;
        emit toChanged(m_to);
        update();
    }
}

double SliderRuler::to() const
{
    return m_to;
}

void SliderRuler::setFrom(double from)
{
    if (!qFuzzyCompare(from, m_from)) {
        m_from = from;
        emit fromChanged(m_from);
        update();
    }
}

double SliderRuler::from() const
{
    return m_from;
}

QPointF SliderRuler::beginLine() const
{
    return m_beginLine;
}

QPointF SliderRuler::endLine() const
{
    return m_endLine;
}

void SliderRuler::setColor(const QColor& color)
{
    if(m_controlColor != color) {
        m_controlColor = color;
        emit colorChanged(m_controlColor);
        update();
    }
}

void SliderRuler::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    Q_UNUSED(oldGeometry)

    const QRectF rect = newGeometry.adjusted(1, 1, -1, -1);
    const auto height = rect.height();
    const auto middlePoint = height / 2;

    QFontMetrics fMetrics(qApp->font());
    auto beginSize = fMetrics.horizontalAdvance(QString::number(qRound(m_from)));
    auto endSize = fMetrics.horizontalAdvance(QString::number(qRound(m_to)));

    const auto width = rect.width() - (beginSize / 2) - (endSize / 2);

    m_beginLine = QPointF(rect.x() + beginSize / 2, middlePoint);
    m_endLine = QPointF(m_beginLine.x() + width, middlePoint);
    emit beginLineChanged(m_beginLine);
    emit endLineChanged(m_endLine);
    update();
}

void SliderRuler::paint(QPainter *painter)
{
    painter->save();

    static constexpr int bitTickSize = 10;
    static constexpr int smallTickSise = 4;

    QFontMetrics fMetrics(qApp->font());

    // Do some borders.
    const QRectF rect = boundingRect().adjusted(1, 1, -1, -1);
    const auto height = rect.height();
    const auto middlePoint = height / 2;

    const auto width = m_endLine.x() - m_beginLine.x();

    // the number of spaces:
    // you can set the total divisions by calling setCount
    // if you don't we try to pick up a number.
    const int spaceCount = m_count != -1 ? m_count
                           : m_to - m_from > 50 ? 50
                           : static_cast<int>(m_to - m_from);

    const int tickCount = spaceCount + 1;

    const double spaceValue = (m_to - m_from) / static_cast<double>(spaceCount);
    const double textHeight = fMetrics.height();

    painter->setPen(m_controlColor);
    painter->drawLine(m_beginLine, m_endLine);

    // add the ticks and the text.
    for(int i = 0; i < tickCount; i++) {
        const bool isBigTick = spaceCount > 20 ? i % 10 == 0 : true;
        if (!isBigTick)
            continue;

        // one tick per division. this calculates the X position
        const double tickX = ((width / static_cast<double>(spaceCount)) * i) + m_beginLine.x();
        painter->drawLine(QLineF(tickX, middlePoint, tickX, middlePoint + (isBigTick ? bitTickSize : smallTickSise)));

        if (m_showText && isBigTick) {
            // Get the current value
            const QString currentText = QString::number(std::round(spaceValue * i + m_from));

            // calculate the position of the text. it should be positioned in the middle of the tick.
            const double middleWidth = fMetrics.horizontalAdvance(currentText) / 2;
            const QPointF textPoint = QPointF(tickX - middleWidth, middlePoint + textHeight + 5);

            painter->drawText(textPoint, currentText);
        }
    }

    painter->restore();
}
