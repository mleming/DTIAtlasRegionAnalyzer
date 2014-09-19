/*std classes*/
#include <vector>
#include <cstdlib> // for getenv() and exit()
#include <signal.h> // for kill()
#include <sys/wait.h> // for waitpid(): see function "void GUI::LaunchScriptRunner()" at the end
#include <csignal> // for signal()

/*itk classes*/
#include "itkImage.h"
#include "itkImageFileReader.h"
#include <itksys/SystemTools.hxx>
#include <itksys/Process.h>
#include <itksys/Configure.h>
#include <Scriptwriter.h>
#include <DTIAtlasRegionAnalyzer.h>

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
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSignalMapper>
#include <QAction>
#include <QFileInfo>

/*Variables*/
QString m_lastCasePath = "";
QString m_CSVseparator = QString(",");
mode_t ITKmode_F_OK = 0;
int returner = 0;

DTIAtlasRegionAnalyzer::DTIAtlasRegionAnalyzer(int value) : QDialog()
{
	setupUi(this);

	QObject::connect(Generate_QPushButton, SIGNAL(clicked()), this, SLOT(GenerateDTIREG()));
	QObject::connect(Browse_Output_QPushButton, SIGNAL(clicked()), this, SLOT(OpenOutputBrowseWindow()));
	QObject::connect(Template_QPushButton, SIGNAL(clicked()), this, SLOT(OpenTemplateBrowseWindow()));
	QObject::connect(Input_Atlas_QPushButton, SIGNAL(clicked()), this, SLOT(OpenInputAtlasBrowseWindow()));
	QObject::connect(Default_QPushButton, SIGNAL(clicked()), this, SLOT(setDefault()));
	QObject::connect(Load_Data_QPushButton, SIGNAL(clicked()), this, SLOT(ReadCSVSlot()));
	QObject::connect(Add_Case_QPushButton,SIGNAL(clicked()), this, SLOT(OpenAddCaseBrowseWindow()));
	QObject::connect(Remove_Case_QPushButton,SIGNAL(clicked()), this, SLOT(removeCase()));
}

void DTIAtlasRegionAnalyzer::GenerateDTIREG()
{
	m_scriptwriter = new Scriptwriter;
	m_OutputPath = Browse_Output_QLineEdit->text();
	m_scriptwriter->openNewScript("DTIAtlasRegionAnalyzer", m_OutputPath.toStdString());
	std::string DTI_Reg = "DTI-Reg_1.1.5";
	for (int i=0; i<CaseList_QListWidget->count(); i++){

		QFileInfo file(CaseList_QListWidget->item(i)->text().split(": ")[1]);

		m_scriptwriter->addParameters("--fixedVolume " + file.absoluteFilePath().toStdString());
		m_scriptwriter->addParameters("--movingVolume " + (Template_QLineEdit->text()).toStdString());
		m_scriptwriter->addParameters(setMethod());

		if(method_useScalar_BRAINS->isChecked()){
			m_scriptwriter->addParameters(setBrainsInitialDeformationField());
			m_scriptwriter->addParameters(setBrainsNumberOfHistogramLevels());
			m_scriptwriter->addParameters(setBrainsNumberOfPyramidLevels());
			m_scriptwriter->addParameters(setBrainsNumberOfPyramidLevelIterations());
			m_scriptwriter->addParameters(setInitialAffine());
			m_scriptwriter->addParameters(setProgramsPathList());
			m_scriptwriter->addParameters(setBrainsRegistrationType());
			m_scriptwriter->addParameters(setBrainsInitializeTransformMode());
		}else{
			m_scriptwriter->addParameters(setNumberOfMatchPoints());
			m_scriptwriter->addParameters(setAntsSimilarityMetric());
			m_scriptwriter->addParameters(setAntsIterationsForDiffeomorphism());
			m_scriptwriter->addParameters(setAntsGaussianSigma());
			m_scriptwriter->addParameters(setAntsRegistrationType());
			m_scriptwriter->addParameters(setAntsSimilarityParameter());
		}
		m_scriptwriter->addParameters("--outputFixedFAVolume " + QDir(m_OutputPath).filePath(file.baseName() + QString::fromStdString("_FixedFAVolume.nrrd")).toStdString());
		m_scriptwriter->addParameters("--outputResampledFAVolume " + QDir(m_OutputPath).filePath(file.baseName() +  QString::fromStdString("_ResampledFAVolume.nrrd")).toStdString());
		m_scriptwriter->addParameters("--outputTransform " + QDir(m_OutputPath).filePath(file.baseName() +  QString::fromStdString("_Transform.nrrd")).toStdString());
		m_scriptwriter->addParameters("--outputDisplacementField " + QDir(m_OutputPath).filePath(file.baseName() +  QString::fromStdString("_DisplacementField.nrrd")).toStdString());
		m_scriptwriter->addParameters("--outputMovingFAVolume " + QDir(m_OutputPath).filePath(file.baseName() +  QString::fromStdString("_MovingFAVolume.nrrd")).toStdString());

		m_scriptwriter->openProgramScript( DTI_Reg );
	}
	m_scriptwriter->runScript();
	delete m_scriptwriter;

}

  /////////////////////////////////////////
 // OUTPUT //
