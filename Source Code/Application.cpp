
//------------------------------------------------------------------------------
#include "Application.h"
//------------------------------------------------------------------------------
#include <QFile>
#include <QString>
#include <QMessageBox>

//解决登录QT出现乱码的问题
#pragma execution_character_set("utf-8")   

//------------------------------------------------------------------------------
using namespace std;
using namespace chai3d;
//------------------------------------------------------------------------------

ApplicationWidget::ApplicationWidget (QWidget *parent)
{
	//--------------------------------------------------------------------------
	// 初始化
	//----------

	// 初始化变量
    m_parent  = (Interface*)(void*)parent;
    m_running = false;
    m_timer = new QBasicTimer;
    m_mouseMoveCamera = false;
    m_toolRadius = 0.002;

	// 重置频率计数器
    m_graphicRate.reset();
    m_hapticRate.reset();


	//--------------------------------------------------------------------------
	// 世界 - 照相机 - 光源
	//--------------------------------------------------------------------------

	// 创建世界.
    m_world = new cWorld();

	// 设置背景色
    m_world->m_backgroundColor.setBlack();

	// 创建相机并插入世界中
    m_camera = new cCamera(m_world);
    m_world->addChild(m_camera);

	// 定义球坐标系原点
    m_camera->setSphericalReferences(cVector3d(0,0,0),    // 原点
                                     cVector3d(0,0,1),    // 天顶方向
                                     cVector3d(1,0,0));   // 方位方向

    m_camera->setSphericalDeg(4.0,    // 求坐标系半径
                              0,      // 求坐标系天顶方向角
                              0);     // 求坐标系极角

    // 设置相机的远和近面
    m_camera->setClippingPlanes (0.01, 20.0);

	// 创建光源
    m_light = new cDirectionalLight (m_world);

	// 添加光源
    m_camera->addChild(m_light);

	// 使能光源
    m_light->setEnabled(true);

	// 定义光束方向
    m_light->setDir(-1.0,-1.0,-0.5);


	//////////+++++++
	// virtual drill mesh
	cMultiMesh* drill;




	//--------------------------------------------------------------------------
	//触觉设备
	//--------------------------------------------------------------------------

	// 创建触觉设备手柄
    cHapticDeviceHandler* handler = new cHapticDeviceHandler ();

	// 连接最近的触觉设备
    handler->getDevice (m_device, 0);

	// 创建工具
    m_tool = new cToolCursor (m_world);
    m_world->addChild (m_tool);

	// 连接触觉设备和虚拟工具
    m_tool->setHapticDevice(m_device);

	// 定义工具半径
    m_tool->setRadius(m_toolRadius);
 
	// 映射物理工作空间到虚拟工作空间.
    m_tool->setWorkspaceRadius(1.0);

	// 启动触觉工具
    m_tool->start();

	// 检索信息
    cHapticDeviceInfo hapticDeviceInfo = m_device->getSpecifications();

	// 读取比例系数
    double workspaceScaleFactor = m_tool->getWorkspaceScaleFactor();

	// 刚度属性
    m_maxStiffness = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor;

	// 添加空物体
    m_mesh = new cMultiMesh();
    m_point = new cMultiPoint();
    m_world->addChild(m_mesh);
    m_world->addChild(m_point);


	/////////////////////////////////////////////////////////////////////////
	// 手术器材
	/////////////////////////////////////////////////////////////////////////


	// 载入
	bool fileload;
	// 渲染
	drill = new cMultiMesh();

	// 载入模型以及贴图
	//fileload = drill->loadFromFile("../../../../1/drill.3ds");
	//fileload = drill->loadFromFile("../../../../bin/1/drill.3ds");
	fileload = drill->loadFromFile("../../../bin/1/drill.3ds"); //调用的是 vcxproj为当前目录
	//fileload = drill->loadFromFile("./drill.3ds");         
	fileload = drill->loadFromFile("./Cut.stl");

	//重新定义尺寸
	drill->scale(0.004);

	// 碰撞检测
	drill->deleteCollisionDetector(true);

	// 定义器材材料属性
	cMaterial mat;
	mat.m_ambient.set(0.5f, 0.5f, 0.5f);
	mat.m_diffuse.set(0.8f, 0.8f, 0.8f);
	mat.m_specular.set(1.0f, 1.0f, 1.0f);
	drill->setMaterial(mat, true);
	drill->computeAllNormals();

	// 将手术器材导入工具
	m_tool->m_image->addChild(drill);








	//--------------------------------------------------------------------------
	// 小窗口
	//--------------------------------------------------------------------------

	// 创建背景
    cBackground* background = new cBackground();
    m_camera->m_backLayer->addChild(background);

	// 设置背景属性
    background->setCornerColors(cColorf(1.00f, 1.00f, 1.00f),
                                cColorf(1.00f, 1.00f, 1.00f),
                                cColorf(0.85f, 0.85f, 0.85f),
                                cColorf(0.85f, 0.85f, 0.85f));
};
  
