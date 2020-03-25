#include <QtTest/QtTest>

/**
 * @brief Test class to test basic functionalities
 *
 */
class Test : public QObject {
    Q_OBJECT
private slots:
    /**
     * @brief Initialize what is necessary
     *
     */
    void initTestCase();

    /**
     * @brief Test file manager
     *
     */
    void fileManager();

    /**
     * @brief Test logger singleton
     *
     */
    void logger();

    /**
     * @brief Test ring vector
     *
     */
    void ringVector();

    /**
     * @brief Test settings manager
     *
     */
    void settingsManager();

    /**
     * @brief Test waterfall gradient
     *
     */
    void waterfallGradient();
};
