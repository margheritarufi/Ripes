//@file hwdescription.cpp
//@author Margherita Rufi
//@version 1.0 2023-11-13
//@brief This file contains the functions that create the verilog files that
// describe the processor hardware.

#include "hwdescription.h"
#include "hwdescriptioncache.h"
#include "io/iomanager.h"
#include "processorhandler.h"
#include "processorregistry.h"
#include "ripes_types.h"
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <iomanip>

// no Ripes namespace because otherwise I get LNK2019 error when I use QDir
Ripes::ProcessorID thisID;
QString QcurrentID;
std::string currentID;
std::string selectedDirectory;
std::string folderName;
std::shared_ptr<std::ofstream> paramsFile;
std::shared_ptr<std::ofstream> regsFile;
std::shared_ptr<std::ofstream> memoryMapFile;
std::map<unsigned int, Ripes::VInt>
    regsInitForHwDescription; // See .h file for info about the type

/**
 * @brief downloadFiles
 * This function is called when the user clicks on the "Download Files" button
 * in the Processor tab. It calls the functions to create the verilog files that
 * describe the processor hardware.
 *
 * @param void
 * @return void
 * */
void downloadFiles() {
  const Ripes::ProcessorID &ID = Ripes::ProcessorHandler::getID();
  // const Ripes::RegisterInitialization &setup =
  // Ripes::RegisterInitialization(); 0 ITEMS
  thisID = ID;
  QcurrentID = getProcessorType();
  currentID = QcurrentID.toStdString();
  QString selectedPath = openDirectoryDialog();
  if (!selectedPath.isEmpty()) {
    QString QfolderName = createFolder(selectedPath);
    if (QfolderName != "") {
      paramsFile = createParamsFile(selectedPath, QfolderName);
      writeProcessorType(paramsFile);
      writeNbStages(paramsFile);
      writeWidth(paramsFile);
      writeFwHz(paramsFile);
      writeISAExtension(paramsFile);
      writeNbPeriph(paramsFile);
      regsFile = createRegsFile();
      writeRegsInitialValues(regsFile);
      // xml? writeActivePeripherals();
      writeCacheSettings(paramsFile);
      memoryMapFile = createMemoryMapFile();
      writeMemoryMap(memoryMapFile);
    }
  }
}

// @brief openDirectoryDialog
// This function is called by the downloadFiles() function. It creates a dialog
// window to choose the directory where the files will be created.
//
// @param void
// @return QString with the path of the selected directory
QString openDirectoryDialog() {
  // Create a dialog window to choose the directory where the folder will be
  // created
  QString QselectedPath = QFileDialog::getExistingDirectory(
      nullptr, "Select directory where the folder will be created",
      QDir::homePath());
  if (!QselectedPath.isEmpty()) {
    std::cout << "Selected directory: " << QselectedPath.toStdString()
              << std::endl;
  } else {
    std::cerr << "No directory chosen." << std::endl;
  }
  return QselectedPath;
}

// @brief getProcessorType
// This function is called by the downloadFiles() function. It returns the
// processor type chosen by the user as a QString.
//
// @param void
// @return QString with the processor type
QString getProcessorType() {
  // Ripes::ProcessorID idValue = thisID;
  // Transform the processor ID from the Ripes::ProcessorID type to a QString
  QString QstringID = processorIDToQString(thisID);
  return QstringID;
}

