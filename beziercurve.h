#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
class BezierCurve
{
public:
    BezierCurve(QVector3D &P1,QVector3D &P2,QVector3D &P3,QVector3D &P4);
    QVector3D GetPoint(float t);
    void Draw();
    float GetClosest(QVector3D &pt, int *id);
    void SetCurrent(int id){
        current=id;
    }
    void SetCurrent(QVector3D pt){
        P[current]=pt;
    }

    QVector3D P[4];                         /*atributos da classe*/
    int current;
};

#endif // BEZIERCURVE_H
