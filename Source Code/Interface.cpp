//==============================================================================
/*
    \author    Sebastien Grange
*/
//==============================================================================

//------------------------------------------------------------------------------
#include "Interface.h"
#include "Application.h"
#include "ui_Interface.h"
#include <QMessageBox>
#include <QMenu>

//�����¼QT�������������
#pragma execution_character_set("utf-8")   


//------------------------------------------------------------------------------
using std::string;
//------------------------------------------------------------------------------

Interface::Interface (QWidget        *parent,
                      Qt::WindowFlags flags) : QMainWindow (parent, flags)
{
    // setup ui
    ui.setupUi(this);


    // setup display keyboard shortcuts
    EscKey   = new QShortcut (Qt::Key_Escape, this, SLOT(ExitFullScreen()));
    FKey     = new QShortcut (Qt::Key_F,      this, SLOT(ToggleFullScreen()));
    SKey     = new QShortcut (Qt::Key_S,      this, SLOT(ToggleSettings()));
    QKey     = new QShortcut (Qt::Key_Q,      this, SLOT(close()));

    // create CHAI3D application widget
    Application = new ApplicationWidget (this);
    if (Application) 
    {
        Application->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
        centralWidget()->layout()->addWidget (Application);
    }
    else 
    {
        QMessageBox::information (this, "Application", "Cannot start application.", QMessageBox::Ok);
        close();
    }

    // configure timers
    StatusTimer = new QTimer (this);
    connect(StatusTimer, SIGNAL(timeout()), this, SLOT(UpdateStatus()));
    StatusTimer->start (1000);

    // initialize status bar
    GraphicRate.setFrameStyle(QFrame::Panel | QFrame::Sunken);
    ui.statusBar->addPermanentWidget (&GraphicRate);
    HapticRate.setFrameStyle(QFrame::Panel | QFrame::Sunken);
    ui.statusBar->addPermanentWidget (&HapticRate);





    // set folder list
    QStringList filters;
    filters << "*.3ds" << "*.obj" << "*.stl";
    dirModel = new QFileSystemModel(this);
    dirModel->setFilter(QDir::NoDot | QDir::AllDirs | QDir::Files);

    dirModel->setRootPath("/");
    dirModel->setNameFilters(filters);
    dirModel->setNameFilterDisables(false);
    ui.folderList->setModel(dirModel);
    ui.folderList->setRootIndex(dirModel->index(QCoreApplication::applicationDirPath()));

    // show settings by default
    ShowSettings(true);


	//////++++++++++++++++++++++
	///++++++++++++++
	this->setMouseTracking(true);//����Ҫ������ƶ���ʱ�򣬾�Ҫ��ȡ������㣬��Ҫ�������׷��
	ui.centralWidget->setMouseTracking(true);


	QMenuBar* pMenuBar = ui.menuBar;   // �˵���
	QMenu* pMenuA = new QMenu("��ͼ��&Q��");    //  �½�һ���˵�A //��q��ΪAlt��ʽ�Ŀ�ݼ�
	QMenu* pMenuB = new QMenu("HELP��&H��");    // W��ΪAlt��ʽ�Ŀ�ݼ�
	//QMenu* pMenuC = new QMenu("ʹ��˵����&C��");    //  �½�һ���˵�C //C��ΪAlt��ʽ�Ŀ�ݼ�
	pMenuBar->addMenu(pMenuA);       //�����˵�A��ӵ��˵����У� �˵�A�����ڲ˵����ĵ�һ��1���˵�λ��
	QAction* pActionA = new QAction("����˵��");          // �½�һ��Action��Ȼ���Ӧ��2���˵� ���뵽�˵�A��
	QAction* pActionB = new QAction("����Q&&A");         //QIcon(QPixmap(":/img/1.png")),
	QAction* pActionD = new QAction("Contact author");
	//pMenuB = menuBar()->addMenu(QString::fromLocal8Bit("HELP��&H��"));//menuBar()��mainwindow�У�Ҳ����widget����̳�QMainWindow��
	//QAction *pNewFile = new QAction(QIcon(tr(":/img/1.png")), QString::fromLocal8Bit("Contact author"), this);//
	//pMenuB->addAction(pNewFile);//��ӵ����ļ�������
	pMenuBar->addMenu(pMenuB);       //�����˵�B��ӵ��˵�����
	pMenuA->addAction(pActionA);  //�˵������1��ѡ��
	pMenuB->addAction(pActionB);     //����1���˵�B����ӵ�MenuBar��
	pMenuB->addAction(pActionD);
	pActionA->setStatusTip(tr("�ɳ�"));              // ��widget �Ĵ��ڵײ���ʾ2���˵���ϸ������
	pActionB->setStatusTip(tr("�Ӳ˵�����ʾ��Ϣ"));
	QObject::connect(pActionA, SIGNAL(triggered(bool)), this, SLOT(onMsg()));  //��Action���źŲ۹�������
	QObject::connect(pActionB, SIGNAL(triggered(bool)), this, SLOT(onQA()));  //��Action���źŲ۹�������
    QObject::connect(pActionD, SIGNAL(triggered(bool)), this, SLOT(close()));  //��Action���źŲ۹�������
	


	//Action���ź� ��ͨ��������ѯ��֪�����������ʱ�򣬻ᷢ��  triggered(bool) �ź� ��Ȼ���Լ������ۣ�����ÿ�ܵĲۺ�����Ӧ����
	QToolBar* pToolBar = ui.toolBar;    //���toolbar��������

	QAction* pActionC = new QAction(QIcon(QPixmap(":/img/1.png")), "����ʹ��");

	pToolBar->addAction(pActionC);

	QObject::connect(pActionC, SIGNAL(triggered(bool)), this, SLOT(onMsg()));  //���toolbar�ĵ������


	QStatusBar* pStatusBar = ui.statusBar;
	pStatusBar->showMessage("CG2018's software");                    //��ʼ��ʱ��ʾ״̬����

	/////////////////hubo


}

