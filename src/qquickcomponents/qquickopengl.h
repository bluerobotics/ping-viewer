#pragma once

#include <QQuickItem>
#include <QSharedPointer>

#include <QOpenGLShaderProgram>

#include <QSGSimpleRectNode>

#include <QSGSimpleMaterialShader>

class Shader : public QSGMaterialShader
{
public:
    Shader()
    {
        loadDefaultShader();
    }

    /**
     * @brief Return attribute names
     *  The last element should be zero, from documentation
     *
     * @return char const* const*
     */
    char const *const *attributeNames() const;

    void loadDefaultShader();

    void initialize();

    void updateState(const RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial);

    virtual void update(const RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial)
    {
        Q_UNUSED(state)
        Q_UNUSED(newMaterial)
        Q_UNUSED(oldMaterial)
    }

private:
    int _matrixId;
    int _opacityId;
};

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
