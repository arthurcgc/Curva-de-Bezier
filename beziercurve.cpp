#include "beziercurve.h"
#include <math.h>
BezierCurve::BezierCurve(QVector3D &P1,QVector3D &P2,QVector3D &P3,QVector3D &P4)/* construtor*/
{
    this->P[0]=P1;
    this->P[1]=P2;
    this->P[2]=P3;
    this->P[3]=P4;
    current=-1; //indice do ponto sendo movido
}

QVector3D BezierCurve::GetPoint(float t){           /*função que implementa a fórmula de bezier para calcular todos*/
                                                    /*os pontos de acordo com o parametro t e os retorna como vetor 3d*/
    QVector3D pt;
    pt.setX(pow((1-t),3)*P[0].x()+3.0*pow((1.0-t),2)*t*P[1].x()+3.0*(1-t)*pow(t,2)*P[2].x()+pow(t,3)*P[3].x());
    pt.setY(pow((1-t),3)*P[0].y()+3.0*pow((1.0-t),2)*t*P[1].y()+3.0*(1-t)*pow(t,2)*P[2].y()+pow(t,3)*P[3].y());
    pt.setZ(0.f);
    return pt;
}



float BezierCurve::GetClosest(QVector3D &pt, int *id){  /* função que calcula a menor distância do clique
                                                        do mouse a um dos pontos de controle da bezier e retorna o seu indice e a distancia*/
    *id=0;
    float clst=P[0].distanceToPoint(pt);
    for (int i=1;i<4;++i){
        float dist=P[i].distanceToPoint(pt);
        if (dist<clst){
            clst=dist;
            *id=i;
        }
    }
    return clst;
}


void BezierCurve::Draw(){                   /* função que desenha a bezier*/
    glLineWidth(3.0);
    glBegin(GL_LINE_STRIP);//comando de inicio do desenho da poligonal
    for(float t=0;t<=1;t+=0.001){
        QVector3D P=GetPoint(t);
        glVertex3f(P.x(),P.y(),P.z());//funcao do Opengl que acrescenta um ponto na poligonal
    }
    glEnd();
    glLineWidth(1.0);
}