//------------------------------------------------------------------------------

int Interface::Start()
{
    // start haptic thread
    if (Application->start () < 0)
    {
        QMessageBox::warning (this, "CG", "No device found.", QMessageBox::Ok);
        return (-1);
    }

    return (0);
}

//------------------------------------------------------------------------------

void Interface::Stop()
{
    Application->stop();
}

//------------------------------------------------------------------------------

Interface::~Interface()
{
    Stop();
    delete Application;
}

//------------------------------------------------------------------------------

void Interface::EnterFullScreen()
{
    showFullScreen ();
}

//------------------------------------------------------------------------------

void Interface::ExitFullScreen()
{
    showNormal ();
}

//------------------------------------------------------------------------------

void Interface::ToggleFullScreen()
{
    if (isFullScreen())
    {
        ExitFullScreen();
    }
    else
    {
        EnterFullScreen();
    }
}

//------------------------------------------------------------------------------

void Interface::SetFullScreen(bool fs)
{
    if( fs && !isFullScreen())
    {
        EnterFullScreen();
    }
    else if (!fs &&  isFullScreen())
    {
        ExitFullScreen();
    }
}

//------------------------------------------------------------------------------

void Interface::ToggleSettings()
{
      bool show = !ui.Settings->isVisible();
      ui.Settings->setVisible (show);
}

//------------------------------------------------------------------------------
void Interface::ShowSettings(bool show)
{
      ui.Settings->setVisible (show);
}

//------------------------------------------------------------------------------

void Interface::on_showWireframe_stateChanged(int val)
{
    Application->m_object->setWireMode(val == Qt::Checked);
    if (val == Qt::Checked)
    {
        ui.showTriangles->setChecked(true);
        ui.showTriangles->setEnabled(false);
    }
    else
    {
        ui.showTriangles->setEnabled(true);
    }
}

//------------------------------------------------------------------------------

void Interface::on_showEdges_stateChanged(int val)
{
    Application->m_mesh->setShowEdges(val == Qt::Checked);
}

