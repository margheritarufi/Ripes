#include "processorregistry.h"
#include <QDebug>
#include <QFileDialog>
#include <iostream>
#include <fstream>

//namespace Ripes {
extern std::shared_ptr<std::ofstream> paramsFile;

void downloadFiles();
QString openDirectoryDialog();
QString getProcessorType();
QString createDirectory(QString directoryPath);
std::shared_ptr<std::ofstream> createParamsFile(QString directoryPath, QString QfolderName);
void writeProcessorType(std::shared_ptr<std::ofstream> file);
void writeNbStages(std::shared_ptr<std::ofstream> file);
void writeWidth(std::shared_ptr<std::ofstream> file);
void writeFwHz(std::shared_ptr<std::ofstream> file);
QString getNbStages(Ripes::ProcessorID ID);
QString getWidth(Ripes::ProcessorID ID);
QString getFw(Ripes::ProcessorID ID);
QString getHazard(Ripes::ProcessorID ID);
QString processorIDToQString(Ripes::ProcessorID ID);
