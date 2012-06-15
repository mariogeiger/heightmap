
#include <QtCore/QCoreApplication>
#include <QtCore/QTime>
#include <QtCore/QDebug>
#include <QtCore/QCryptographicHash>
#include <QtGui/QApplication>
#include <QtGui/QImage>
#include <QtOpenGL/QGLFormat>
#include <QtOpenGL/QGLContext>
#include <QtOpenGL/QGLShaderProgram>
#include <QtOpenGL/QGLFramebufferObject>
#include <QtOpenGL/QGLPixelBuffer>
#include <QtOpenGL/QGLBuffer>
#include <cmath>
#include <iostream>

using namespace std;

QImage noise(QSize size, int octave, float persistence);
void initRandomPerm(GLint *perm, GLint count);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qsrand(a.applicationPid());
    
    qDebug() << "Start application pid(" << a.applicationPid() << ")";
    QTime time;
    time.start();
    int w = 1024;
    int h = 1024;
    int octave = 5;
    float persistence = 0.5;

    qDebug() << "w(" << w << ") h(" << h << ") octave(" << octave << ") persistence(" << persistence << ")";

    QImage map = noise(QSize(w, h), octave, persistence);

    qDebug() << "Map created in " << time.elapsed() << "ms.";

    map.save("map.jpg");

    QByteArray data((const char*)map.constBits(), map.byteCount());
    qDebug() << QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex();

    qDebug() << "Map saved";

    return 0;
}

QImage noise(QSize size, int octave, float persistence)
{
    QGLPixelBuffer pb(size);
    if (!pb.makeCurrent())
        qDebug() << "makeCurrent error";

    GLint max;
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &max);
    qDebug() << "GL_MAX_VERTEX_UNIFORM_COMPONENTS" << max;

    QGLShaderProgram p;
    p.addShaderFromSourceFile(QGLShader::Vertex, ":/noise.vert");
    p.addShaderFromSourceFile(QGLShader::Fragment, ":/noise.frag");
    p.bindAttributeLocation("vertex", 0);
    qDebug() << "link" << p.link();

    int permLocation = p.uniformLocation("perm");
    qDebug() << "perm location : " << permLocation;

    p.bind();
    p.enableAttributeArray(0);

    glViewport(0, 0, size.width(), size.height());

    //test de profondeur désactivé
    glDisable(GL_DEPTH_TEST);

    //mélange des couches
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    GLint perm[256];
    int divide = 1;
    float amplitude = (1.0 - persistence) / (1.0 - std::pow(persistence, octave));

    for (int o = 0; o < octave; ++o) {
        float step = 1.0 / float(divide);

        initRandomPerm(perm, 256);
        p.setUniformValueArray(permLocation, perm, 256);
        p.setUniformValue("amplitude", amplitude);
        p.setUniformValue("step", step);


        qDebug() << "octave #" << o+1 << " : amplitude(" << amplitude << ") step(" << step << ")";

        glBegin(GL_QUADS);
        for (int yy = 0; yy < divide; ++yy) {
            for (int xx = 0; xx < divide; ++xx) {
                glVertex2f(xx * step * 2.0 - 1.0, yy * step * 2.0 - 1.0);
                glVertex2f((xx+1) * step * 2.0 - 1.0, yy * step * 2.0 - 1.0);
                glVertex2f((xx+1) * step * 2.0 - 1.0, (yy+1) * step * 2.0 - 1.0);
                glVertex2f(xx * step * 2.0 - 1.0, (yy+1) * step * 2.0 - 1.0);
            }
        }
        glEnd();

        divide *= 2;
        amplitude *= persistence;
    }

    p.disableAttributeArray(0);
    p.release();

    return pb.toImage();
}

void initRandomPerm(GLint *perm, GLint count)
{
    QList<GLint> bag;
    for (GLint i = 0; i < count; ++i) {
        bag << i;
    }
    for (GLint i = 0; i < count; ++i) {
        perm[i] = bag.takeAt(qrand() % bag.size());
    }
}
