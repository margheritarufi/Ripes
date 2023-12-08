//@file hwdescription.cpp
//@author Margherita Rufi
//@version 1.2 2023-12-08
//@brief This file contains the functions that create the verilog files that
// describe the processor hardware.

#include "hwdescription.h"
#include "hwdescriptioncache.h"
#include "hwdescriptionioperiph.h"
#include "hwdescriptionmmap.h"
#include "hwdescriptionregfile.h"
#include "hwdescriptionstaticfiles.h"
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
#include <memory>

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
std::string paramsFileName = "ripes_params.vh";

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
  thisID = ID;
  QcurrentID = getProcessorType();
  currentID = QcurrentID.toStdString();

  // Open a dialog window to let the user choose a directory
  QString QselectedDirectory = openDirectoryDialog();

  // Create folder if the user has chosen a valid path
  if (!QselectedDirectory.isEmpty()) {
    QString QfolderName = createFolder(QselectedDirectory);

    selectedDirectory = QselectedDirectory.toStdString();
    folderName = QfolderName.toStdString();

    // Create files if the use has chosen a valid name
    if (folderName != "") {
      // Create and write params.vh
      paramsFile = createParamsFile();
      writeProcessorType(paramsFile);
      writeNbStages(paramsFile);
      writeWidth(paramsFile);
      writeFwHz(paramsFile);
      writeISAExtension(paramsFile);
      writeNbPeriph(paramsFile);
      writePeriphParams(paramsFile);
      writeCacheSettings(paramsFile);
      paramsFile->close();

      // Create and write regsfile.xml
      regsFile = createRegsFile();
      writeRegsInitialValues(regsFile);
      regsFile->close();

      // Create and write memorymap.xml
      memoryMapFile = createMemoryMapFile();
      writeMemoryMap(memoryMapFile);
      memoryMapFile->close();

      downloadStaticFiles();
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
      nullptr, "Select the directory where the folder will be created",
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
      // a new dialog window to choose the folder name. If the user clicks on
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
std::shared_ptr<std::ofstream> createParamsFile() {
  // Create the params.vh file
  auto file = std::make_shared<std::ofstream>(
      selectedDirectory + "/" + folderName + "/ripes_params.vh", std::ios::out);

  // Write a presentation comment in the file
  if (file->is_open()) {
    (*file) << "//These are automatically-generated parameters."
            << "\n"
            << "//The designer cannot change them because they depend on the "
               "user's choice of the processor"
            << std::endl;
    // file.close();
    std::cout << "ripes_params.vh created successfully." << std::endl;
  } else {
    std::cerr << "Ripes couldn't open ripes_params.vh" << std::endl;
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
    (*file) << "`define " << std::left << std::setw(23) << "PROC_TYPE "
            << currentID << std::endl;
    sendOutputStream("Processor ID", paramsFileName);
  } else {
    sendErrorStream("Processor ID", paramsFileName);
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
    printVerilogDefine(file, "NB_STAGES", getNbStages(thisID),
                       "//Possible values: 1, 5, 6");
    sendOutputStream("Number of stages", paramsFileName);
  } else {
    sendErrorStream("Number of stages", paramsFileName);
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
    printVerilogDefine(file, "DATA_WIDTH", getWidth(thisID),
                       "//Possible values: 32, 64. The error value is 0.");
    printVerilogDefine(file, "ADDR_WIDTH", getWidth(thisID),
                       "//Possible values: 32, 64. The error value is 0.");
    sendOutputStream("Width of data and addresses", paramsFileName);
  } else {
    sendErrorStream("Width of data and addresses", paramsFileName);
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
    printVerilogDefine(file, "FW_PATH", getFw(thisID),
                       "//Possible values: 1/0 : yes/no. Error value: -1");
    printVerilogDefine(file, "HZ_DETECT", getHazard(thisID),
                       "//Possible values: 1/0 : yes/no. Error value: -1");
    sendOutputStream("Forwarding path and hazard detection unit parameters",
                     paramsFileName);
  } else {
    sendErrorStream("Forwarding path and hazard detection unit parameters",
                    paramsFileName);
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
    (*file) << "`define " << std::left << std::setw(23) << "EXTENSION "
            << exstensionCode << std::right << std::setw(70)
            << " //Possible values: 00, 01, 10, 11 (MC extensions: 1=enabled, "
               "0=disabled)"
            << std::endl;
    sendOutputStream("ISA extension code", paramsFileName);
  } else {
    sendErrorStream("ISA extension code", paramsFileName);
  }
}

//@brief getNbStages
// This function is called by the writeNbStages() function. It returns the
// number of stages of the processor as an int.
//
// @param Ripes::ProcessorID with the ID of the processor selected by the user
// @return int with the number of stages of the processor
int getNbStages(Ripes::ProcessorID ID) {
  switch (ID) {
  case Ripes::RV32_SS:
    return 1;
  case Ripes::RV32_5S_NO_FW_HZ:
    return 5;
  case Ripes::RV32_5S_NO_HZ:
    return 5;
  case Ripes::RV32_5S_NO_FW:
    return 5;
  case Ripes::RV32_5S:
    return 5;
  case Ripes::RV32_6S_DUAL:
    return 6;
  case Ripes::RV64_SS:
    return 1;
  case Ripes::RV64_5S_NO_FW_HZ:
    return 5;
  case Ripes::RV64_5S_NO_HZ:
    return 5;
  case Ripes::RV64_5S_NO_FW:
    return 5;
  case Ripes::RV64_5S:
    return 5;
  case Ripes::RV64_6S_DUAL:
    return 6;
  case Ripes::NUM_PROCESSORS:
    return 0;
  default:
    return 0;
  }
}

// @brief getWidth
// This function is called by the writeWidth() function. It returns the width of
// data and addresses of the processor as an int.
//
// @param Ripes::ProcessorID with the ID of the processor selected by the user
// @return int with the width of data and addresses of the processor
int getWidth(Ripes::ProcessorID ID) {
  switch (ID) {
  case Ripes::RV32_SS:
    return 32;
  case Ripes::RV32_5S_NO_FW_HZ:
    return 32;
  case Ripes::RV32_5S_NO_HZ:
    return 32;
  case Ripes::RV32_5S_NO_FW:
    return 32;
  case Ripes::RV32_5S:
    return 32;
  case Ripes::RV32_6S_DUAL:
    return 32;
  case Ripes::RV64_SS:
    return 64;
  case Ripes::RV64_5S_NO_FW_HZ:
    return 64;
  case Ripes::RV64_5S_NO_HZ:
    return 64;
  case Ripes::RV64_5S_NO_FW:
    return 64;
  case Ripes::RV64_5S:
    return 64;
  case Ripes::RV64_6S_DUAL:
    return 64;
  case Ripes::NUM_PROCESSORS:
    return -1;
  default:
    return -1;
  }
}

// @brief getFw
// This function is called by the writeFwHz() function. It returns the value of
// the forwarding path parameter as an int.
//
// @param Ripes::ProcessorID with the ID of the processor selected by the user
// @return int with the value of the forwarding path parameter
int getFw(Ripes::ProcessorID ID) {
  switch (ID) {
  case Ripes::RV32_5S_NO_HZ:
    return 1;
  case Ripes::RV32_5S:
    return 1;
  case Ripes::RV32_6S_DUAL:
    return 1;
  case Ripes::RV64_5S_NO_HZ:
    return 1;
  case Ripes::RV64_5S:
    return 1;
  case Ripes::RV64_6S_DUAL:
    return 1;
  case Ripes::NUM_PROCESSORS:
    return -1;
  default:
    return 0;
  }
}

// @brief getHazard
// This function is called by the writeFwHz() function. It returns the value of
// the hazard detection unit parameter as an int.
//
// @param Ripes::ProcessorID with the ID of the processor selected by the user
// @return int with the value of the hazard detection unit parameter
int getHazard(Ripes::ProcessorID ID) {
  switch (ID) {
  case Ripes::RV32_5S_NO_FW:
    return 1;
  case Ripes::RV32_5S:
    return 1;
  case Ripes::RV32_6S_DUAL:
    return 1;
  case Ripes::RV64_5S_NO_FW:
    return 1;
  case Ripes::RV64_5S:
    return 1;
  case Ripes::RV64_6S_DUAL:
    return 1;
  case Ripes::NUM_PROCESSORS:
    return -1;
  default:
    return 0;
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

// #brief createDialogFolderName
//  This function is called by the createFolder() function. It creates a dialog
//  window to choose the name of the folder.
//
//  @param QInputDialog with the dialog window
//  @return QString with the name of the folder
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

// @brief createWarningBox
// This function is called by the createFolder() function. It creates a warning
// window to ask the user if he/she wants to overwrite the existing folder.
//
// @param QMessageBox with the warning window
// @param QString with the name of the folder
// @return int with the result of the warning window
int createWarningBox(QMessageBox &msgBox, QString localQfolderName) {
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setText("The folder " + localQfolderName +
                 " already exists. If you continue, its files will be "
                 "overwritten.\nDo you want to overwrite the existing files?");
  msgBox.addButton(QMessageBox::Yes);
  msgBox.addButton(QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Cancel);
  return msgBox.exec();
}

// @brief printVerilogDefine
// This function is called by all the functions that write a parameter in the
// params.vh file. It writes the parameter in the file following the Verilog
// syntax and a regular format.
//
// @param std::shared_ptr<std::ofstream> with the pointer to the created
// "params.vh" file
// @param std::string with the name of the parameter
// @param int with the value of the parameter
// @param std::string with the comment of the parameter
// @return void
void printVerilogDefine(std::shared_ptr<std::ofstream> file, std::string name,
                        int value, std::string comment) {
  (*file) << "`define " << std::left << std::setw(23) << name << value
          << std::right << std::setw(70) << comment << std::endl;
}

// @brief sendOutputStream
// This function sends an output stream to inform that something was
// successfully written inside the file 'fileName'
//
// @param std::string with the name of the object that was written in the file
// @param std::string with the name of the file
void sendOutputStream(std::string objectName, std::string fileName) {
  std::cout << objectName << " successfully written in " << fileName << "."
            << std::endl;
}

// @brief sendErrorStream
// This function sends an error stream to inform that Ripes couldn't open a file
// to write some data.
//
// @param std::string with the name of the object that couldn't be written in
// the file
// @param std::string with the name of the file
void sendErrorStream(std::string objectName, std::string fileName) {
  std::cerr << "Ripes couldn't open " << fileName << " to write " << objectName
            << "." << std::endl;
}
