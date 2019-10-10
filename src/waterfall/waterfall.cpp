#include "waterfall.h"
#include "filemanager.h"

#include <limits>

#include <QPainter>
#include <QVector>
#include <QtConcurrent>
#include <QtMath>

PING_LOGGING_CATEGORY(waterfall, "ping.waterfall")

QList<WaterfallGradient> Waterfall::_gradients {
    {QStringLiteral("Thermal blue"),
        {
            QColor(5, 34, 95),
            QColor(106, 168, 79),
            QColor(255, 255, 0),
            QColor(127, 96, 0),
            QColor(92, 15, 8),
        }},
    {QStringLiteral("Thermal black"),
        {
            Qt::black,
            QColor(106, 168, 79),
            QColor(255, 255, 0),
            QColor(127, 96, 0),
            QColor(92, 15, 8),
        }},
    {QStringLiteral("Thermal white"),
        {
            Qt::white,
            QColor(106, 168, 79),
            QColor(255, 255, 0),
            QColor(127, 96, 0),
            QColor(92, 15, 8),
        }},
    {QStringLiteral("Monochrome black"),
        {
            Qt::black,
            Qt::white,
        }},
    {QStringLiteral("Monochrome white"),
        {
            Qt::white,
            Qt::black,
        }},
    {QStringLiteral("Monochrome sepia"),
        {
            QColor(48, 33, 19),
            QColor(232, 201, 67),
        }},
};

Waterfall::Waterfall(QQuickItem* parent)
    : QQuickPaintedItem(parent)
    , _containsMouse(false)
    , _smooth(true)
{
    setAntialiasing(_smooth);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setGradients();
    setTheme("Thermal 5");
}

void Waterfall::setGradients()
{
    loadUserGradients();

    for (auto& gradient : _gradients) {
        _themes.append(gradient.name());
    }
    qCDebug(waterfall) << "Gradients:" << _themes;
    emit themesChanged();
}

void Waterfall::loadUserGradients()
{
    auto fileInfoList = FileManager::self()->getFilesFrom(FileManager::Folder::Gradients);
    for (auto& fileInfo : fileInfoList) {
        qCDebug(waterfall) << fileInfo.fileName();
        QFile file(fileInfo.absoluteFilePath());
        WaterfallGradient gradient(file);
        if (!gradient.isOk()) {
            qCDebug(waterfall) << "Invalid gradient file:" << fileInfo.fileName();
            continue;
        }
        if (std::any_of(_gradients.cbegin(), _gradients.cend(),
                [&](const auto& gradientItem) { return gradientItem.name() == gradient.name(); })) {
            qCDebug(waterfall) << "Gradient already exist:" << gradient.name() << fileInfo.fileName();
            continue;
        }
        _gradients.append(gradient);
    }
}

void Waterfall::setTheme(const QString& theme)
{
    for (auto& gradient : _gradients) {
        if (gradient.name() == theme) {
            _gradient = gradient;
            _theme = theme;
            emit themeChanged();
            return;
        }
    }
    qCWarning(waterfall) << "Not valid theme:" << theme << " in:" << _themes;
}

QColor Waterfall::valueToRGB(float point) { return _gradient.getColor(point); }

float Waterfall::RGBToValue(const QColor& color) { return _gradient.getValue(color); }

void Waterfall::hoverMoveEvent(QHoverEvent* event)
{
    event->accept();
    _mousePos = event->pos();
    _containsMouse = true;
    emit mousePosChanged();
    emit containsMouseChanged();
}

void Waterfall::hoverLeaveEvent(QHoverEvent* event)
{
    Q_UNUSED(event)
    _containsMouse = false;
    emit containsMouseChanged();
}

void Waterfall::hoverEnterEvent(QHoverEvent* event)
{
    Q_UNUSED(event)
    _containsMouse = true;
    emit containsMouseChanged();
}
