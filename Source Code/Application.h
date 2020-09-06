

//------------------------------------------------------------------------------
#ifndef APPLICATION_H
#define APPLICATION_H
//------------------------------------------------------------------------------
#if defined(WIN32) | defined(WIN64)
#pragma warning(disable: 4100)
#endif
//------------------------------------------------------------------------------
#include "chai3d.h"
//------------------------------------------------------------------------------
#include "Interface.h"
//------------------------------------------------------------------------------
#include <QBasicTimer>
#include <QWheelEvent>
//------------------------------------------------------------------------------
#include <string>
//------------------------------------------------------------------------------








void _hapticThread (void *arg);

//------------------------------------------------------------------------------

class ApplicationWidget : public QGLWidget
{

	//--------------------------------------------------------------------------
	// ���캯������������:
	//--------------------------------------------------------------------------

public:

    ApplicationWidget (QWidget *parent);
    virtual ~ApplicationWidget ();


	//--------------------------------------------------------------------------
	// ���г�Ա����:
	//--------------------------------------------------------------------------

public:

    bool loadModel(std::string filename);  //����ģ��
    int start();  //��������
    int stop();         //ֹͣ����
    void waitForStop();      //�ȴ�ֹͣ
    void* hapticThread();        //������Ⱦ�߳�
    bool isRunning() { return m_running; }

    double getGraphicRate() { return (m_graphicRate.getFrequency()); }   // ��ȡ�Ӿ�Ƶ��
    double getHapticRate() { return  (m_hapticRate.getFrequency()); }      //��ȡ����Ƶ��


    //--------------------------------------------------------------------------
    //������Ա����:
    //--------------------------------------------------------------------------

protected:

    void initializeGL();                            //��ʼ��OpenGL
    void resizeGL(int a_width, int a_height);           //����OpenGL����
    void paintGL();                                           //����OpenGL����
    void wheelEvent(QWheelEvent *event);                           //�����¼�
    void mousePressEvent(QMouseEvent *event);                          //��갴���¼�
    void mouseMoveEvent(QMouseEvent *event);                               //����ƶ��¼�
    void mouseReleaseEvent(QMouseEvent *event);                         //����ͷ��¼�
    void timerEvent(QTimerEvent *event) { updateGL(); }              //ʱ���¼�





    //--------------------------------------------------------------------------
    // ������Ա����:
    //--------------------------------------------------------------------------

public:

    // һ�����
    double m_toolRadius;
    double m_maxStiffness;

    // ���Ʊ���
    Interface* m_parent;
    chai3d::cMutex m_worldLock;
    chai3d::cMutex m_modelLock;
    chai3d::cMutex m_runLock;
    chai3d::cThread m_thread;
    chai3d::cFrequencyCounter m_graphicRate;
    chai3d::cFrequencyCounter m_hapticRate;

    QBasicTimer *m_timer;
    bool m_running;
    int m_width;
    int m_height;
    int m_mouseX;
    int m_mouseY;
    bool m_mouseMoveCamera;

    // CHAI3D ����
    chai3d::cGenericHapticDevicePtr m_device;
    chai3d::cWorld* m_world;
    chai3d::cCamera* m_camera;
    chai3d::cDirectionalLight* m_light;
    chai3d::cToolCursor* m_tool;
    chai3d::cMultiMesh* m_mesh;
    chai3d::cMultiPoint* m_point;
    chai3d::cGenericObject* m_object;
};

//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
