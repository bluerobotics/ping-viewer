#pragma once

#include <QOpenGLFunctions>
#include <QQuickItem>
#include <QSharedPointer>

class QOpenGLShaderProgram;

class QQuickOpenGL : public QQuickItem, protected QOpenGLFunctions {
    Q_OBJECT
    Q_PROPERTY(int shift READ shift WRITE setShift NOTIFY shiftChanged)

public:
    QQuickOpenGL(QQuickItem* parent = nullptr);
    void paint();

    void handleWindowChanged(QQuickWindow* window);

    int shift() { return _shift; }
    void setShift(int shift)
    {
        if (_shift != shift) {
            _shift = shift;
            emit shiftChanged();
        }
    }

signals:
    void shiftChanged();

private:
    QSharedPointer<QOpenGLShaderProgram> _shaderProgram;
    int _shift;
    QQuickWindow* _window;
};