/////////////////////////////////////////

void DTIAtlasRegionAnalyzer::OpenOutputBrowseWindow() /*SLOT*/
{
	QString OutputBrowse=QFileDialog::getExistingDirectory(this);
	if(!OutputBrowse.isEmpty())
	{
		Browse_Output_QLineEdit->setText(OutputBrowse);
	}

}

  /////////////////////////////////////////
 // DTI-Reg //
/////////////////////////////////////////

void DTIAtlasRegionAnalyzer::OpenInputAtlasBrowseWindow() /*SLOT*/
{
	QString InputAtlasBrowse=QFileDialog::getOpenFileName(this, "Open Atlas Template", QString(), "NERD Image (*.nrrd *.nhdr *.*)");
	if(!InputAtlasBrowse.isEmpty())
	{
		Input_Atlas_QLineEdit->setText(InputAtlasBrowse);
	}

}

  /////////////////////////////////////////
 // TEMPLATE //
/////////////////////////////////////////

void DTIAtlasRegionAnalyzer::OpenTemplateBrowseWindow() /*SLOT*/
{
	QString TemplateBrowse=QFileDialog::getOpenFileName(this, "Open Atlas Template", QString(), "NERD Image (*.nrrd *.nhdr *.*)");
	if(!TemplateBrowse.isEmpty())
	{
		Template_QLineEdit->setText(TemplateBrowse);
	}
}

void DTIAtlasRegionAnalyzer::setDefault() /*SLOT*/
{
	
	Template_QLineEdit->setText(itksys::SystemTools::GetEnv("DTIREG_TEMPLATE"));
	Input_Atlas_QLineEdit->setText(itksys::SystemTools::GetEnv("DTIREG_INPUT_ATLAS"));
}

void DTIAtlasRegionAnalyzer::OpenAddCaseBrowseWindow() /*SLOT*/
{
	QStringList CaseListBrowse=QFileDialog::getOpenFileNames(this, "Open Cases", m_lastCasePath, "NERD Images (*.nrrd *.nhdr)");
	CaseList_QListWidget->addItems(CaseListBrowse);
	if(!CaseListBrowse.isEmpty())
	{
		if ( CaseList_QListWidget->count()>0 )
		{
			Remove_Case_QPushButton->setEnabled(true);
			Generate_QPushButton->setEnabled(true);
		}
	//	m_ParamSaved=0;
	//	Select_Cases_QLabel->setText( QString("") );
	//	m_lastCasePath = CaseListBrowse.last();

	//	CheckCasesIndex();
	}
}

