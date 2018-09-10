#include <QtTest/QtTest>

class Test: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void fileManager();
    void ringVector();
    void settingsManager();
};