// @brief createFolder
// This function is called by the downloadFiles() function. It creates a folder
// in directoryPath with the name chosen by the user.
//
// @param QString with the path of the selected directory
// @return QString with the name of the created folder
QString createFolder(QString directoryPath) {
  QString QfolderName;

  // Create a dialog window to choose the folder name
  QInputDialog inputDialogFolderName;
  QfolderName = createDialogFolderName(inputDialogFolderName);

  // If the user has chosen a name, check if a folder with the chosen name
  // already exists. If it does, open a warning window.
  if (QfolderName != "") {
    QDir completeDirectoryToCheck(directoryPath + "/" + QfolderName);
    int resultWarningBox = 0;
    QInputDialog inputDialogFolderNameRetry;
    bool overwrite = false;

    // Continue opening a warning window until:
    // 1. the user chooses a name of a folder that doesn't exist yet
    // 2. the user chooses to overwrite an existing folder
    // 3. the user closes the DialogFolderName Window
    while (completeDirectoryToCheck.exists() &&
           resultWarningBox != QMessageBox::Yes) {
      QMessageBox warningOverwriteBox;
      resultWarningBox = createWarningBox(warningOverwriteBox, QfolderName);

      // If the user clicks on "Cancel" or "X" of the warning message box, open
      // a new dialog window to choose the folder name If the user clicks on
      // "Yes", the chosen folder will be overwritten with the new files.
      if (resultWarningBox == QMessageBox::Cancel) {
        QfolderName = createDialogFolderName(inputDialogFolderNameRetry);
        // If the user has chosen a new name, update the directory with the new
        // chosen name to check in the next while loop cycle if it already
        // exists. Else, exit the while loop.
        if (QfolderName != "") {
          completeDirectoryToCheck.setPath(directoryPath + "/" + QfolderName);
        } else {
          break;
        }
      } else {
        std::cerr << directoryPath.toStdString() + "/" +
                         QfolderName.toStdString()
                  << " folder will be overwritten." << std::endl;
        overwrite = true;
      }
    }

    // Check again if the user has chosen a new name
    if (QfolderName != "") {
      // Once the selected folder name is confirmed, create the folder
      QDir dir(directoryPath);
      bool resultFolderCreation = dir.mkdir(QfolderName);
      if (resultFolderCreation) {
        std::cout << "The folder was created successfully." << std::endl;
      } else if (overwrite == false) {
        std::cerr << "Ripes couldn't create the folder." << std::endl;
      }
    }
  }
  return QfolderName;
}

// @brief createParamsFile
// This function is called by the downloadFiles() function. It creates the
// params.vh file in the created folder and writes a presentation comment.
//
// @param QString with the path of the selected directory
// @param QString with the name of the created folder
// @return std::shared_ptr<std::ofstream> with the pointer to the created file
std::shared_ptr<std::ofstream> createParamsFile(QString directoryPath,
                                                QString QfolderName) {
  // Create the params.vh file
  selectedDirectory = directoryPath.toStdString();
  folderName = QfolderName.toStdString();
  auto file = std::make_shared<std::ofstream>(
      selectedDirectory + "/" + folderName + "/params.vh", std::ios::out);

  // Write a presentation comment in the file
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

// @brief writeProcessorType
// This function is called by the downloadFiles() function. It writes the
// processor type in the params.vh file.
//
// @param std::shared_ptr<std::ofstream> with the pointer to the created
// "params.vh" file
// @return void
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

//@brief writeNbStages
// This function is called by the downloadFiles() function. It writes the number
// of stages in the params.vh file.
//
// @param std::shared_ptr<std::ofstream> with the pointer to the created
// "params.vh" file
// @return void
void writeNbStages(std::shared_ptr<std::ofstream> file) {
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(15) << "NB_STAGES "
            << getNbStages(thisID).toStdString() << std::right << std::setw(35)
            << " //Possible values: 1, 5, 6" << std::endl;
    std::cout << "Number of stages successfully written in params.vh."
              << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the number of stages"
              << std::endl;
  }
}

//@brief writeWidth
// This function is called by the downloadFiles() function. It writes the width
// of data and addresses in the params.vh file.
//
// @param std::shared_ptr<std::ofstream> with the pointer to the created
// "params.vh" file
// @return void
void writeWidth(std::shared_ptr<std::ofstream> file) {
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(15) << "DATA_WIDTH "
            << getWidth(thisID).toStdString() << std::right << std::setw(35)
            << " //Possible values: 32, 64" << std::endl;
    (*file) << "`define " << std::left << std::setw(15) << "ADDR_WIDTH "
            << getWidth(thisID).toStdString() << std::right << std::setw(35)
            << " //Possible values: 32, 64" << std::endl;
    std::cout
        << "Width of data and addresses successfully written in params.vh."
        << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the width of data and "
                 "addresses"
              << std::endl;
  }
}

