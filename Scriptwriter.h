#ifndef DEF_SCRIPTWRITER
#define DEF_SCRIPTWRITER

/*std classes*/
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QCloseEvent>
#include <QSignalMapper>
#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QStackedWidget>

/*itk classes*/
#include "itkImage.h"
#include "itkImageFileReader.h"

class Scriptwriter
{
	public:
		void openProgramScript(std::string Program);
		void addParameters(std::string Parameter);
		void openNewScript(std::string ScriptName, std::string OutputPath);
		void runScript();
	private:
		std::string parameters;
		std::string Script;
		std::string ScriptPath;
		QString m_OutputPath;
};
#endif