void DTIAtlasRegionAnalyzer::removeCase()
{
	int row_select = CaseList_QListWidget->currentRow();
	CaseList_QListWidget->takeItem(CaseList_QListWidget->currentRow());
	if ( CaseList_QListWidget->count()<= 0 )
	{
		Remove_Case_QPushButton->setEnabled(false);
		Generate_QPushButton->setEnabled(false);
	}
	else {
		/*for (int i = 0; i < CaseList_QListWidget->count(); i++)
		{
			CaseList_QListWidget->insertItem(i, QString::fromStdString(std::to_string(i) + ": "+ (CaseList_QListWidget->takeItem(i)->text().split(": ")[1]).toStdString()));
		}*/
	
	}
	if (row_select >= CaseList_QListWidget->count())
		row_select = CaseList_QListWidget->count() - 1;
	//CaseList_QListWidget->setCurrentRow(row_select);
}

  /////////////////////////////////////////
 // DATASET //
/////////////////////////////////////////
void DTIAtlasRegionAnalyzer::ReadCSVSlot() /*SLOT*/
{
  QString CSVBrowse=QFileDialog::getOpenFileName(this, "Open CSV File", QString(), ".csv Files (*.csv)");

  if(!CSVBrowse.isEmpty())
  {
    ReadCSV(CSVBrowse);
  }
}

int DTIAtlasRegionAnalyzer::QuestionBox(QStringList list){
  QDialog dialog(this);
   // Use a layout allowing to have a label next to each field
  QFormLayout form(&dialog);
  // Add some text above the fields
  form.addRow(new QLabel("Which column in your file would you like to select?"));


  for (int i = 0;i < list.length();i++){
    QSignalMapper *signalMapper = new QSignalMapper( this );
    QRadioButton *radioButton = new QRadioButton(list[i],&dialog);
    signalMapper->setMapping( radioButton, i );
    //connect( website, SIGNAL(triggered()), signalMapper, SLOT(map()) );
    form.addRow(radioButton);
    //QObject::connect(radioButton, SIGNAL(toggled(bool)), &dialog, SLOT(setReturner(i)));
    QObject::connect(radioButton, SIGNAL(clicked(bool)), signalMapper, SLOT(map()));
    QObject::connect(signalMapper, SIGNAL(mapped(int)), SLOT(setReturner(int)));
  }
  //form.addRow(groupBox);

  // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
  QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
  form.addRow(&buttonBox);
  QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
  QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
  std::cout<<dialog.exec()<<std::endl;
  return returner;
}
int DTIAtlasRegionAnalyzer::setReturner(int i){
  returner = i;
  return returner;
}
int DTIAtlasRegionAnalyzer::ReadCSV(QString CSVfile)
{
  int ret=QMessageBox::Yes; // <=> replace current dataset by new
  if( CaseList_QListWidget->count()!=0 )
  {
    ret = QMessageBox::question(this,"Case list not empty","There are already some cases listed.\nDo you want to replace them by the new dataset ?\nIf No, the new dataset will be added to the existing cases.",QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes);

    if (ret == QMessageBox::Yes) CaseList_QListWidget->clear();
    else if (ret == QMessageBox::Cancel) return 0;
  }

  if(!CSVfile.isEmpty())
  {
    if( itksys::SystemTools::GetPermissions(CSVfile.toStdString().c_str(), ITKmode_F_OK) ) // Test if the csv file exists => itksys::SystemTools::GetPermissions() returns true if ITKmode_F(file)_OK
    {
      QFile file(CSVfile);
      if (file.open(QFile::ReadOnly))
      {
        std::cout<<"| Loading csv file \'"<< CSVfile.toStdString() <<"\'..."; // command line display
        QTextStream stream(&file);
        int column_choice = 1;
        while(!stream.atEnd()) //read all the lines
        {
          QString line = stream.readLine();
          if(line != "")
          {
            QStringList list = line.split(m_CSVseparator);

            
            if((list.at(0) != "id" && list.at(0) != "")) {
             CaseList_QListWidget->addItem( list.at(column_choice) ); //display in the Widget so that some can be removed
             
            }else{
////////////////////////HERERERERE///////////////////////
              column_choice = QuestionBox(list);
		column_choice = returner;
            }
          }
        }
        CheckCasesIndex();  
        if (ret == QMessageBox::Yes) Select_Cases_QLabel->setText( QString("Current CSV file : ") + CSVfile );
        else Select_Cases_QLabel->setText( QString("") );
        m_ParamSaved=0;
        std::cout<<"DONE"<<std::endl; // command line display
      }
      else
      {
        Select_Cases_QLabel->setText( QString("Could not open CSV File"));
        qDebug( "Could not open csv file");
        return -1;
      }

      if ( CaseList_QListWidget->count()>0 ){
        Remove_Case_QPushButton->setEnabled(true);
        Generate_QPushButton->setEnabled(true);
      }
    }
    else std::cout<<"| The given CSV file does not exist"<<std::endl; // command line display
  }

  return 0;
}

