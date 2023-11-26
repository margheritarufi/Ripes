//@file hwdescriptionregfile.cpp
//@author Margherita Rufi
//@version 1.0 2023-11-26
//@brief This file contains the functions used to write the initial values of
// the
// registers in the registerfile.xml file.

#include "hwdescriptionregfile.h"
#include "hwdescription.h"
#include "hwdescriptioncache.h"
#include "hwdescriptionioperiph.h"
#include "hwdescriptionmmap.h"
#include "processorhandler.h"
#include "processorregistry.h"
#include "ripes_types.h"
#include <QDebug>
#include <iomanip>
#include <memory>

std::string registerFileName = "registerfile.xml";

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
    std::cout << registerFileName << " created successfully." << std::endl;
  } else {
    std::cerr << "Ripes couldn't create " << registerFileName << std::endl;
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
    sendOutputStream("Initial values of registers", registerFileName);
  } else {
    sendErrorStream("initial values of registers", registerFileName);
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
