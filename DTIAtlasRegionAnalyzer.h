#ifndef DEF_HELLOWORLD
#define DEF_HELLOWORLD

#include <QApplication>
#include <QDialog>
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

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include <ui_GUI.h>
#include "itksys/Process.h"
#include <itksys/Configure.h>
#include "Scriptwriter.h"


class DTIAtlasRegionAnalyzer  : public QDialog, public Ui::DTIAtlasRegionAnalyzer
{
	Q_OBJECT

public:
	DTIAtlasRegionAnalyzer(int);
public slots:
	void GenerateDTIREG();
	void OpenOutputBrowseWindow();
	void OpenTemplateBrowseWindow();
	void OpenInputAtlasBrowseWindow();
	void OpenAddCaseBrowseWindow();
	void setDefault();
	void ReadCSVSlot();
	int ReadCSV(QString);
	void SaveCSVDatasetBrowse();
	void SaveCSVResults(int, int);
	void CheckCasesIndex();
	void removeCase();
	std::string setMethod();

	std::string setBrainsInitialDeformationField();
	std::string setBrainsNumberOfHistogramLevels();
	std::string setBrainsNumberOfPyramidLevels();
	std::string setBrainsNumberOfPyramidLevelIterations();
	std::string setInitialAffine();
	std::string setProgramsPathList();

	std::string setBrainsRegistrationType();
	std::string setBrainsInitializeTransformMode();

	std::string setNumberOfMatchPoints();
	std::string setAntsSimilarityMetric();
	std::string setAntsIterationsForDiffeomorphism();
	std::string setAntsGaussianSigma();

	std::string setAntsRegistrationType();
	std::string setAntsSimilarityParameter();

        int QuestionBox(QStringList);
	int setReturner(int);
	signals:




protected :
	QString m_lastCasePath;
	int m_ParamSaved;
	int returner;
private :
	Scriptwriter* m_scriptwriter;
	QString m_OutputPath;

};
#endif