void DTIAtlasRegionAnalyzer::CheckCasesIndex() /* Change ids at the begining of the lines */
{
  std::string text;

  for(int i=0; i < CaseList_QListWidget->count() ;i++)
  {
    std::ostringstream outi;
    outi << i+1;
    std::string i_str = outi.str();

    if( CaseList_QListWidget->item(i)->text().contains(": ") ) text = i_str + ": " + CaseList_QListWidget->item(i)->text().toStdString().substr( CaseList_QListWidget->item(i)->text().split(":").at(0).size()+2 ); // from pos to the end
    else text = i_str + ": " + CaseList_QListWidget->item(i)->text().toStdString();

    CaseList_QListWidget->item(i)->setText( QString( text.c_str() ) );
  }
}

void DTIAtlasRegionAnalyzer::SaveCSVDatasetBrowse() /*SLOT*/
{
  if(CaseList_QListWidget->count()==0)
  {
    QMessageBox::critical(this, "No Dataset", "No Dataset");
    return;
  }

  QString CSVBrowseName = QFileDialog::getSaveFileName(this, tr("Save Dataset"),"./DTIAtlasBuilderDataSet.csv",tr("CSV File (*.csv)"));

  if(!CSVBrowseName.isEmpty())
  {

  QFile file(CSVBrowseName);

  if ( file.open( IO_WriteOnly | IO_Translate ) )
  {
    std::cout<<"| Generating Dataset csv file..."; // command line display

    QTextStream stream( &file );
    stream << QString("id") << m_CSVseparator << QString("Original DTI Image") << endl;
    for(int i=0; i < CaseList_QListWidget->count() ;i++) stream << i+1 << m_CSVseparator << CaseList_QListWidget->item(i)->text().remove(0, CaseList_QListWidget->item(i)->text().split(":").at(0).size()+2 ) << endl;
    std::cout<<"DONE"<<std::endl; // command line display
  
    Select_Cases_QLabel->setText( QString("Current CSV file : ") + CSVBrowseName );
    QMessageBox::information(this, "Saving succesful", "Dataset has been succesfully saved at" + CSVBrowseName);
  }
  else qDebug( "Could not create file");

  }
}

