
//------------------------------------------------------------------------------
#ifndef INTERFACE_H
#define INTERFACE_H
//------------------------------------------------------------------------------
#if defined(WIN32) | defined(WIN64)
#pragma warning(disable: 4100)
#endif
//------------------------------------------------------------------------------
#include "ui_Interface.h"
//------------------------------------------------------------------------------
#include "chai3d.h"
//------------------------------------------------------------------------------
#include <QFileSystemModel>
#include <QGLWidget>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>
#include <QShortcut>
#include <QTextCodec>

////++++++
/////+++++
#include <QObject>
#include <QAction>
#include <QIcon>
#include <QString> 
/////hubo ++++

//解决登录QT出现乱码的问题
#pragma execution_character_set("utf-8")   

//------------------------------------------------------------------------------
class ApplicationWidget;
//------------------------------------------------------------------------------

namespace Ui
{
    class InterfaceClass;
}

//------------------------------------------------------------------------------

class Interface : public QMainWindow
{
    Q_OBJECT

		//--------------------------------------------------------------------------
		// 构造函数和析构函数
		//--------------------------------------------------------------------------

public:

    Interface (QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~Interface ();


	//--------------------------------------------------------------------------
	// 私有成员变量- UI:
	//--------------------------------------------------------------------------

private:

    Ui::InterfaceClass ui;      
    QShortcut *EscKey;
    QShortcut *FKey;
    QShortcut *SKey;
    QShortcut *QKey;
    QTimer *StatusTimer;
	ApplicationWidget *Application;
    QLabel GraphicRate;
    QLabel HapticRate;
    QFileSystemModel *dirModel;

 //private:
	//Ui::InterfaceClass *uiprt;  //Interface类中，有一ui指针，为指向UI界面的类。可以通过ui指针，获取界面中的所有元素。
	//--------------------------------------------------------------------------
	// 私有成员变量:
	//--------------------

private:

    int AbortRequest;


	//--------------------------------------------------------------------------
	// 私有槽函数:
	//--------------------------------------------------------------------------

private slots:

    void on_showWireframe_stateChanged(int val);    //显示线框
    void on_showEdges_stateChanged(int val);         //显示边框
    void on_showNormals_stateChanged(int val);         //显示法向
    void on_showBoundaryBox_stateChanged(int val);       //显示边界盒
    void on_showTriangles_stateChanged(int val);           //显示三角片
    void on_showFrame_stateChanged(int val);                  //显示外边框
    void on_folderList_doubleClicked(const QModelIndex &index);   //双击显示列表
    void on_folderList_clicked(const QModelIndex &index);         //单机显示列表

    void EnterFullScreen();                   //进入全屏
    void ExitFullScreen();                    //退出全屏
    void ToggleFullScreen();                  //切换全屏
    void SetFullScreen(bool fs);              //设置全屏
    void ToggleSettings();                    //切换设置
    void ShowSettings(bool show);             //显示设置
    void UpdateStatus();                      //更新状态



////////////++++++++++
//////++++++++
protected:
virtual void mouseMoveEvent(QMouseEvent *event);

private slots:
void onMsg();   // 必须在.h中先声明 才能在.cpp里使用（.cpp 利用include 调用了）

private slots:
void onQA();

private slots:
void close();
////////++++  hubo

//--------------------------------------------------------------------------
// 公共变量:
//--------------------------------------------------------------------------

public:

    int  Start();                           //启动
    void Stop();                            //停止
};


//////////////


//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
