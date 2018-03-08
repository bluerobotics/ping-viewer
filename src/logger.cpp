#include "logger.h"

#include <iostream>
#include <QDebug>
#include <QString>
#include <QTime>
#include <QtConcurrent>
#include <QColor>

PING_LOGGING_CATEGORY(logger, "ping.logger")

static QtMessageHandler originalHandler = nullptr;

QVariant LogListModel::data(const QModelIndex & index, int role) const
{
    switch (role) {
    case Qt::ForegroundRole :
        {
            auto itr = _rowColors.find(index.row());
            if (itr != _rowColors.end()) {
                return itr->second;
            }
        }
        break;
    case LogListModel::TimeRole :
        {
            auto itr = _rowTimes.find(index.row());
            if (itr != _rowTimes.end()) {
                return itr->second;
            }
        }
        break;
    default:
        break;
    }

    return QStringListModel::data(index, role);
}

bool LogListModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    QVector<int> roles(4);
    roles.append(Qt::DisplayRole);
    roles.append(Qt::EditRole);
    roles.append(Qt::ForegroundRole);
    roles.append(LogListModel::TimeRole);

    switch (role) {
    case (Qt::ForegroundRole) :
        _rowColors[index.row()] = value.value<QColor>();
        emit dataChanged(index, index, roles);
        return true;
    case (LogListModel::TimeRole) :
        _rowTimes[index.row()] = value.value<QString>();
        emit dataChanged(index, index, roles);
        return true;
    default :
        break;
    }

    return QStringListModel::setData(index, value, role);
}

QHash<int, QByteArray> LogListModel::roleNames() const
{
    QHash<int, QByteArray> ret = QStringListModel::roleNames();
    ret.insert(Qt::ForegroundRole, "foreground");
    ret.insert(LogListModel::TimeRole, "time");
    return ret;
}

Logger::Logger():
    _settings("Blue Robotics Inc.", "Ping Viewer")
{
    if(_settings.contains("filter")) {
        QString filter = _settings.value("filter").toString();
        QLoggingCategory::setFilterRules(filter);
    }

    registerCategory("qml");
}

void Logger::installHandler()
{
    originalHandler = qInstallMessageHandler(handleMessage); // This function returns the previous message handler
}

void Logger::logMessage(const QString& msg, QtMsgType type)
{
    const QString time = QTime::currentTime().toString(QStringLiteral("[hh:mm:ss:zzz]"));

    const int line = _logModel.rowCount();
    // Debug, Warning, Critical, Fatal, Info
    static const QColor colors[] = { QColor("lightGrey"), QColor("orange"), QColor("red"), QColor("red"), QColor("grey") };

    _logModel.insertRows(line, 1);
    _logModel.setData(_logModel.index(line), time, LogListModel::TimeRole);
    _logModel.setData(_logModel.index(line), QString(msg).replace("\\n", "\n"), Qt::DisplayRole);
    _logModel.setData(_logModel.index(line), colors[type], Qt::ForegroundRole);
}

void Logger::handleMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    static const QString msgTypes[] = { "Debug", "Warning", "Critical", "Fatal", "Info" };
    const QString file = QString(context.file).split('/').last();
    const QString logMsg = QString("%2: %3 at %4(%5) %6").arg(msgTypes[type]).arg(context.category).arg(file).arg(context.line).arg(msg);

    Logger::self()->logMessage(logMsg, type);
    if (originalHandler) {
        originalHandler(type, context, logMsg);
    }
}

void Logger::registerCategory(const char* category)
{
    _registeredCategories << category;
    qCDebug(logger) << category << _settings.value(category).toBool();
    emit registeredCategoryChanged();
}

void Logger::setCategory(QString category, bool enable)
{
    qCDebug(logger) << category << enable;
    _settings.setValue(category, enable);

    QString filter;
    for(const auto ourCategory : _registeredCategories) {
        filter += QStringLiteral("%1.debug=%2\n").arg(ourCategory, _settings.value(ourCategory).toBool() ? "true" : "false");
    }

    qCDebug(logger) << "filter" << filter;
    QLoggingCategory::setFilterRules(filter);
    _settings.setValue("filter", filter);
}

bool Logger::getCategory(QString category)
{
    qCDebug(logger) << category << _settings.value(category).toBool();
    return _settings.value(category).toBool();
};

Logger* Logger::self()
{
    static Logger* self = new Logger();
    return self;
}

void Logger::test()
{
    qInfo() << "This is an info message";
    qDebug() << "This is a debug message";
    qWarning() << "This is a warning message";
    qCritical() << "This is a critical message";
}

Logger::~Logger()
{
    QString filter;
    for(const auto ourCategory : _registeredCategories) {
        filter += QStringLiteral("%1.debug=%2\n").arg(ourCategory, _settings.value(ourCategory).toBool() ? "true" : "false");
    }
    _settings.setValue("filter", filter);
}