void DTIAtlasRegionAnalyzer::SaveCSVResults(int Crop, int nbLoops) // Crop = 0 if no cropping , 1 if cropping needed
{

  QString csvPath;
  csvPath = m_OutputPath + QString("/DTIAtlas/DTIAtlasBuilderResults.csv");
  QFile file(csvPath);

  if ( file.open( IO_WriteOnly | IO_Translate ) )
  {
    std::cout<<"| Generating Results csv file..."; // command line display

    QTextStream stream( &file );

    stream << QString("id") << m_CSVseparator << QString("Original DTI Image");
    if(Crop==1) stream << m_CSVseparator << QString("Cropped DTI");
    stream << m_CSVseparator << QString("FA from original") << m_CSVseparator << QString("Affine transform") << m_CSVseparator << QString("Affine registered DTI") << m_CSVseparator << QString("Affine Registered FA") << m_CSVseparator << QString("Diffeomorphic Deformed FA") << m_CSVseparator << QString("Diffeomorphic Deformation field to Affine space") << m_CSVseparator << QString("Diffeomorphic Inverse Deformation field to Affine space") << m_CSVseparator << QString("Diffeomorphic DTI") << m_CSVseparator << QString("Diffeomorphic Deformation field to Original space") << m_CSVseparator << QString("DTI-Reg Final DTI") << endl;

    for(int i=0; i < CaseList_QListWidget->count() ;i++) // for all cases
    {
      stream << i+1 << m_CSVseparator << CaseList_QListWidget->item(i)->text().remove(0, CaseList_QListWidget->item(i)->text().split(":").at(0).size()+2 ); // Original DTI Image
      if(Crop==1) stream << m_CSVseparator << m_OutputPath + QString("/DTIAtlas/1_Affine_Registration/Case") << i+1 << QString("_croppedDTI.nrrd"); // Cropped DTI
      stream << m_CSVseparator << m_OutputPath + QString("/DTIAtlas/1_Affine_Registration/Case") << i+1 << QString("_FA.nrrd"); // FA from original
      stream << m_CSVseparator << m_OutputPath + QString("/DTIAtlas/1_Affine_Registration/Loop") << nbLoops << QString("/Case") << i+1 << QString("_Loop ") << nbLoops << QString("_LinearTrans.txt"); // Affine transform
      stream << m_CSVseparator << m_OutputPath + QString("/DTIAtlas/1_Affine_Registration/Loop") << nbLoops << QString("/Case") << i+1 << QString("_Loop ") << nbLoops << QString("_LinearTrans_DTI.nrrd"); // Affine registered DTI
      stream << m_CSVseparator << m_OutputPath + QString("/DTIAtlas/1_Affine_Registration/Loop") << nbLoops << QString("/Case") << i+1 << QString("_Loop ") << nbLoops << QString("_FinalFA.nrrd"); // Affine Registered FA
      stream << m_CSVseparator << m_OutputPath + QString("/DTIAtlas/2_NonLinear_Registration/Case") << i+1 << QString("_NonLinearTrans_FA.mhd"); // Diffeomorphic Deformed FA
      stream << m_CSVseparator << m_OutputPath + QString("/DTIAtlas/2_NonLinear_Registration/Case") << i+1 << QString("_HField.mhd"); // Diffeomorphic Deformation H field to Affine space
      stream << m_CSVseparator << m_OutputPath + QString("/DTIAtlas/2_NonLinear_Registration/Case") << i+1 << QString("_InverseHField.mhd"); // Diffeomorphic Inverse Deformation H field to Affine space
      stream << m_CSVseparator << m_OutputPath + QString("/DTIAtlas/3_Diffeomorphic_Atlas/Case") << i+1 << QString("_DiffeomorphicDTI.nrrd"); // Diffeomorphic DTI
      stream << m_CSVseparator << m_OutputPath + QString("/DTIAtlas/4_Final_Resampling/Second_Resampling/Case") << i+1 << QString("_GlobalDisplacementField.nrrd"); // Diffeomorphic Deformation Displacement field to Original space
      stream << m_CSVseparator << m_OutputPath + QString("/DTIAtlas/4_Final_Resampling/Second_Resampling/Case") << i+1 << QString("_FinalDeformedDTI.nrrd"); // DTI-Reg Final DTI
      stream << endl;
    }

    std::cout<<"DONE"<<std::endl; // command line display
    
  }
  else qDebug( "Could not create file");
}



