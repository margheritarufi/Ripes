//@file hwdescription.h
//@author Margherita Rufi
//@version 1.0 2023-11-26
//@brief This file contains the declaration of the functions of
// hwdescription.cpp.

#include "processorregistry.h"
#include "ripes_types.h"
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>

// namespace Ripes {
extern Ripes::ProcessorID thisID;
extern std::shared_ptr<std::ofstream> paramsFile;
extern std::string selectedDirectory;
extern std::string folderName;
extern std::shared_ptr<std::ofstream> regsFile;
extern std::shared_ptr<std::ofstream> memoryMapFile;
extern std::map<unsigned int, Ripes::VInt>
    regsInitForHwDescription; // This type is equivalent to extern
                              // std::map<unsigned int, unsigned long long>.
                              // VInt is used to avoid error in the GitHub
                              // action "test.yml" (Ubuntu build)
extern std::string paramsFileName;

void downloadFiles();
QString openDirectoryDialog();
QString getProcessorType();
QString createFolder(QString directoryPath);
std::shared_ptr<std::ofstream>
createParamsFile(); // QString directoryPath, QString QfolderName);
void writeProcessorType(std::shared_ptr<std::ofstream> file);
void writeNbStages(std::shared_ptr<std::ofstream> file);
void writeWidth(std::shared_ptr<std::ofstream> file);
void writeFwHz(std::shared_ptr<std::ofstream> file);
void writeISAExtension(std::shared_ptr<std::ofstream> file);
int getNbStages(Ripes::ProcessorID ID);
int getWidth(Ripes::ProcessorID ID);
int getFw(Ripes::ProcessorID ID);
int getHazard(Ripes::ProcessorID ID);
QString processorIDToQString(Ripes::ProcessorID ID);
std::string getISAExtension(QString ISAname);
QString createDialogFolderName(QInputDialog &inputDialog);
int createWarningBox(QMessageBox &msgBox, QString localQfolderName);
void printVerilogDefine(std::shared_ptr<std::ofstream> file, std::string name,
                        int value, std::string comment = "");
void sendOutputStream(std::string objectName, std::string fileName);
void sendErrorStream(std::string objectName, std::string fileName);
