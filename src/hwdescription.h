#include "processorregistry.h"
#include "ripes_types.h"
#include <QDebug>
#include <QFileDialog>
#include <fstream>
#include <iostream>
#include <map>

// namespace Ripes {
// extern Ripes::ProcessorID globalID;
extern std::shared_ptr<std::ofstream> paramsFile;
extern std::map<unsigned int, Ripes::VInt>
    regsInitForHwDescription; // This type is equivalent to extern
                              // std::map<unsigned int, unsigned long long>.
                              // VInt is used to avoid error in the action
                              // "test.yml" (Ubuntu build)

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
void writeISAExtension(std::shared_ptr<std::ofstream> file);
std::shared_ptr<std::ofstream> createRegsFile();
void writeRegsInitialValues(std::shared_ptr<std::ofstream> file);
QString getNbStages(Ripes::ProcessorID ID);
QString getWidth(Ripes::ProcessorID ID);
QString getFw(Ripes::ProcessorID ID);
QString getHazard(Ripes::ProcessorID ID);
QString processorIDToQString(Ripes::ProcessorID ID);
std::string getISAExtension(QString ISAname);
std::string getAliasRegName(int i);
