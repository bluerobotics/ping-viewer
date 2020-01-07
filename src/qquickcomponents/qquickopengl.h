#pragma once

#include <QQuickItem>
#include <QSharedPointer>

#include <QOpenGLShaderProgram>

class QQuickOpenGL : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int shift READ shift WRITE setShift NOTIFY shiftChanged)

public:
    QQuickOpenGL(QQuickItem *parent = nullptr);
    void paint();

    void handleWindowChanged(QQuickWindow *window);

    int shift() { return _shift; }
    void setShift(int shift) {
        if(_shift != shift) {
            _shift = shift;
            emit shiftChanged();
        }
    }

    QSGNode* updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;

signals:
    void shiftChanged();

private:
    QSharedPointer<QOpenGLShaderProgram> _shaderProgram;
    int _shift;
    QQuickWindow* _window;
};
