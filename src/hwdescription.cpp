#include "hwdescription.h"
#include "hwdescriptioncache.h"
#include "processorhandler.h"
#include "processorregistry.h"
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <iomanip>
#include "ripes_types.h"

// no Ripes namespace because otherwise I get LNK2019 error when I use QDir
Ripes::ProcessorID thisID;
QString QcurrentID;
std::string currentID;
std::string selectedDirectory;
std::string folderName;
std::shared_ptr<std::ofstream> paramsFile;
std::shared_ptr<std::ofstream> regsFile;
std::map<unsigned int, Ripes::VInt> regsInitForHwDescription; //to try fix test.yml Ubuntu build action
//std::map<unsigned int, unsigned long long> regsInitForHwDescription;
// extern Ripes::RegisterInitialization regsInitForHwDescription =
// Ripes::ProcessorRegistry::getDescription(globalID).defaultRegisterVals; //as
// it is called by _selectProcessor in processorhandler.cpp before launching the
// gui)

void downloadFiles() {
  const Ripes::ProcessorID &ID = Ripes::ProcessorHandler::getID();
  // const Ripes::RegisterInitialization &setup =
  // Ripes::RegisterInitialization(); 0 ITEMS
  thisID = ID;
  // globalID = thisID;
  QcurrentID = getProcessorType();
  currentID = QcurrentID.toStdString();
  QString selectedPath = openDirectoryDialog();
  QString QfolderName = createDirectory(selectedPath);
  paramsFile = createParamsFile(selectedPath, QfolderName);
  writeProcessorType(paramsFile);
  writeNbStages(paramsFile);
  writeWidth(paramsFile);
  writeFwHz(paramsFile);
  // writeISAExtension();
  regsFile = createRegsFile();
  writeRegsInitialValues(regsFile);
  // xml? writeActivePeripherals();
  writeCacheSettings(paramsFile);
  // writeMemoryMap();
}

// This function creates the dialog window to choose the directory and the name
// of the file to
QString openDirectoryDialog() {
  QString QselectedPath = QFileDialog::getExistingDirectory(
      nullptr, "Select directory where the folder will be created",
      QDir::homePath());
  qDebug() << "Selected folder: " << QselectedPath;

  if (QselectedPath.isEmpty()) {
    std::cerr << "No folder chosen." << std::endl;
  }
  return QselectedPath;
}

QString getProcessorType() {
  Ripes::ProcessorID idValue =
      thisID; // Now idValue contains the value of the object
  qDebug() << "Current Processor ID:" << idValue;
  QString QstringID = processorIDToQString(idValue);
  return QstringID;
}

QString createDirectory(QString directoryPath) {
  QString QfolderName;
  if (!directoryPath.isEmpty()) {
    QfolderName = QInputDialog::getText(nullptr, "Name of the folder",
                                        "Choose a name for the folder");
  }
  if (QfolderName == "") {
    QfolderName = QcurrentID;
  }
  QDir dir(directoryPath);
  qDebug() << "Directory exists:" << dir.exists();
  dir.mkdir(QfolderName);
  return QfolderName;
}

std::shared_ptr<std::ofstream> createParamsFile(QString directoryPath,
                                                QString QfolderName) {
  selectedDirectory = directoryPath.toStdString();
  folderName = QfolderName.toStdString();
  auto file = std::make_shared<std::ofstream>(
      selectedDirectory + "/" + folderName + "/params.vh", std::ios::app);

  if (file->is_open()) {
    (*file) << "//These are automatically-generated parameters."
            << "\n"
            << "//The designer cannot change them because they depend on the "
               "user's choice of the processor"
            << std::endl;
    // file.close();
    std::cout << "params.vh created successfully." << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh" << std::endl;
  }
  return file;
}

void writeProcessorType(std::shared_ptr<std::ofstream> file) {
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(15) << "PROC_TYPE "
            << currentID << std::endl;
    std::cout << "Processor ID successfully written in params.vh." << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the processor ID"
              << std::endl;
  }
}

void writeNbStages(std::shared_ptr<std::ofstream> file) {
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(15) << "NB_STAGES "
            << getNbStages(thisID).toStdString() << std::right << std::setw(35)
            << " //Possible values: 1, 5, 6" << std::endl;
    std::cout << "Processor ID successfully written in params.vh." << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the processor ID"
              << std::endl;
  }
}

void writeWidth(std::shared_ptr<std::ofstream> file) {
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(15) << "DATA_WIDTH "
            << getWidth(thisID).toStdString() << std::right << std::setw(35)
            << " //Possible values: 32, 64" << std::endl;
    (*file) << "`define " << std::left << std::setw(15) << "ADDR_WIDTH "
            << getWidth(thisID).toStdString() << std::right << std::setw(35)
            << " //Possible values: 32, 64" << std::endl;
    std::cout << "Processor ID successfully written in params.vh." << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the processor ID"
              << std::endl;
  }
}

void writeFwHz(std::shared_ptr<std::ofstream> file) {
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(15) << "FW_PATH "
            << getFw(thisID).toStdString() << std::right << std::setw(35)
            << " //Possible values: 1/0 : yes/no" << std::endl;
    (*file) << "`define " << std::left << std::setw(15) << "HZ_DETECT "
            << getHazard(thisID).toStdString() << std::right << std::setw(35)
            << " //Possible values: 1/0 : yes/no" << std::endl;
    std::cout << "Processor ID successfully written in params.vh." << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the processor ID"
              << std::endl;
  }
}

