
//------------------------------------------------------------------------------
#include "Application.h"
//------------------------------------------------------------------------------
#include <QFile>
#include <QString>
#include <QMessageBox>

//�����¼QT�������������
#pragma execution_character_set("utf-8")   

//------------------------------------------------------------------------------
using namespace std;
using namespace chai3d;
//------------------------------------------------------------------------------

ApplicationWidget::ApplicationWidget (QWidget *parent)
{
	//--------------------------------------------------------------------------
	// ��ʼ��
	//----------

	// ��ʼ������
    m_parent  = (Interface*)(void*)parent;
    m_running = false;
    m_timer = new QBasicTimer;
    m_mouseMoveCamera = false;
    m_toolRadius = 0.002;

	// ����Ƶ�ʼ�����
    m_graphicRate.reset();
    m_hapticRate.reset();


	//--------------------------------------------------------------------------
	// ���� - ����� - ��Դ
	//--------------------------------------------------------------------------

	// ��������.
    m_world = new cWorld();

	// ���ñ���ɫ
    m_world->m_backgroundColor.setBlack();

	// �������������������
    m_camera = new cCamera(m_world);
    m_world->addChild(m_camera);

	// ����������ϵԭ��
    m_camera->setSphericalReferences(cVector3d(0,0,0),    // ԭ��
                                     cVector3d(0,0,1),    // �춥����
                                     cVector3d(1,0,0));   // ��λ����

    m_camera->setSphericalDeg(4.0,    // ������ϵ�뾶
                              0,      // ������ϵ�춥�����
                              0);     // ������ϵ����

    // ���������Զ�ͽ���
    m_camera->setClippingPlanes (0.01, 20.0);

	// ������Դ
    m_light = new cDirectionalLight (m_world);

	// ��ӹ�Դ
    m_camera->addChild(m_light);

	// ʹ�ܹ�Դ
    m_light->setEnabled(true);

	// �����������
    m_light->setDir(-1.0,-1.0,-0.5);


	//////////+++++++
	// virtual drill mesh
	cMultiMesh* drill;




	//--------------------------------------------------------------------------
	//�����豸
	//--------------------------------------------------------------------------

	// ���������豸�ֱ�
    cHapticDeviceHandler* handler = new cHapticDeviceHandler ();

	// ��������Ĵ����豸
    handler->getDevice (m_device, 0);

	// ��������
    m_tool = new cToolCursor (m_world);
    m_world->addChild (m_tool);

	// ���Ӵ����豸�����⹤��
    m_tool->setHapticDevice(m_device);

	// ���幤�߰뾶
    m_tool->setRadius(m_toolRadius);
 
	// ӳ���������ռ䵽���⹤���ռ�.
    m_tool->setWorkspaceRadius(1.0);

	// ������������
    m_tool->start();

	// ������Ϣ
    cHapticDeviceInfo hapticDeviceInfo = m_device->getSpecifications();

	// ��ȡ����ϵ��
    double workspaceScaleFactor = m_tool->getWorkspaceScaleFactor();

	// �ն�����
    m_maxStiffness = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor;

	// ��ӿ�����
    m_mesh = new cMultiMesh();
    m_point = new cMultiPoint();
    m_world->addChild(m_mesh);
    m_world->addChild(m_point);


	/////////////////////////////////////////////////////////////////////////
	// ��������
	/////////////////////////////////////////////////////////////////////////


	// ����
	bool fileload;
	// ��Ⱦ
	drill = new cMultiMesh();

	// ����ģ���Լ���ͼ
	//fileload = drill->loadFromFile("../../../../1/drill.3ds");
	//fileload = drill->loadFromFile("../../../../bin/1/drill.3ds");
	fileload = drill->loadFromFile("../../../bin/1/drill.3ds"); //���õ��� vcxprojΪ��ǰĿ¼
	//fileload = drill->loadFromFile("./drill.3ds");         
	fileload = drill->loadFromFile("./Cut.stl");

	//���¶���ߴ�
	drill->scale(0.004);

	// ��ײ���
	drill->deleteCollisionDetector(true);

	// �������Ĳ�������
	cMaterial mat;
	mat.m_ambient.set(0.5f, 0.5f, 0.5f);
	mat.m_diffuse.set(0.8f, 0.8f, 0.8f);
	mat.m_specular.set(1.0f, 1.0f, 1.0f);
	drill->setMaterial(mat, true);
	drill->computeAllNormals();

	// ���������ĵ��빤��
	m_tool->m_image->addChild(drill);








	//--------------------------------------------------------------------------
	// С����
	//--------------------------------------------------------------------------

	// ��������
    cBackground* background = new cBackground();
    m_camera->m_backLayer->addChild(background);

	// ���ñ�������
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
      // �Ƴ�����
    m_worldLock.acquire();
    m_modelLock.acquire();
    m_world->removeChild(m_mesh);
    m_world->removeChild(m_point);
    m_modelLock.release();
    m_worldLock.release();

    // ��������
    m_mesh->clear();
    m_point->clear();

    // ��������͹���
    m_camera->setSphericalDeg(4.0,    // spherical coordinate radius
                              0,      // spherical coordinate azimuth angle
                              0);     // spherical coordinate polar angle
    m_tool->setLocalRot(m_camera->getLocalRot());

    // ����3Dģ��
    if (m_mesh->loadFromFile(filename) && m_mesh->getNumTriangles() > 0)
    {
        // ��ֹ culling 
        m_mesh->setUseCulling(false);

        // ��ȡ����ά��
        m_mesh->computeBoundaryBox(true);
        double size = cSub(m_mesh->getBoundaryMax(), m_mesh->getBoundaryMin()).length();

        // ��������
        m_mesh->scale(2.0 / size);

        // ����߽��
        m_mesh->computeBoundaryBox(true);

        // ������ײ����㷨
        m_mesh->createAABBCollisionDetector(m_toolRadius);

        // ����Ĭ�ϵĸն�����
        m_mesh->setStiffness(0.2 * m_maxStiffness, true);

        // ����Ħ������
        m_mesh->setFriction(0.1, 0.2, true);

        // ʹ����ʾ
        m_mesh->setUseDisplayList(true);

        // �����������еı�
        m_mesh->computeAllEdges(15.0);

        // �����߿�
        cColorf colorEdges;
        colorEdges.setBlack();
        m_mesh->setEdgeProperties(1, colorEdges);

        // ������ʾ�ķ�������
        cColorf colorNormals;
        colorNormals.setOrangeTomato();
        m_mesh->setNormalsProperties(0.01, colorNormals);

        // ������ͼ����
        m_mesh->setShowTriangles(true);
        m_mesh->setShowEdges(false);
        m_mesh->setShowNormals(false);
        m_mesh->setShowBoundaryBox(false);
        m_mesh->setShowFrame(false);

        // �������
        m_worldLock.acquire();
        m_modelLock.acquire();
        m_world->addChild(m_mesh);
        m_mesh->setLocalPos(-1.0 * m_mesh->getBoundaryCenter());
        m_object = m_mesh;
        m_modelLock.release();
        m_worldLock.release();

        // ���������豸
        m_tool->start();

        return (true);
    }


	// �������
    if (m_point->loadFromFile(filename))
    {
		// ��������ά��
        m_point->computeBoundaryBox(true);
        double size = cSub(m_point->getBoundaryMax(), m_point->getBoundaryMin()).length();

		// ��������
        m_point->scale(2.0 / size);

        // ����߽��
        m_point->computeBoundaryBox(true);

        // ������ײ����㷨
        m_point->createAABBCollisionDetector(m_toolRadius);

        // ����Ĭ�ϸն�t
        m_point->setStiffness(0.2 * m_maxStiffness, true);

        // ʹ����ʾ
        m_point->setUseDisplayList(true);

        // ���õ�ߴ�
        m_point->setPointSize(5.0);

        // �������
        m_worldLock.acquire();
        m_modelLock.acquire();
        m_world->addChild(m_point);
        m_point->setLocalPos(-1.0 * m_point->getBoundaryCenter());
        m_object = m_point;
        m_modelLock.release();
        m_worldLock.release();

        // ���������豸
        m_tool->start();

        return (true);
    }

    // ������Ϣ��
    QMessageBox::warning(this, "CG", QString("Failed to load model %1").arg(filename.c_str()), QMessageBox::Ok);
    return false;
}

