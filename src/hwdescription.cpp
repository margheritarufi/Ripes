#include "hwdescription.h"
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include "processorregistry.h"
#include "processorhandler.h"
#include <QInputDialog>


//no Ripes namespace because otherwise I get LNK2019 error when I use QDir
QString QselectedPath;
QString QfolderName;

void downloadFiles(){
  QString selectedPath = openDirectoryDialog();
  Ripes::ProcessorID currentID = getProcessorType();
  createDirectory(selectedPath, currentID);
  //collectParameters();
  createParamsFile();
  //writeProcessorType();
  //writeNbStages();
  //writeWidth();
  //writeFwHz();
  //writeRegsInitialValues(); //better to write them one by one or create an xml?
  //writeActivePeripherals();
  //writeCacheSettings(); //instr and data
  //writeMemoryMap();
}

//This function creates the dialog window to choose the directory and the name of the file to
QString openDirectoryDialog(){
  //QString defaultFileName = "CPU_hw_description";
  QselectedPath = QFileDialog::getExistingDirectory(nullptr, "Select directory where the folder will be created", QDir::homePath());
  qDebug() << "Selected folder: " << QselectedPath;

  if (QselectedPath.isEmpty()) {
    std::cerr << "No folder chosen." << std::endl;
  }
  return QselectedPath;
}

Ripes::ProcessorID getProcessorType(){
  const Ripes::ProcessorID &ID = Ripes::ProcessorHandler::getID(); // Now ID contains the constant reference to the object of type ProcessorID
  Ripes::ProcessorID idValue = ID; // Now idValue contains the value of the object
  qDebug() << "Current Processor ID:" << idValue;
  return idValue;
}

void createDirectory(QString directoryPath, Ripes::ProcessorID ID){
  QString QstringID = processorIDToQString(ID);
  QfolderName = QstringID;
  if (!directoryPath.isEmpty()) {
    QfolderName = QInputDialog::getText(
        nullptr,
        "Name of the folder",
        "Choose a name for the folder"
        );
  }
  QDir dir(directoryPath);
  qDebug() << "Directory exists:" << dir.exists();
  dir.mkdir(QfolderName);
}

void createParamsFile(){
  std::string selectedDirectory = QselectedPath.toStdString();
  std::string folderName = QfolderName.toStdString();
  std::ofstream file(selectedDirectory + "/" + folderName + "/params.vh");

  if (file.is_open()) {
    file << "//These are automatically-generated parameters. The designer cannot change them because they depend on the user's choice of the processor" << std::endl;
    file.close();
    std::cout << "params.vh created successfully." << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh" << std::endl;
  }
}

QString processorIDToQString(Ripes::ProcessorID ID) {
  switch (ID) {
  case Ripes::RV32_SS:
    return "RV32_SS";
  case Ripes::RV32_5S_NO_FW_HZ:
    return "RV32_5S_NO_FW_HZ";
  case Ripes::RV32_5S_NO_HZ:
    return "RV32_5S_NO_HZ";
  case Ripes::RV32_5S_NO_FW:
    return "RV32_5S_NO_FW";
  case Ripes::RV32_5S:
    return "RV32_5S";
  case Ripes::RV32_6S_DUAL:
    return "RV32_6S_DUAL";
  case Ripes::RV64_SS:
    return "RV64_SS";
  case Ripes::RV64_5S_NO_FW_HZ:
    return "RV64_5S_NO_FW_HZ";
  case Ripes::RV64_5S_NO_HZ:
    return "RV64_5S_NO_HZ";
  case Ripes::RV64_5S_NO_FW:
    return "RV64_5S_NO_FW";
  case Ripes::RV64_5S:
    return "RV64_5S";
  case Ripes::RV64_6S_DUAL:
    return "RV64_6S_DUAL";
  case Ripes::NUM_PROCESSORS:
    return "NUM_PROCESSORS";
  default:
    return "Unknown Processor ID";
  }
}

