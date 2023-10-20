#include "processorhandler.h"
#include "processorregistry.h"
#include <QDebug>

namespace Ripes {

void collectParams() {
  const Ripes ::ProcessorID &ID =
      ProcessorHandler::getID(); // Now ID contains the constant reference to
                                 // the object of type ProcessorID
  Ripes ::ProcessorID idValue =
      ID; // Now idValue contains the value of the object
  qDebug() << "Current Processor ID:" << idValue;

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
