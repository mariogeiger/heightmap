
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
    
    cout << "Start application pid(" << a.applicationPid() << ")" << endl;
    QTime time;
    time.start();
    int w = 1024;
    int h = 1024;
    int octave = 9;
    float persistence = 0.5;

    cout << "w(" << w << ") h(" << h << ") octave(" << octave << ") persistence(" << persistence << ")" << endl;

    QImage map = noise(QSize(w, h), octave, persistence);

    cout << "Map created in " << time.elapsed() << "ms." << endl;

    map.save("map.jpg");

    QByteArray data((const char*)map.constBits(), map.byteCount());

    cout << "Map saved (" << QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex().constData() << ")" << endl;

    return 0;
}

QImage noise(QSize size, int octave, float persistence)
{
    QGLPixelBuffer pb(size);
    if (!pb.makeCurrent()) {
        qWarning() << "makeCurrent error";
        return QImage();
    }

    QGLShaderProgram p;
    p.addShaderFromSourceFile(QGLShader::Vertex, ":/noise.vert");
    p.addShaderFromSourceFile(QGLShader::Fragment, ":/noise.frag");
    p.bindAttributeLocation("vertex", 0);
    if (!p.link()) {
        qWarning() << "link error";
    return QImage();
}

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

    for (int oc = 0; oc < octave; ++oc) {
        float step = 1.0 / float(divide);

        initRandomPerm(perm, 256);
        p.setUniformValueArray("perm", perm, 256);
        p.setUniformValue("amplitude", amplitude);
        p.setUniformValue("step", step);


        cout << "octave #" << oc+1 << " : amplitude(" << amplitude << ") step(" << step << ")" << endl;

        glBegin(GL_QUADS);
        for (int y = 0; y < divide; ++y) {
            for (int x = 0; x < divide; ++x) {
                glVertex2f(x * step * 2.0 - 1.0, y * step * 2.0 - 1.0);
                glVertex2f((x+1) * step * 2.0 - 1.0, y * step * 2.0 - 1.0);
                glVertex2f((x+1) * step * 2.0 - 1.0, (y+1) * step * 2.0 - 1.0);
                glVertex2f(x * step * 2.0 - 1.0, (y+1) * step * 2.0 - 1.0);
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
