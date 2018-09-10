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

// Register message enums to qml
#include "pingmessage/pingmessage.h"

void Test::initTestCase()
{
    FileManager::self();
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

QTEST_MAIN(Test)