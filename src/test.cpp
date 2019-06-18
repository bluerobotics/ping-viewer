#define private public
#define protected public

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QDebug>
#include <QRegularExpression>

#include "abstractlink.h"
#include "filemanager.h"
#include "linkconfiguration.h"
#include "logger.h"
#include "ping.h"
#include "settingsmanager.h"
#include "util.h"
#include "waterfall.h"

#include "test.h"

#include "ping-message-ping1d.h"

void Test::initTestCase()
{
    FileManager::self();
    SettingsManager::self();
}

void Test::fileManager()
{
    auto fileManager = FileManager::self();
    // Check if folders exist and if it's ok
    for(const auto& folder : fileManager->folderMap) {
        QVERIFY2(folder->dir.exists(),
                 qPrintable(QString("Folder does not exist: %1").arg(folder->dir.path())));

        // If folder does not have extension, nothing will be created inside it
        if(folder->extension.isEmpty()) {
            continue;
        }

        QVERIFY2(folder->ok,
                 qPrintable(QString("Folder exist but ok is invalid: %1").arg(folder->dir.path())));
    }

    // Create file name and check if it's valid
    // - file name need to follow FileManager._fileName string
    const QRegularExpression regex(QStringLiteral("[0-9]{8}-[0-9]{9}"));
    for(const auto key : fileManager->folderMap.keys()) {
        auto folder = fileManager->folderMap[key];
        if(folder->extension.isEmpty()) {
            continue;
        }

        const QString fileName = fileManager->createFileName(key);

        // Check if file is inside Documents
        QVERIFY2(fileName.contains(fileManager->_docDir.dir.absolutePath()),
                 qPrintable(QString("File is not inside the Documents folder: %1").arg(fileName)));

        // Check if file has the correct name
        QRegularExpressionMatch match = regex.match(fileName);
        QVERIFY2(match.hasMatch(),
                 qPrintable(QString("File name does not follow rules of FileManager: %1").arg(fileName)));

        // Check if file has the correct extension
        QVERIFY2(fileName.contains(folder->extension),
                 qPrintable(QString("File name does not have extension compatible with FileManager: %1").arg(fileName)));
    }

    // TODO: Populate gradients folder and test FileManager.getFilesFrom
}

void Test::ringVector()
{
    // Create RingVector
    RingVector<int> ring;
    int size = 100;
    // Populate and test it
    ring.fill(0, size);
    for(auto item : ring) {
        QVERIFY2(item == 0, qPrintable("Ring is not populated."));
    }

    // Add more $size numbers and check if it's working
    for(int i{0}; i < ring.length(); i++) {
        ring.append(i);
    }
    // The first element is in 99 and the last in 0
    for(int i{0}; i < ring.length(); i++) {
        QVERIFY2(ring[i] == size - 1 - i,
                 qPrintable(QString("Ring is not working: Ring[%2]=%1").arg(ring[i]).arg(i)));
    }
}

void Test::settingsManager()
{
    auto settingsManager = SettingsManager::self();
    // Check if settings is working
    // Bool
    QVERIFY2(!settingsManager->debugMode(), qPrintable("Debug mode should be false [Default value]."));
    settingsManager->debugMode(true);
    QVERIFY2(settingsManager->debugMode(), qPrintable("Debug mode should be true [Changed from false]."));
    settingsManager->debugMode(false);
    QVERIFY2(!settingsManager->debugMode(), qPrintable("Debug mode should be false [Changed from true]."));

    // JSONMODEL
    auto index = settingsManager->distanceUnitsIndex();
    // Check default distance unit
    QVERIFY2(index == 0,
             qPrintable(QString("Meters is not default: %1").arg(index)));
    auto name = settingsManager->distanceUnits()[QString("name")].toString();
    QVERIFY2(name.contains("Metric"),
             qPrintable(QString("Distance unit name is wrong: %1").arg(name)));
    auto scalar = settingsManager->distanceUnits()[QString("distanceScalar")].toDouble();
    QVERIFY2(qFuzzyCompare(scalar, 1),
             qPrintable(QString("Distance scalar in meters is wrong: %1").arg(scalar)));

    // Change to imperial
    settingsManager->distanceUnitsIndex(1);
    index = settingsManager->distanceUnitsIndex();
    QVERIFY2(index == 1,
             qPrintable(QString("Not changed to impérial: %1").arg(index)));
    name = settingsManager->distanceUnits()[QString("name")].toString();
    QVERIFY2(name.contains("Imperial"),
             qPrintable(QString("Distance unit name is wrong: %1").arg(name)));
    scalar = settingsManager->distanceUnits()[QString("distanceScalar")].toDouble();
    QVERIFY2(qFuzzyCompare(scalar, 3.280839895),
             qPrintable(QString("Distance scalar in meters is wrong: %1").arg(scalar)));
}

void Test::waterfallGradient()
{
    QVector<QColor> colorList = {Qt::black, Qt::white};
    // Check name and validation
    auto testName = QString("Test");
    auto gradient = WaterfallGradient(testName, colorList);
    QVERIFY2(gradient.isOk(), qPrintable("Basic gradient is not ok!"));
    QVERIFY2(gradient.name().contains(testName),
             qPrintable(QString("Name does not match: %1 != %2.").arg(gradient.name(), testName)));

    // Check if colors and values are correct
    QColor color0 = gradient.getColor(0);
    QColor color05 = gradient.getColor(0.5);
    QColor color1 = gradient.getColor(1);

    QVERIFY2(color0 == QColor(Qt::black),
             qPrintable(QString("Color does not match: %1").arg(color0.name())));

    QVERIFY2(color05 == QColor("#7f7f7f"),
             qPrintable(QString("Color does not match: %1").arg(color05.name())));

    QVERIFY2(color1 == QColor(Qt::white),
             qPrintable(QString("Color does not match: %1").arg(color1.name())));

    float value0 = gradient.getValue(color0);
    float value05 = gradient.getValue(color05);
    float value1 = gradient.getValue(color1);

    QVERIFY2(qFuzzyCompare(value0, 0),
             qPrintable(QString("Value does not match: %1").arg(value0)));

    QVERIFY2(abs(value05 - 0.5) < 0.05,
             qPrintable(QString("Value does not match: %1").arg(value05)));

    QVERIFY2(qFuzzyCompare(value1, 1),
             qPrintable(QString("Value does not match: %1").arg(value1)));

}

QTEST_MAIN(Test)