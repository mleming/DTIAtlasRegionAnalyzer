#include <QApplication>

#include "DTIAtlasRegionAnalyzer.h"
#include <stdio.h>
#include <stdlib.h>
#include <ui_GUI.h>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	DTIAtlasRegionAnalyzer window(5);

	window.show();

	return app.exec();
}
