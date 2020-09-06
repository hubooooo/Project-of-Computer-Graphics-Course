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

//解决登录QT出现乱码的问题
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
	this->setMouseTracking(true);//若需要在鼠标移动的时候，就要获取到坐标点，则要进行鼠标追踪
	ui.centralWidget->setMouseTracking(true);


	QMenuBar* pMenuBar = ui.menuBar;   // 菜单栏
	QMenu* pMenuA = new QMenu("截图（&Q）");    //  新建一个菜单A //　q则为Alt方式的快捷键
	QMenu* pMenuB = new QMenu("HELP（&H）");    // W则为Alt方式的快捷键
	//QMenu* pMenuC = new QMenu("使用说明（&C）");    //  新建一个菜单C //C则为Alt方式的快捷键
	pMenuBar->addMenu(pMenuA);       //　将菜单A添加到菜单栏中， 菜单A出现在菜单栏的第一个1级菜单位置
	QAction* pActionA = new QAction("操作说明");          // 新建一个Action，然后对应的2级菜单 加入到菜单A中
	QAction* pActionB = new QAction("常见Q&&A");         //QIcon(QPixmap(":/img/1.png")),
	QAction* pActionD = new QAction("Contact author");
	//pMenuB = menuBar()->addMenu(QString::fromLocal8Bit("HELP（&H）"));//menuBar()在mainwindow中，也就是widget必须继承QMainWindow。
	//QAction *pNewFile = new QAction(QIcon(tr(":/img/1.png")), QString::fromLocal8Bit("Contact author"), this);//
	//pMenuB->addAction(pNewFile);//添加到“文件”下面
	pMenuBar->addMenu(pMenuB);       //　将菜单B添加到菜单栏中
	pMenuA->addAction(pActionA);  //菜单栏里加1级选项
	pMenuB->addAction(pActionB);     //　将1级菜单B再添加到MenuBar中
	pMenuB->addAction(pActionD);
	pActionA->setStatusTip(tr("成长"));              // 在widget 的窗口底部显示2级菜单的细节详情
	pActionB->setStatusTip(tr("子菜单的提示信息"));
	QObject::connect(pActionA, SIGNAL(triggered(bool)), this, SLOT(onMsg()));  //将Action和信号槽关联起来
	QObject::connect(pActionB, SIGNAL(triggered(bool)), this, SLOT(onQA()));  //将Action和信号槽关联起来
    QObject::connect(pActionD, SIGNAL(triggered(bool)), this, SLOT(close()));  //将Action和信号槽关联起来
	


	//Action的信号 ，通过帮助查询可知，当被点击的时候，会发送  triggered(bool) 信号 ，然后自己建立槽，或调用框架的槽函数响应即可
	QToolBar* pToolBar = ui.toolBar;    //添加toolbar到界面里

	QAction* pActionC = new QAction(QIcon(QPixmap(":/img/1.png")), "工具使用");

	pToolBar->addAction(pActionC);

	QObject::connect(pActionC, SIGNAL(triggered(bool)), this, SLOT(onMsg()));  //添加toolbar的点击功能


	QStatusBar* pStatusBar = ui.statusBar;
	pStatusBar->showMessage("CG2018's software");                    //初始化时显示状态属性

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

	// 鼠标事件包含了全局坐标与本地坐标，在此只需要本地坐标即可

	QPointF pt = event->localPos();

	// 将坐标点格式化

	QString strShow = QString("当前坐标：X:%1 Y:%2").arg(pt.x()).arg(pt.y());

	// 在SatusBar中实时显示

	ui.statusBar->showMessage(strShow);

}

void Interface::onMsg()
{

	QMessageBox msgBox;   //#include <QMessageBox> 必须要添加上

	msgBox.setText("The document has been modified  0.0.");
	msgBox.exec();   //调用exec() 来显示消息。最简单的配置是只设置消息文本属性。
}


void Interface::onQA()
{

	QMessageBox msgBox;   //#include <QMessageBox> 必须要添加上

	msgBox.setText("1.如何打开model \n2.如何切换手术器械 \n3.如何截图保存以及存放地址\n4.快捷键 \n5.联系作者");
	msgBox.exec();   //调用exec() 来显示消息。最简单的配置是只设置消息文本属性。
}




void Interface::close()
{

	QMessageBox msgBox;
	msgBox.setText("请勿尝试联系作者，本程序不用于商业使用");
	msgBox.setInformativeText("您想保留现有peoject吗?");
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Save);
	int ret = msgBox.exec();
}



/////////////////hubo