std::string DTIAtlasRegionAnalyzer::setAntsRegistrationType()
{
	if(ANTSReg_Elast->isChecked()){
		return "--ANTSRegistrationType Elast";
	}
	else if(ANTSReg_Exp->isChecked()){
		return "--ANTSRegistrationType Exp";
	}
	else if(ANTSReg_GreedyDiffeo->isChecked()){
		return "--ANTSRegistrationType GreedyDiffeo";
	}
	else if(ANTSReg_GreedyExp->isChecked()){
		return "--ANTSRegistrationType GreedyExp";
	}
	else if(ANTSReg_None->isChecked()){
		return "--ANTSRegistrationType None";
	}
	else if(ANTSReg_Rigid->isChecked()){
		return "--ANTSRegistrationType Rigid";
	}
	else if(ANTSReg_SpatioTempDiffeo->isChecked()){
		return "--ANTSRegistrationType SpatioTempDiffeo";
	}
	else if(ANTSReg_Affine->isChecked()){
		return "--ANTSRegistrationType Affine";
	}
	else
	{
		return "";
	}
}

std::string DTIAtlasRegionAnalyzer::setBrainsRegistrationType()
{
	if(BRAINSReg_None->isChecked()){
		return "--BRAINSRegistrationType None";
	}
	else if(BRAINSReg_Rigid->isChecked()){
		return "--BRAINSRegistrationType Rigid";
	}
	else if(BRAINSReg_Affine->isChecked()){
		return "--BRAINSRegistrationType Affine";
	}
	else if(BRAINSReg_BSpline->isChecked()){
		return "--BRAINSRegistrationType BSpline";
	}
	else if(BRAINSReg_Diffeomorphic->isChecked()){
		return "--BRAINSRegistrationType Diffeomorphic";
	}
	else if(BRAINSReg_Demons->isChecked()){
		return "--BRAINSRegistrationType Demons";
	}
	else if(BRAINSReg_FastSymmetricForces->isChecked()){
		return "--BRAINSRegistrationType FastSymmetricForces";
	}
	else
	{
		return "";
	}
}

std::string DTIAtlasRegionAnalyzer::setAntsSimilarityMetric()
{
	if(ANTSSimilarityMetic_MSQ->isChecked()){
		return "--ANTSSimilarityMetic MSQ";
	}
	else if(ANTSSimilarityMetric_CC->isChecked()){
		return "--ANTSSimilarityMetric CC";
	}
	else if(ANTSSimilarityMetric_MI->isChecked()){
		return "--ANTSSimilarityMetric MI";
	}
	else
	{
		return "";
	}
}
std::string DTIAtlasRegionAnalyzer::setBrainsInitializeTransformMode()
{
	if(BRAINSinitializeTransformMode_Off->isChecked()){
		return "--BRAINSinitializeTransformMode Off";
	}
	else if(BRAINSinitializeTransformMode_useCenterOfHeadAlign->isChecked()){
		return "--BRAINSinitializeTransformMode useCenterOfHeadAlign";
	}
	else if(BRAINSinitializeTransformMode_useGeometryAlign->isChecked()){
		return "--BRAINSinitializeTransformMode useGeometryAlign";
	}
	else if(BRAINSinitializeTransformMode_useMomentsAlign->isChecked()){
		return "--BRAINSinitializeTransformMode useMomentsAlign";
	}
	else
	{
		return "";
	}
}

std::string DTIAtlasRegionAnalyzer::setMethod()
{
	if(method_useScalar_ANTS->isChecked()){
		return "--method useScalar-ANTS";
	}
	else if(method_useScalar_BRAINS->isChecked()){
		return "--method useScalar-BRAINS";
	}
	else
	{
		return "";
	}
}
//In development


