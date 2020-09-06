

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
	// 构造函数和析构函数:
	//--------------------------------------------------------------------------

public:

    ApplicationWidget (QWidget *parent);
    virtual ~ApplicationWidget ();


	//--------------------------------------------------------------------------
	// 共有成员函数:
	//--------------------------------------------------------------------------

public:

    bool loadModel(std::string filename);  //加载模型
    int start();  //启动运行
    int stop();         //停止运行
    void waitForStop();      //等待停止
    void* hapticThread();        //触觉渲染线程
    bool isRunning() { return m_running; }

    double getGraphicRate() { return (m_graphicRate.getFrequency()); }   // 获取视觉频率
    double getHapticRate() { return  (m_hapticRate.getFrequency()); }      //获取触觉频率


    //--------------------------------------------------------------------------
    //保护成员函数:
    //--------------------------------------------------------------------------

protected:

    void initializeGL();                            //初始化OpenGL
    void resizeGL(int a_width, int a_height);           //重置OpenGL窗口
    void paintGL();                                           //绘制OpenGL窗口
    void wheelEvent(QWheelEvent *event);                           //滚轮事件
    void mousePressEvent(QMouseEvent *event);                          //鼠标按下事件
    void mouseMoveEvent(QMouseEvent *event);                               //鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *event);                         //鼠标释放事件
    void timerEvent(QTimerEvent *event) { updateGL(); }              //时间事件





    //--------------------------------------------------------------------------
    // 公共成员变量:
    //--------------------------------------------------------------------------

public:

    // 一般变量
    double m_toolRadius;
    double m_maxStiffness;

    // 控制变量
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

    // CHAI3D 变量
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