//@brief writeFwHz
// This function is called by the downloadFiles() function. It writes the
// forwarding path and hazard detection unit parameters in the params.vh file.
// The parameters are equal to 1 if the processor has the forwarding path and/or
// the hazard detection unit, 0 otherwise.
//
// @param std::shared_ptr<std::ofstream> with the pointer to the created
// "params.vh" file
// @return void
void writeFwHz(std::shared_ptr<std::ofstream> file) {
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(15) << "FW_PATH "
            << getFw(thisID).toStdString() << std::right << std::setw(35)
            << " //Possible values: 1/0 : yes/no" << std::endl;
    (*file) << "`define " << std::left << std::setw(15) << "HZ_DETECT "
            << getHazard(thisID).toStdString() << std::right << std::setw(35)
            << " //Possible values: 1/0 : yes/no" << std::endl;
    std::cout << "Forwarding Path and Hazard Detection Unit parameters "
                 "successfully written in params.vh."
              << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the Forwarding Path "
                 "and Hazard Detection Unit parameters"
              << std::endl;
  }
}

//@brief writeISAExtension
// This function is called by the downloadFiles() function. It writes the ISA
// extension code in the params.vh file. The parameter is equal to 00 if the ISA
// doesn't implement any extension, 10 if it implements the M extension, 01 if
// it implements the C extension, 11 if it implements both the M and C.
//
// @param std::shared_ptr<std::ofstream> with the pointer to the created
// "params.vh" file
// @return void
void writeISAExtension(std::shared_ptr<std::ofstream> file) {
  QString QISAname =
      Ripes::ProcessorHandler::getProcessor()->implementsISA()->name();
  std::string exstensionCode = getISAExtension(QISAname);
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(15) << "EXTENSION "
            << exstensionCode << std::right << std::setw(35)
            << " //Possible values: 00, 01, 10, 11 (MC extensions: 1=enabled, "
               "0=disabled)"
            << std::endl;
    std::cout << "ISA extension code successfully written in params.vh."
              << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the ISA extension code"
              << std::endl;
  }
}

//@brief createRegsFile
// This function is called by the downloadFiles() function. It creates the
// registerfile.xml file in the folder created by createDirectory() and writes
// a presentation comment in it.
//
// @param void
// @return std::shared_ptr<std::ofstream> with the pointer to the created file
std::shared_ptr<std::ofstream> createRegsFile() {
  // Create the registerfile.xml file
  auto file = std::make_shared<std::ofstream>(
      selectedDirectory + "/" + folderName + "/registerfile.xml",
      std::ios::out);

  // Write a presentation comment in the file
  if (file->is_open()) {
    (*file) << "<<!--This file describes the register file at its initial "
               "status -->"
            << std::endl;
    // file.close();
    std::cout << "registerfile.xml created successfully." << std::endl;
  } else {
    std::cerr << "Ripes couldn't create registerfile.xml" << std::endl;
  }

  return file;
}

//@brief writeRegsInitialValues
// This function is called by the downloadFiles() function. It writes the
// initial values of the registers in the registerfile.xml file. It uses the
// values stored in the regsInitForHwDescription variable. This variable is
// written before the launch of Ripes GUI with the default values of the
// registers and then it is overwritten any time the user sets the inital
// register values from the Select Processor window (Register Initialization
// section). See processortab.cpp.
//
// @param std::shared_ptr<std::ofstream> with the pointer to the created
// "registerfile.xml" file
void writeRegsInitialValues(std::shared_ptr<std::ofstream> file) {
  Ripes::RegisterInitialization localRegsInit = regsInitForHwDescription;

  // Generate the XML content
  if (file->is_open()) {
    (*file) << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    (*file) << "<registerfile>" << std::endl;
    (*file) << "  <name>RegisterFile</name>" << std::endl;
    (*file) << "  <description>Initial status of the register file of the "
               "processor</description>"
            << std::endl;
    (*file) << "  <registers>" << std::endl;

    // Create individual register elements
    for (int i = 0; i < 32; i++) {
      if (regsInitForHwDescription[i] != 0) {
        (*file) << "    <register>" << std::endl;
        (*file) << "      <name>x" << std::dec << i << "</name>" << std::endl;
        (*file) << "      <alias>" << getAliasRegName(i) << "</alias>"
                << std::endl;
        (*file) << "      <value>0x" << std::hex << regsInitForHwDescription[i]
                << "</value>" << std::endl;
        (*file) << "    </register>" << std::endl;
      }
    }

    (*file) << "  </registers>" << std::endl;
    (*file) << "</register_file>" << std::endl;

    // Close the file
    (*file).close();
  } else {
    std::cerr << "Ripes couldn't open registerfile.xml" << std::endl;
  }
}