std::string DTIAtlasRegionAnalyzer::setNumberOfMatchPoints(){
	if(numberOfMatchPoints_QLineEdit->text() != ""){
		std::string param = numberOfMatchPoints_QLineEdit->text().toStdString();
		return "--numberOfMatchPoints " + param;
	}
	return "";
}
std::string DTIAtlasRegionAnalyzer::setBrainsNumberOfHistogramLevels(){
	if(BRAINSnumberOfHistogramLevels_QLineEdit->text() != ""){
		std::string param = BRAINSnumberOfHistogramLevels_QLineEdit->text().toStdString();
		return "--BRAINSnumberOfHistogramLevels " + param;
	}
	return "";
}
std::string DTIAtlasRegionAnalyzer::setBrainsNumberOfPyramidLevelIterations(){
	if(BRAINSarrayOfPyramidLevelIterations1_QLineEdit->text() != "" && BRAINSarrayOfPyramidLevelIterations2_QLineEdit->text() != "" && BRAINSarrayOfPyramidLevelIterations3_QLineEdit->text() != "" && BRAINSarrayOfPyramidLevelIterations4_QLineEdit->text() != "" && BRAINSarrayOfPyramidLevelIterations5_QLineEdit->text() != "" ){
		std::string param = BRAINSarrayOfPyramidLevelIterations1_QLineEdit->text().toStdString() + "," + BRAINSarrayOfPyramidLevelIterations2_QLineEdit->text().toStdString() + "," + BRAINSarrayOfPyramidLevelIterations3_QLineEdit->text().toStdString() + "," + BRAINSarrayOfPyramidLevelIterations4_QLineEdit->text().toStdString() + "," + BRAINSarrayOfPyramidLevelIterations5_QLineEdit->text().toStdString();
	
		return "--BRAINSarrayOfPyramidLevelIterations " + param;
	}
	return "";
}
std::string DTIAtlasRegionAnalyzer::setAntsSimilarityParameter(){

	if (ANTSSimilarityParameter_QLineEdit->text().toStdString() != ""){
		std::string param = ANTSSimilarityParameter_QLineEdit->text().toStdString();
		return "--ANTSSimilarityParameter " + param;
	}
	return "";

}
std::string DTIAtlasRegionAnalyzer::setAntsIterationsForDiffeomorphism(){

	if(ANTSIterations1_QLineEdit->text() != "" && ANTSIterations2_QLineEdit->text() != "" && ANTSIterations3_QLineEdit->text() != ""){
		std::string param = ANTSIterations1_QLineEdit->text().toStdString() + "x" + ANTSIterations2_QLineEdit->text().toStdString() + "x" + ANTSIterations3_QLineEdit->text().toStdString();
		return "--ANTSIterations " + param;
}

	return "";

}

std::string DTIAtlasRegionAnalyzer::setInitialAffine(){
	if(initialAffine_QLineEdit->text() != ""){
		std::string param = initialAffine_QLineEdit->text().toStdString();
			return "--initialAffine " + param;
	}
	return "";

}

std::string DTIAtlasRegionAnalyzer::setProgramsPathList(){
	if(programPathsVector_QLineEdit->text() != ""){
		std::string param = programPathsVector_QLineEdit->text().toStdString();
			return "--programPathsVector " + param;
	}
	return "";


}

std::string DTIAtlasRegionAnalyzer::setBrainsInitialDeformationField(){

	if(BRAINSinitialDeformationField_QLineEdit->text() != ""){
		std::string param = BRAINSinitialDeformationField_QLineEdit->text().toStdString();
			return "--BRAINSinitialDeformationField " + param;
	}
	return "";

}
std::string DTIAtlasRegionAnalyzer::setBrainsNumberOfPyramidLevels(){

	if(BRAINSnumberOfPyramidLevels_QLineEdit->text() != ""){
		std::string param = BRAINSnumberOfPyramidLevels_QLineEdit->text().toStdString();
		return "--BRAINSnumberOfPyramidLevels " + param;
	}
	return "";

}
std::string DTIAtlasRegionAnalyzer::setAntsGaussianSigma(){
	if(ANTSGaussianSigma_QLineEdit->text() != ""){
		std::string param = ANTSGaussianSigma_QLineEdit->text().toStdString();
		return "--ANTSGaussianSigma " + param;
	}
	return "";
}















