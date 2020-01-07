#include "qquickopengl.h"

#include <QQuickWindow>

#include <math.h>

#include <QDebug>
#include <QTimer>

#include <QSGSimpleRectNode>

class Shader : public QSGMaterialShader
{
public:
    Shader()
    {
        setShaderSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/opengl/qquickopengl/fragment.glsl"));
        setShaderSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/opengl/qquickopengl/vertex.glsl"));
    }

    /**
     * @brief Return attribute names
     *  The last element should be zero, from documentation
     *
     * @return char const* const*
     */
    char const *const *attributeNames() const
    {
        static char const *const names[] = {"qt_Vertex", "qt_MultiTexCoord0", 0 };
        return names;
    }

    void initialize()
    {
        QSGMaterialShader::initialize();
        m_id_matrix = program()->uniformLocation("qt_Matrix");
        m_id_opacity = program()->uniformLocation("qt_Opacity");
        /*
        m_id_shift = program()->uniformLocation("shift");

        auto timer = new QTimer();
        QObject::connect(timer, &QTimer::timeout, [&] { shift += 0.01; } );
        timer->start(10);
        */
    }

    void updateState(const RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial)
    {
        qDebug() << newMaterial << oldMaterial;
        Q_ASSERT(program()->isLinked());
        if (state.isOpacityDirty()) {
            program()->setUniformValue(m_id_opacity, state.opacity());
        }
        if (state.isMatrixDirty()) {
            program()->setUniformValue(m_id_matrix, state.combinedMatrix());
        }

        qDebug() << "MATRIX combined:" << state.combinedMatrix();
        qDebug() << "MATRIX modelview:" << state.modelViewMatrix();
        qDebug() << "MATRIX projection:" << state.projectionMatrix();
        //qt_ModelViewProjectionMatrix
        qDebug() << "RECT1:" << state.deviceRect();
        qDebug() << "RECT:" << state.viewportRect();
        qDebug() << "PIXEL:" << state.devicePixelRatio();

        /*
        qDebug() << "update";
        program()->setUniformValue(m_id_shift, shift);
        */
    }

private:
    int m_id_matrix;
    int m_id_opacity;
    int m_id_shift;
    //float shift = 0;
};

class Material : public QSGMaterial
{
public:
    QSGMaterialType *type() const { static QSGMaterialType type; return &type; }
    QSGMaterialShader *createShader() const { return new Shader; }
};

QQuickOpenGL::QQuickOpenGL(QQuickItem *parent)
    : QQuickItem(parent)
    , _shaderProgram(nullptr)
    , _shift(0)
    , _window(nullptr)
{
    // We are going to fill our own content with OpenGL
    QQuickItem::setFlag(QQuickItem::ItemHasContents);

    connect(this, &QQuickItem::windowChanged, this, &QQuickOpenGL::handleWindowChanged);
    connect(this, &QQuickOpenGL::shiftChanged, this, [this]{
        if (_window) {
            _window->update();
        }
        //qDebug() << boundingRect();
    });

    auto timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &QQuickOpenGL::shiftChanged);
    timer->start(1000/60);

    connect(this, &QQuickItem::childrenRectChanged, this, [this](auto rect) {
        /*
        if(!node) {
            return;
        }
        QSGGeometry::updateTexturedRectGeometry(node->geometry(), boundingRect(), QRectF(0, 0, 1, 1));
        node->markDirty(QSGNode::DirtyGeometry);
        */
    });
}

void QQuickOpenGL::handleWindowChanged(QQuickWindow* window)
{
    if (!window) {
        return;
    }

    _window = window;
    window->setClearBeforeRendering(false);
}


QSGNode *QQuickOpenGL::updatePaintNode(QSGNode *node, UpdatePaintNodeData *)
{
    QSGSimpleRectNode *simpleRectNode = static_cast<QSGSimpleRectNode *>(node);
    if (!simpleRectNode) {
        simpleRectNode = new QSGSimpleRectNode();
        simpleRectNode->setMaterial(new Material());
        simpleRectNode->material()->setFlag(QSGMaterial::Blending);
        simpleRectNode->setFlag(QSGNode::OwnsMaterial, true);
        QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);
        QSGGeometry::updateTexturedRectGeometry(geometry, boundingRect(), QRectF(0, 0, 1, 1));
        simpleRectNode->setGeometry(geometry);
        simpleRectNode->setFlag(QSGNode::OwnsGeometry, true);
    } else {
        QSGGeometry::updateTexturedRectGeometry(simpleRectNode->geometry(), boundingRect(), QRectF(0, 0, 1, 1));
        simpleRectNode->markDirty(QSGNode::DirtyGeometry);
    }
    simpleRectNode->setRect(boundingRect());
    return simpleRectNode;
}