std::shared_ptr<std::ofstream> createMemoryMapFile() {
  // Create the memorymap.xml file
  auto file = std::make_shared<std::ofstream>(
      selectedDirectory + "/" + folderName + "/memorymap.xml", std::ios::out);

  // Write a presentation comment in the file
  if (file->is_open()) {
    (*file) << "<<!--This file describes the memory map -->" << std::endl;
    // file.close();
    std::cout << "memorymap.xml created successfully." << std::endl;
  } else {
    std::cerr << "Ripes couldn't create memorymap.xml" << std::endl;
  }
  return file;
}

void writeMemoryMap(std::shared_ptr<std::ofstream> file) {
  const Ripes::MemoryMap *myMemMap = &Ripes::IOManager::get().memoryMap();

  auto activePeripherals = Ripes::IOManager::get().getPeripherals();

  // Generate the XML content
  if (file->is_open()) {
    (*file) << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    (*file) << "<memorymap>" << std::endl;
    (*file) << "  <name>MemoryMap</name>" << std::endl;
    (*file) << "  <description>Memory map with name of memory section, start "
               "address, size, type of access"
            << std::endl;
    (*file) << "  <memory_regions>" << std::endl;

    // Create individual memory region elements
    for (const auto &entry : *myMemMap) {
      (*file) << "    <region>" << std::endl;
      (*file) << "      <name>" << entry.second.name.toStdString() << "</name>"
              << std::endl;
      (*file) << "      <start_address>x" << std::hex << entry.second.startAddr
              << "</start_address>" << std::endl;
      (*file) << "      <size>" << std::dec << entry.second.size << "</size>"
              << std::endl;
      (*file) << "      <access>"
              << "ACCESS"
              << "</access>" << std::endl;
      if (entry.second.name.toStdString() == "led") {
        (*file) << "      <height>" << std::dec << "PARAM"
                << "</height>" << std::endl;
        (*file) << "      <width>" << std::dec << "PARAM"
                << "</width>" << std::endl;
        (*file) << "      <LEDsize>" << std::dec << "PARAM"
                << "</LEDsize>" << std::endl;
      }
      (*file) << "    </region>" << std::endl;
    }

    (*file) << "  </memory_regions>" << std::endl;
    (*file) << "</memorymap>" << std::endl;

    // Close the file
    (*file).close();
  } else {
    std::cerr << "Ripes couldn't open memorymap.xml" << std::endl;
  }
}

void writeNbPeriph(std::shared_ptr<std::ofstream> file) {
  nbPeripheralsStruct counters;
  counters.ledArraysCounter = 0;
  counters.switchesCounter = 0;
  counters.dpadsCounter = 0;
  bool nbLedsAlreadyWritten = false;
  bool nbSwitchesAlreadyWritten = false;
  bool nbDpadsAlreadyWritten = false;
  auto activePeripherals = Ripes::IOManager::get().getPeripherals();
  for (const Ripes::IOBase *peripheral : activePeripherals) {
    if (peripheral->baseName() == "LED Matrix") {
      counters.ledArraysCounter++;
    }
    if (peripheral->baseName() == "Switches") {
      counters.switchesCounter++;
    }
    if (peripheral->baseName() == "D-Pad") {
      counters.dpadsCounter++;
    }
    /*qDebug() << peripheral->name();
    qDebug() << peripheral->baseName();
    qDebug() << peripheral->getm_id();
    auto params = peripheral->getParams();
    qDebug() << "Just for breakpoint";*/
  }
  if (file->is_open()) {
    (*file) << "`define " << std::left << std::setw(15) << "NB_LED_MATRICES "
            << counters.ledArraysCounter << std::right << std::setw(35)
            << " //Maximum value: " << std::endl;
    std::cout
        << "Number of active LED matrices successfully written in params.vh."
        << std::endl;
    (*file) << "`define " << std::left << std::setw(15) << "NB_SWITCH_SETS "
            << counters.switchesCounter << std::right << std::setw(35)
            << " //Maximum value: " << std::endl;
    std::cout
        << "Number of active switch sets successfully written in params.vh."
        << std::endl;
    nbSwitchesAlreadyWritten = true;
    (*file) << "`define " << std::left << std::setw(15) << "NB_DPADS "
            << counters.dpadsCounter << std::right << std::setw(35)
            << " //Maximum value: " << std::endl;
    std::cout << "Number of active D-Pads successfully written in params.vh."
              << std::endl;
    nbDpadsAlreadyWritten = true;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the number of active "
                 "peripherals"
              << std::endl;
  }
}

