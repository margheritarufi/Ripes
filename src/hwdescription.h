#include "processorregistry.h"
#include <QDebug>
#include <QFileDialog>
#include <fstream>
#include <iostream>
#include <map>
#include "ripes_types.h"

// namespace Ripes {
// extern Ripes::ProcessorID globalID;
extern std::shared_ptr<std::ofstream> paramsFile;
extern std::map<unsigned int, Ripes::VInt> regsInitForHwDescription; //to try fix test.yml Ubuntu build action
//extern std::map<unsigned int, unsigned long long>
//    regsInitForHwDescription; // =
                              // Ripes::ProcessorRegistry::getDescription(globalID).defaultRegisterVals;
                              // //as it is called by _selectProcessor in
                              // processorhandler.cpp before launching the gui)

void downloadFiles();
QString openDirectoryDialog();
QString getProcessorType();
QString createDirectory(QString directoryPath);
std::shared_ptr<std::ofstream> createParamsFile(QString directoryPath,
                                                QString QfolderName);
void writeProcessorType(std::shared_ptr<std::ofstream> file);
void writeNbStages(std::shared_ptr<std::ofstream> file);
void writeWidth(std::shared_ptr<std::ofstream> file);
void writeFwHz(std::shared_ptr<std::ofstream> file);
std::shared_ptr<std::ofstream> createRegsFile();
void writeRegsInitialValues(std::shared_ptr<std::ofstream> file);
QString getNbStages(Ripes::ProcessorID ID);
QString getWidth(Ripes::ProcessorID ID);
QString getFw(Ripes::ProcessorID ID);
QString getHazard(Ripes::ProcessorID ID);
QString processorIDToQString(Ripes::ProcessorID ID);
