
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

//�����¼QT�������������
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
		// ���캯������������
		//--------------------------------------------------------------------------

public:

    Interface (QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~Interface ();


	//--------------------------------------------------------------------------
	// ˽�г�Ա����- UI:
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
	//Ui::InterfaceClass *uiprt;  //Interface���У���һuiָ�룬Ϊָ��UI������ࡣ����ͨ��uiָ�룬��ȡ�����е�����Ԫ�ء�
	//--------------------------------------------------------------------------
	// ˽�г�Ա����:
	//--------------------

private:

    int AbortRequest;


	//--------------------------------------------------------------------------
	// ˽�вۺ���:
	//--------------------------------------------------------------------------

private slots:

    void on_showWireframe_stateChanged(int val);    //��ʾ�߿�
    void on_showEdges_stateChanged(int val);         //��ʾ�߿�
    void on_showNormals_stateChanged(int val);         //��ʾ����
    void on_showBoundaryBox_stateChanged(int val);       //��ʾ�߽��
    void on_showTriangles_stateChanged(int val);           //��ʾ����Ƭ
    void on_showFrame_stateChanged(int val);                  //��ʾ��߿�
    void on_folderList_doubleClicked(const QModelIndex &index);   //˫����ʾ�б�
    void on_folderList_clicked(const QModelIndex &index);         //������ʾ�б�

    void EnterFullScreen();                   //����ȫ��
    void ExitFullScreen();                    //�˳�ȫ��
    void ToggleFullScreen();                  //�л�ȫ��
    void SetFullScreen(bool fs);              //����ȫ��
    void ToggleSettings();                    //�л�����
    void ShowSettings(bool show);             //��ʾ����
    void UpdateStatus();                      //����״̬



////////////++++++++++
//////++++++++
protected:
virtual void mouseMoveEvent(QMouseEvent *event);

private slots:
void onMsg();   // ������.h�������� ������.cpp��ʹ�ã�.cpp ����include �����ˣ�

private slots:
void onQA();

private slots:
void close();
////////++++  hubo

//--------------------------------------------------------------------------
// ��������:
//--------------------------------------------------------------------------

public:

    int  Start();                           //����
    void Stop();                            //ֹͣ
};


//////////////


//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