//------------------------------------------------------------------------------

void Interface::on_showNormals_stateChanged(int val)
{
    Application->m_mesh->setShowNormals(val == Qt::Checked);
}

//------------------------------------------------------------------------------

void Interface::on_showBoundaryBox_stateChanged(int val)
{
    Application->m_object->setShowBoundaryBox(val == Qt::Checked);
}

//------------------------------------------------------------------------------

void Interface::on_showTriangles_stateChanged(int val)
{
    Application->m_mesh->setShowTriangles(val == Qt::Checked);
}

//------------------------------------------------------------------------------

void Interface::on_showFrame_stateChanged(int val)
{
    Application->m_object->setShowFrame(val == Qt::Checked);
}

//------------------------------------------------------------------------------

void Interface::on_folderList_clicked(const QModelIndex &index)
{
    QFileInfo info = dirModel->fileInfo(index);
    if (info.isFile())
    {

        if (Application) Application->loadModel(info.absoluteFilePath().toStdString());

        bool mesh = (Application->m_mesh == Application->m_object);

        ui.showWireframe->setEnabled(mesh);
        ui.showTriangles->setEnabled(mesh);
        ui.showEdges->setEnabled(mesh);
        ui.showNormals->setEnabled(mesh);

        ui.showFrame->setChecked(false);
        ui.showBoundaryBox->setChecked(false);
        ui.showTriangles->setChecked(mesh);
        ui.showWireframe->setChecked(false);
        ui.showEdges->setChecked(false);
        ui.showNormals->setChecked(false);
    }
}

//------------------------------------------------------------------------------

void Interface::on_folderList_doubleClicked(const QModelIndex &index)
{
    QFileInfo info = dirModel->fileInfo(index);
    if (info.isDir())
    {
        ui.folderList->setModel(dirModel);
        ui.folderList->setRootIndex(dirModel->index(info.absoluteFilePath()));
    }
}

//------------------------------------------------------------------------------

void Interface::UpdateStatus()
{
    if (Application)
    {
        GraphicRate.setText(QString("graphic: %1 Hz").arg((int)(Application->getGraphicRate()), 3));
        HapticRate.setText(QString("haptic: %1 Hz").arg((int)(Application->getHapticRate()), 4));
    }
    else 
    {
        GraphicRate.setText(QString("graphic: --- Hz"));
        HapticRate.setText(QString("haptic: ---- Hz"));
    }
}

//------------------------------------------------------------------------------
///++++++++++++
void Interface::mouseMoveEvent(QMouseEvent *event)

{

	// ����¼�������ȫ�������뱾�����꣬�ڴ�ֻ��Ҫ�������꼴��

	QPointF pt = event->localPos();

	// ��������ʽ��

	QString strShow = QString("��ǰ���꣺X:%1 Y:%2").arg(pt.x()).arg(pt.y());

	// ��SatusBar��ʵʱ��ʾ

	ui.statusBar->showMessage(strShow);

}

void Interface::onMsg()
{

	QMessageBox msgBox;   //#include <QMessageBox> ����Ҫ�����

	msgBox.setText("The document has been modified  0.0.");
	msgBox.exec();   //����exec() ����ʾ��Ϣ����򵥵�������ֻ������Ϣ�ı����ԡ�
}


void Interface::onQA()
{

	QMessageBox msgBox;   //#include <QMessageBox> ����Ҫ�����

	msgBox.setText("1.��δ�model \n2.����л�������е \n3.��ν�ͼ�����Լ���ŵ�ַ\n4.��ݼ� \n5.��ϵ����");
	msgBox.exec();   //����exec() ����ʾ��Ϣ����򵥵�������ֻ������Ϣ�ı����ԡ�
}




void Interface::close()
{

	QMessageBox msgBox;
	msgBox.setText("��������ϵ���ߣ�������������ҵʹ��");
	msgBox.setInformativeText("���뱣������peoject��?");
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Save);
	int ret = msgBox.exec();
}



/////////////////hubo