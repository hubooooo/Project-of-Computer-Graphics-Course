//==============================================================================
/*
    \author    Sebastien Grange
*/
//==============================================================================


//------------------------------------------------------------------------------
#include "Interface.h"
//------------------------------------------------------------------------------
//解决登录QT出现乱码的问题
#pragma execution_character_set("utf-8")   

int main (int argc, char *argv[])
{

    QApplication a (argc, argv);
    Interface    w;

	//QApplication::addLibraryPath("../../../bin/1/plugins");

    w.show ();
    w.Start ();
	w.setWindowTitle("CG2018");
    return a.exec ();
}

//------------------------------------------------------------------------------
