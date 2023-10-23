#include "processorhandler.h"
#include "processorregistry.h"
#include <QDebug>
#include <iostream>
#include <fstream>

namespace Ripes {

void collectParams() {
  const Ripes ::ProcessorID &ID =
      ProcessorHandler::getID(); // Now ID contains the constant reference to
                                 // the object of type ProcessorID
  Ripes ::ProcessorID idValue =
      ID; // Now idValue contains the value of the object
  qDebug() << "Current Processor ID:" << idValue;

  // Name of the file to be crated or overwritten
  std::string nomeFile = "params.vh";
  std::ofstream file(nomeFile);

  if (file.is_open()) {
    file << "//These are automatically-generated parameters. The designer cannot change them because they depend on th user's choice of the processor" << std::endl;

    file.close();

    std::cout << "File 'params.vh' creato o sovrascritto con successo." << std::endl;
    } else {
    std::cerr << "Impossibile aprire il file 'params.vh'." << std::endl;
    }

  /*switch(ID){
  case RV32_SS:

  case RV32_5S_NO_FW_HZ:

  case RV32_5S_NO_HZ:

  case RV32_5S_NO_FW:

  case RV32_5S:

  case RV32_6S_DUAL:

  case RV64_SS:

  case RV64_5S_NO_FW_HZ:

  case RV64_5S_NO_HZ:

  case RV64_5S_NO_FW:

  case RV64_5S:

  case RV64_6S_DUAL:

  }*/
}

/*Inspirations:
//Returns the ID of the currently instantiated processor. (from
processorhandler.h) static const ProcessorID &getID() { return get()->_getID();
} auto &desc = ProcessorRegistry::getDescription(ProcessorHandler::getID());
QString status = "Processor: " + desc.name + "    ISA: " +
ProcessorHandler::getProcessor()->implementsISA()->name();
*/

} // namespace Ripes