//------------------------------------------------------------------------------

ApplicationWidget::~ApplicationWidget ()
{
    stop();
    delete m_world;
}

//------------------------------------------------------------------------------

bool
ApplicationWidget::loadModel (string filename)
{
      // 移除物体
    m_worldLock.acquire();
    m_modelLock.acquire();
    m_world->removeChild(m_mesh);
    m_world->removeChild(m_point);
    m_modelLock.release();
    m_worldLock.release();

    // 重置物体
    m_mesh->clear();
    m_point->clear();

    // 重置相机和工具
    m_camera->setSphericalDeg(4.0,    // spherical coordinate radius
                              0,      // spherical coordinate azimuth angle
                              0);     // spherical coordinate polar angle
    m_tool->setLocalRot(m_camera->getLocalRot());

    // 载入3D模型
    if (m_mesh->loadFromFile(filename) && m_mesh->getNumTriangles() > 0)
    {
        // 禁止 culling 
        m_mesh->setUseCulling(false);

        // 获取物体维度
        m_mesh->computeBoundaryBox(true);
        double size = cSub(m_mesh->getBoundaryMax(), m_mesh->getBoundaryMin()).length();

        // 重置物体
        m_mesh->scale(2.0 / size);

        // 计算边界盒
        m_mesh->computeBoundaryBox(true);

        // 计算碰撞检测算法
        m_mesh->createAABBCollisionDetector(m_toolRadius);

        // 定义默认的刚度属性
        m_mesh->setStiffness(0.2 * m_maxStiffness, true);

        // 定义摩擦属性
        m_mesh->setFriction(0.1, 0.2, true);

        // 使能显示
        m_mesh->setUseDisplayList(true);

        // 计算物体所有的边
        m_mesh->computeAllEdges(15.0);

        // 设置线宽
        cColorf colorEdges;
        colorEdges.setBlack();
        m_mesh->setEdgeProperties(1, colorEdges);

        // 设置显示的法向属性
        cColorf colorNormals;
        colorNormals.setOrangeTomato();
        m_mesh->setNormalsProperties(0.01, colorNormals);

        // 设置视图属性
        m_mesh->setShowTriangles(true);
        m_mesh->setShowEdges(false);
        m_mesh->setShowNormals(false);
        m_mesh->setShowBoundaryBox(false);
        m_mesh->setShowFrame(false);

        // 添加物体
        m_worldLock.acquire();
        m_modelLock.acquire();
        m_world->addChild(m_mesh);
        m_mesh->setLocalPos(-1.0 * m_mesh->getBoundaryCenter());
        m_object = m_mesh;
        m_modelLock.release();
        m_worldLock.release();

        // 启动触觉设备
        m_tool->start();

        return (true);
    }


	// 载入点云
    if (m_point->loadFromFile(filename))
    {
		// 获得物体的维度
        m_point->computeBoundaryBox(true);
        double size = cSub(m_point->getBoundaryMax(), m_point->getBoundaryMin()).length();

		// 重置物体
        m_point->scale(2.0 / size);

        // 计算边界盒
        m_point->computeBoundaryBox(true);

        // 计算碰撞检测算法
        m_point->createAABBCollisionDetector(m_toolRadius);

        // 定义默认刚度t
        m_point->setStiffness(0.2 * m_maxStiffness, true);

        // 使能显示
        m_point->setUseDisplayList(true);

        // 设置点尺寸
        m_point->setPointSize(5.0);

        // 添加物体
        m_worldLock.acquire();
        m_modelLock.acquire();
        m_world->addChild(m_point);
        m_point->setLocalPos(-1.0 * m_point->getBoundaryCenter());
        m_object = m_point;
        m_modelLock.release();
        m_worldLock.release();

        // 启动触觉设备
        m_tool->start();

        return (true);
    }

    // 错误消息框
    QMessageBox::warning(this, "CG", QString("Failed to load model %1").arg(filename.c_str()), QMessageBox::Ok);
    return false;
}

