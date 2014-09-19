#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <math.h>

#include <QApplication>
#include <QDialog>
#include <ui_GUI.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstdlib> // for getenv()
#include <signal.h> // for kill()

#include "DTIAtlasRegionAnalyzer.h"

/*Qt classes*/
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
#include <QLineEdit>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include <itksys/SystemTools.hxx>
#include "Scriptwriter.h"

/*Variables*/
std::string parameters = " ";
std::string Script;
std::string ScriptPath;
QString m_OutputPath;

void Scriptwriter::openNewScript(std::string ScriptName, std::string OutputPath){
	if(m_OutputPath.toStdString() == ""){
		m_OutputPath = QString::fromStdString(OutputPath);
		if(m_OutputPath.toStdString() == ""){
			std::cout << "Error: empty output path" << std::endl;
		}
		else{
			std::cout << "m_OutputPath is " + m_OutputPath.toStdString() << std::endl;		
			ScriptPath = m_OutputPath.toStdString() + "/scripts/" + ScriptName + ".script";
			Script = Script + "#!/bin/bash\n\n";
		}	
	}
	else{
		std::cout << "Cannot place a second output path." << std::endl;
	}

}

void Scriptwriter::openProgramScript(std::string Program){
	std::string program;
	std::string pathOut;
	std::string commandRan;
	std::vector< std::string > m_FindProgramDTIABExecDirVec;
	std::string DTIABExecutablePath= itksys::SystemTools::GetRealPath( itksys::SystemTools::GetFilenamePath(commandRan).c_str() );
	if(DTIABExecutablePath=="") DTIABExecutablePath= itksys::SystemTools::GetCurrentWorkingDirectory();
		m_FindProgramDTIABExecDirVec.push_back(DTIABExecutablePath);
	Program = itksys::SystemTools::FindProgram(Program.c_str(), m_FindProgramDTIABExecDirVec);

	std::cout<<"| $ " << program << std::endl;

	std::cout<<Program<<std::endl;
	//Script = Script + "os.system(\"" + Program + parameters + " &" + "\")\n\n";
	Script = Script + Program + parameters + "\n\n";
	std::cout<< Program + parameters <<std::endl;
	parameters = " ";
}

void Scriptwriter::runScript(){	
	QFile fileMain(ScriptPath.c_str());
	QProcess * chmodProcess = new QProcess;

	QDir output(m_OutputPath);
	output.mkdir(QString::fromStdString("scripts"));

	if(!fileMain.exists() || fileMain.remove()){
		std::cout<<"In here" <<std::endl;
	}

	if ( fileMain.open( IO_WriteOnly | IO_Translate ) ){
		QTextStream stream( &fileMain );
		stream << QString::fromStdString(Script) << endl;
	}

	std::string script_path_execution = "bash " + ScriptPath + " ";
	std::cout<<"| $ " << script_path_execution << std::endl;
	
	chmodProcess->execute( script_path_execution.c_str() );
}

void Scriptwriter::addParameters(std::string Parameter){
	parameters = parameters + " " + Parameter;
}