//@brief getNbStages
// This function is called by the writeNbStages() function. It returns the
// number of stages of the processor as a QString.
//
// @param Ripes::ProcessorID with the ID of the processor selected by the user
// @return QString with the number of stages of the processor
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

// @brief getWidth
// This function is called by the writeWidth() function. It returns the width of
// data and addresses of the processor as a QString.
//
// @param Ripes::ProcessorID with the ID of the processor selected by the user
// @return QString with the width of data and addresses of the processor
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
    return "32";
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

// @brief getFw
// This function is called by the writeFwHz() function. It returns the value of
// the forwarding path parameter as a QString.
//
// @param Ripes::ProcessorID with the ID of the processor selected by the user
// @return QString with the value of the forwarding path parameter
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

// @brief getHazard
// This function is called by the writeFwHz() function. It returns the value of
// the hazard detection unit parameter as a QString.
//
// @param Ripes::ProcessorID with the ID of the processor selected by the user
// @return QString with the value of the hazard detection unit parameter
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

// @brief processorIDToQString
// This function is called by the getProcessorType() function. It transforms the
// processor ID from the Ripes::ProcessorID type to a QString.
//
// @param Ripes::ProcessorID with the ID of the processor selected by the user
// @return QString with the processor ID
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

// @brief getISAExtension
// This function is called by the writeISAExtension() function. It transforms
// the ISA name into the ISA extension parameter.
//
// @param QString with the name of the ISA implemented by the processor
// @return QString with the ISA extension parameter
std::string getISAExtension(QString ISAname) {
  if (ISAname == "RV32I" || ISAname == "RV64I") {
    return "00";
  } else if (ISAname == "RV32IM" || ISAname == "RV64IM") {
    return "10";
  } else if (ISAname == "RV32IC" || ISAname == "RV64IC") {
    return "01";
  } else if (ISAname == "RV32IMC" || ISAname == "RV64IMC") {
    return "11";
  } else {
    std::cerr << "Non-supported RISC-V extension: " << ISAname.toStdString()
              << ". Supported extensions: Integer multiplication and division "
                 "(M) and Compressed Instructions (C)"
              << std::endl;
    return "err";
  }
}

// @brief getAliasRegName
// This function is called by the writeRegsInitialValues() function. It returns
// the alias name of the register i as a std::string.
//
// @param int with the number of the register
// @return std::string with the alias name of the register
std::string getAliasRegName(int i) {
  const auto &isa = Ripes::ProcessorHandler::currentISA();
  QString QaliasName = isa->regAlias(i);
  std::string aliasName = QaliasName.toStdString();
  return aliasName;
}

QString createDialogFolderName(QInputDialog &inputDialog) {
  inputDialog.setWindowTitle("Name of the folder");
  inputDialog.setLabelText("Choose a name for the folder.");
  inputDialog.setTextValue(QcurrentID);
  int result = inputDialog.exec();
  QString localQfolderName;

  // If the user clicks on "OK", get the name
  if (result == QDialog::Accepted) {
    localQfolderName = inputDialog.textValue();
  }
  // If the user clicks on "Cancel" or "X" or leaves the box empty and then
  // clicks on "OK", it means that no folder name was chosen.
  if (result == QDialog::Rejected ||
      localQfolderName == "") { // case of "" + yes
    std::cerr << "No folder name chosen." << std::endl;
    localQfolderName = "";
  }
  return localQfolderName;
}

int createWarningBox(QMessageBox &msgBox, QString localQfolderName) {
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setText("The folder" + localQfolderName +
                 "already exists. If you continue, its files will be "
                 "overwritten.\nDo you want to overwrite the existing files?");
  msgBox.addButton(QMessageBox::Yes);
  msgBox.addButton(QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Cancel);
  return msgBox.exec();
}