//------------------------------------------------------------------------------

void* ApplicationWidget::hapticThread ()
{
    // 获取运行锁定 
    m_runLock.acquire();

    // 更新状态
    m_running = true;

    while (m_running) 
    {
        // 更新位置和方向
        m_tool->updateFromDevice();

        // 在渲染过程中保持模型不变
        m_modelLock.acquire();

        // 计算全局参考
        m_world->computeGlobalPositions(true);

        // 计算交互力
        m_tool->computeInteractionForces();

        // 更改模型
        m_modelLock.release();

        // 发送反馈力
        m_tool->applyToDevice();

        // 更新频率计算器
        m_hapticRate.signal(1);
    }

    // 禁止力
    m_device->setForceAndTorqueAndGripperForce (cVector3d(0.0, 0.0, 0.0),
                                                cVector3d(0.0, 0.0, 0.0),
                                                0.0);

    // 更新状态
    m_running = false;

    // 释放运行锁
    m_runLock.release();

    // 退出线程
    return (NULL);
}

//------------------------------------------------------------------------------

void ApplicationWidget::initializeGL ()
{
#ifdef GLEW_VERSION
    glewInit ();
#endif

    // 使能抗混叠
    QGLWidget::setFormat(QGLFormat(QGL::SampleBuffers));
}

//------------------------------------------------------------------------------

void ApplicationWidget::paintGL ()
{
    if (!m_running) return;

    m_worldLock.acquire();

    // 渲染世界
    m_camera->renderView(m_width, m_height);

    // 等待所有的GL命令运行完成
    glFinish();

    m_graphicRate.signal(1);

    m_worldLock.release();
}

//------------------------------------------------------------------------------

void ApplicationWidget::resizeGL (int a_width,  int a_height)
{
    m_worldLock.acquire ();

    m_width = a_width;
    m_height = a_height;

    m_worldLock.release ();
}

//------------------------------------------------------------------------------

int ApplicationWidget::start ()
{
    // 启动图形渲染
    m_timer->start(25, this);

    // 启动图形线程
    m_thread.start (_hapticThread, CTHREAD_PRIORITY_HAPTICS, this);

    return(0);
}

//------------------------------------------------------------------------------

int ApplicationWidget::stop ()
{
    // 停止仿真线程
    m_running = false;
    m_runLock.acquire();
    m_runLock.release();

    // 停止触觉
    m_tool->stop();

    // 停止图形
    m_timer->stop();

    return 0;
}

//------------------------------------------------------------------------------

void ApplicationWidget::wheelEvent (QWheelEvent *event)
{
    double radius = m_camera->getSphericalRadius() + (double)(event->delta())*5e-4;
    m_camera->setSphericalRadius(radius);
}

//------------------------------------------------------------------------------

void ApplicationWidget::mousePressEvent(QMouseEvent *event)
{
    m_mouseX = event->pos().x();
    m_mouseY = event->pos().y();
    m_mouseMoveCamera = true;
}

//------------------------------------------------------------------------------

void ApplicationWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mouseMoveCamera)
    {
        int x = event->pos().x();
        int y = event->pos().y();

        // 计算鼠标运动
        int dx = x - m_mouseX;
        int dy = y - m_mouseY;
        m_mouseX = x;
        m_mouseY = y;

        // 计算新相机角度
        double azimuthDeg = m_camera->getSphericalAzimuthDeg() + (0.5 * dy);
        double polarDeg = m_camera->getSphericalPolarDeg() + (-0.5 * dx);

        // 赋予新角度
        m_camera->setSphericalAzimuthDeg(azimuthDeg);
        m_camera->setSphericalPolarDeg(polarDeg);

        // 连接角度
        m_tool->setLocalRot(m_camera->getLocalRot());
    }
}

//------------------------------------------------------------------------------

void ApplicationWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_mouseMoveCamera = false;
}

//------------------------------------------------------------------------------

void _hapticThread (void *arg)
{
    ((ApplicationWidget*)arg)->hapticThread();
}

//------------------------------------------------------------------------------
