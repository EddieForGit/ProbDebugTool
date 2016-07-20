// main.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include <QtWidgets/qapplication.h>
#include "ProbDebugTool.h"


int main(int agc, char *agv[])
{
	QApplication app(agc, agv);

	ProbDebugTool p;



	return app.exec();
}

