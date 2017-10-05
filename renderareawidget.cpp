#include "renderareawidget.h"

#include <QMouseEvent>

const char* vertexShaderSource = R"(
    #version 330 core

    layout( location = 0 ) in vec3 vertexPos;
    uniform mat4 transformMatrix;

    void main()
    {
        gl_Position = transformMatrix * vec4( vertexPos, 1 );
    }
)";


const char* fragmentShaderSource = R"(
    #version 330 core

    uniform vec3 color;
    out vec3 finalColor;

    void main()
    {
        finalColor = color;
    }
)";


RenderAreaWidget::RenderAreaWidget(QWidget* parent) /*construtor do objeto que cria a area de desenho */
    : QOpenGLWidget(parent),
      program(nullptr),
      bezier(nullptr)
{
}


RenderAreaWidget::~RenderAreaWidget()       /*destrutor da area de desenho*/
{
    makeCurrent();
    pointsBuffer.destroy();
    doneCurrent();
    delete program;
}


void RenderAreaWidget::initializeGL()
{
    initializeOpenGLFunctions();

    makeCurrent();

    glViewport(0,0,width(),height());

    //Layout de ponto e linha:
    glEnable(GL_POINT_SMOOTH);
    glEnable( GL_LINE_SMOOTH );
    glLineWidth(1.0f);
    glPointSize(10.0f);

    program = new QOpenGLShaderProgram();
    program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    program->link();

    if (!program->isLinked())
    {
        //TODO: Exibir erro de link e fechar o programa
    }

    program->bind();

    pointsBuffer.create();

    proj.ortho(-10.f,10.f,-10.f,10.f,-1.f,1.0f);   /*projetou num espaço 20x20*/

    program->setUniformValue("transformMatrix", proj*view);
}


void RenderAreaWidget::paintGL()        /* metodo que desenha*/
{
    program->bind();

    glClearColor(0, 0, 0, 1);           /*determina as cores do plano de fundo RGB + opacidade*/
    glClear(GL_COLOR_BUFFER_BIT);

    if(!points.empty() && pointsBuffer.isCreated())
    {
        pointsBuffer.bind();
        pointsBuffer.allocate( &points[0], (int)points.size()*sizeof(QVector3D) );
        program->enableAttributeArray(0);
        program->setAttributeBuffer(0,GL_FLOAT,0,3,sizeof(QVector3D));

        //Desenha o poligono
        program->setUniformValue("color", QVector3D(0,1,0)); //verde
        glDrawArrays(GL_LINE_STRIP, 0, (int)points.size());

        //Desenha os pontos
        program->setUniformValue("color", QVector3D(1,0.5,0)); //laranja
        glDrawArrays(GL_POINTS, 0, (int)points.size());

        if(bezier!=NULL){               /*se criou a bezier, chama o metodo de desenho*/
            bezier->Draw();
        }
    }
}


void RenderAreaWidget::resizeGL(int width, int height)                      /*resize da janela*/
{
    glViewport(0, 0, width, height);
}


void RenderAreaWidget::mousePressEvent(QMouseEvent *event)
{
    if(points.size()<4){                    /*se a poligonal tiver menos de quatro pontos, nao faz nada*/
        return;
    }

    QVector3D point( event->x(), height()-event->y(), 0 );
    point = point.unproject( view, proj, QRect(0,0,width(),height()));
    point.setZ(0.f);
    int id;
    float dist=bezier->GetClosest(point,&id);   /*usada para manipulação dos pontos após a criação da bezier*/
    if(dist>1.5) return;                        /*tolerancia de proximidade aceita*/

    bezier->SetCurrent(id);                     /*seta o ponto de controle que será movido*/
}


void RenderAreaWidget::mouseMoveEvent(QMouseEvent *event)
{
 if (bezier!=NULL && bezier->current!=-1) /*checa se criou a bezier e se iniciou o movimento de um ponto*/
 {
     QVector3D point( event->x(), height()-event->y(), 0 );             /*inicializa as coordenadas do mouse event na variavel point*/
     point = point.unproject( view, proj, QRect(0,0,width(),height())); /*transforma o ponto para o espaço do objeto, no caso 20x20*/
     point.setZ(0.f);
     bezier->SetCurrent(point);                                         /*seta a nova posição do ponto id(o ponto de controle sendo movido)*/
     points[bezier->current]=point;                                     /*move o ponto correspondente na linha poligonal*/
     update();
 }
}


void RenderAreaWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (bezier!=NULL){
        bezier->SetCurrent(-1);         /*finaliza o possivel movimento do ponto current da bezier para nao ser movido apos o release*/
        return;
    }
    /*adicionando pontos à poligonal que dará origem à bezier*/
    QVector3D point( event->x(), height()-event->y(), 0 );
    point = point.unproject( view, proj, QRect(0,0,width(),height()));
    point.setZ(0.f);

    points.push_back( point );          /*acrescenta o novo ponto no final do vetor*/
    if(points.size()==4){
        bezier=new BezierCurve(points[0],points[1],points[2],points[3]); /*cria a bezier*/
    }
    update();
}

