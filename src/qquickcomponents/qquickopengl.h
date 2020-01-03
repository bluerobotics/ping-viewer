#pragma once

#include <QOpenGLFunctions>
#include <QQuickItem>
#include <QSharedPointer>

#include <QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QSGSimpleRectNode>

class QQuickOpenGL : public QQuickItem/*, QSGMaterial, QSGMaterialShader*/
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

/*

public:
    const char *vertexShader() const {
        return
        R"(
            uniform highp mat4 matrix;
            attribute highp vec4 vertex;
            attribute highp vec2 qt_MultiTexCoord0;
            varying highp vec2 coord;
            void main() {
                coord = qt_MultiTexCoord0;
                gl_Position = matrix * vertex;
            }

        )";
    }

    const char *fragmentShader() const {
        return
        R"(
            uniform lowp float opacity;
            uniform vec2 coord;
            void main() {
                gl_FragColor = vec4(1, coord.x, 0, 1) * opacity;
            }
        )";
    }

    char const *const *attributeNames() const
    {
        static char const *const names[] = { "vertex", 0 };
        return names;
    }

    void initialize()
    {
        QSGMaterialShader::initialize();
        m_id_matrix = program()->uniformLocation("matrix");
        m_id_opacity = program()->uniformLocation("opacity");
    }

    void updateState(const RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial)
    {
        program()->enableAttributeArray(0);
        Q_ASSERT(program()->isLinked());
        if (state.isMatrixDirty())
            program()->setUniformValue(m_id_matrix, state.combinedMatrix());
        if (state.isOpacityDirty())
            program()->setUniformValue(m_id_opacity, state.opacity());
    }

    QSGMaterialType *type() const { static QSGMaterialType type; return &type; }
    QSGMaterialShader *createShader() const { return const_cast<QQuickOpenGL*>(this); }

private:
    int m_id_matrix;
    int m_id_opacity;
    */
};