//------------------------------------------------------------------------------

void* ApplicationWidget::hapticThread ()
{
    // ��ȡ�������� 
    m_runLock.acquire();

    // ����״̬
    m_running = true;

    while (m_running) 
    {
        // ����λ�úͷ���
        m_tool->updateFromDevice();

        // ����Ⱦ�����б���ģ�Ͳ���
        m_modelLock.acquire();

        // ����ȫ�ֲο�
        m_world->computeGlobalPositions(true);

        // ���㽻����
        m_tool->computeInteractionForces();

        // ����ģ��
        m_modelLock.release();

        // ���ͷ�����
        m_tool->applyToDevice();

        // ����Ƶ�ʼ�����
        m_hapticRate.signal(1);
    }

    // ��ֹ��
    m_device->setForceAndTorqueAndGripperForce (cVector3d(0.0, 0.0, 0.0),
                                                cVector3d(0.0, 0.0, 0.0),
                                                0.0);

    // ����״̬
    m_running = false;

    // �ͷ�������
    m_runLock.release();

    // �˳��߳�
    return (NULL);
}

//------------------------------------------------------------------------------

void ApplicationWidget::initializeGL ()
{
#ifdef GLEW_VERSION
    glewInit ();
#endif

    // ʹ�ܿ����
    QGLWidget::setFormat(QGLFormat(QGL::SampleBuffers));
}

//------------------------------------------------------------------------------

void ApplicationWidget::paintGL ()
{
    if (!m_running) return;

    m_worldLock.acquire();

    // ��Ⱦ����
    m_camera->renderView(m_width, m_height);

    // �ȴ����е�GL�����������
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
    // ����ͼ����Ⱦ
    m_timer->start(25, this);

    // ����ͼ���߳�
    m_thread.start (_hapticThread, CTHREAD_PRIORITY_HAPTICS, this);

    return(0);
}

//------------------------------------------------------------------------------

int ApplicationWidget::stop ()
{
    // ֹͣ�����߳�
    m_running = false;
    m_runLock.acquire();
    m_runLock.release();

    // ֹͣ����
    m_tool->stop();

    // ֹͣͼ��
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

        // ��������˶�
        int dx = x - m_mouseX;
        int dy = y - m_mouseY;
        m_mouseX = x;
        m_mouseY = y;

        // ����������Ƕ�
        double azimuthDeg = m_camera->getSphericalAzimuthDeg() + (0.5 * dy);
        double polarDeg = m_camera->getSphericalPolarDeg() + (-0.5 * dx);

        // �����½Ƕ�
        m_camera->setSphericalAzimuthDeg(azimuthDeg);
        m_camera->setSphericalPolarDeg(polarDeg);

        // ���ӽǶ�
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