std::shared_ptr<std::ofstream> createRegsFile() {
  auto file = std::make_shared<std::ofstream>(
      selectedDirectory + "/" + folderName + "/registerfile.xml",
      std::ios::app);

  if (file->is_open()) {
    (*file) << "<<!--This file describes the register file at its initial "
               "status -->"
            << std::endl;
    // file.close();
    std::cout << "registerfile.xml created successfully." << std::endl;
  } else {
    std::cerr << "Ripes couldn't open registerfile.xml" << std::endl;
  }

  return file;
}

void writeRegsInitialValues(std::shared_ptr<std::ofstream> file) {
  //Ripes::RegisterInitialization localRegsInit =
  //    static_cast<Ripes::RegisterInitialization>(regsInitForHwDescription);
  Ripes::RegisterInitialization localRegsInit = regsInitForHwDescription;
  qDebug() << "Just for breakpoint";

  // Generate the XML content
  if (file->is_open()) {
    (*file) << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    (*file) << "<register_file>" << std::endl;
    (*file) << "  <name>RegisterFile</name>" << std::endl;
    (*file) << "  <description>Initial status of the register file of the "
               "processor</description>"
            << std::endl;
    (*file) << "  <registers>" << std::endl;

    // Create individual register elements
    for (int i = 0; i < 32; i++) {
      if (regsInitForHwDescription[i] != 0) {
        (*file) << "    <register>" << std::endl;
        (*file) << "      <name>x" << i << "</name>" << std::endl;
        (*file) << "      <alias>alias name</alias>" << std::endl;
        (*file) << "      <address>0x" << std::hex << i * 4 << "</address>"
                << std::endl; //??????
        (*file) << "      <value>0x" << regsInitForHwDescription[i]
                << "</value>" << std::endl;
        (*file) << "    </register>" << std::endl;
      }
    }

    (*file) << "  </registers>" << std::endl;
    (*file) << "</register_file>" << std::endl;

    // Close the file
    (*file).close();
  }
}

QString getNbStages(Ripes::ProcessorID ID) {
  switch (ID) {
  case Ripes::RV32_SS:
    return "1";
  case Ripes::RV32_5S_NO_FW_HZ:
    return "5";
  case Ripes::RV32_5S_NO_HZ:
    return "5";
  case Ripes::RV32_5S_NO_FW:
    return "5";
  case Ripes::RV32_5S:
    return "5";
  case Ripes::RV32_6S_DUAL:
    return "6";
  case Ripes::RV64_SS:
    return "1";
  case Ripes::RV64_5S_NO_FW_HZ:
    return "5";
  case Ripes::RV64_5S_NO_HZ:
    return "5";
  case Ripes::RV64_5S_NO_FW:
    return "5";
  case Ripes::RV64_5S:
    return "5";
  case Ripes::RV64_6S_DUAL:
    return "6";
  case Ripes::NUM_PROCESSORS:
    return "NUM_PROCESSORS";
  default:
    return "Unknown Processor ID";
  }
}

QString getWidth(Ripes::ProcessorID ID) {
  switch (ID) {
  case Ripes::RV32_SS:
    return "32";
  case Ripes::RV32_5S_NO_FW_HZ:
    return "32";
  case Ripes::RV32_5S_NO_HZ:
    return "32";
  case Ripes::RV32_5S_NO_FW:
    return "32";
  case Ripes::RV32_5S:
    return "32";
  case Ripes::RV32_6S_DUAL:
    return "6";
  case Ripes::RV64_SS:
    return "64";
  case Ripes::RV64_5S_NO_FW_HZ:
    return "64";
  case Ripes::RV64_5S_NO_HZ:
    return "64";
  case Ripes::RV64_5S_NO_FW:
    return "64";
  case Ripes::RV64_5S:
    return "64";
  case Ripes::RV64_6S_DUAL:
    return "64";
  case Ripes::NUM_PROCESSORS:
    return "NUM_PROCESSORS";
  default:
    return "Unknown Processor ID";
  }
}

QString getFw(Ripes::ProcessorID ID) {
  switch (ID) {
  case Ripes::RV32_5S_NO_HZ:
    return "1";
  case Ripes::RV32_5S:
    return "1";
  case Ripes::RV32_6S_DUAL:
    return "1";
  case Ripes::RV64_5S_NO_HZ:
    return "1";
  case Ripes::RV64_5S:
    return "1";
  case Ripes::RV64_6S_DUAL:
    return "1";
  case Ripes::NUM_PROCESSORS:
    return "NUM_PROCESSORS";
  default:
    return "0";
  }
}

QString getHazard(Ripes::ProcessorID ID) {
  switch (ID) {
  case Ripes::RV32_5S_NO_FW:
    return "1";
  case Ripes::RV32_5S:
    return "1";
  case Ripes::RV32_6S_DUAL:
    return "1";
  case Ripes::RV64_5S_NO_FW:
    return "1";
  case Ripes::RV64_5S:
    return "1";
  case Ripes::RV64_6S_DUAL:
    return "1";
  case Ripes::NUM_PROCESSORS:
    return "NUM_PROCESSORS";
  default:
    return "0";
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
