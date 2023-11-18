#include "hwdescription.h"
#include "hwdescriptioncache.h"
#include "hwdescriptionioperiph.h"
#include "io/iomanager.h"
#include "ioledmatrix.h"
#include <QDebug>
#include <iomanip>

void writeNbPeriph(std::shared_ptr<std::ofstream> file) {
  nbPeripheralsStruct counters;
  counters.ledArraysCounter = 0;
  counters.switchesCounter = 0;
  counters.dpadsCounter = 0;
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
  }
  if (file->is_open()) {
    printVerilogDefine(file, "NB_LED_MATRICES ", counters.ledArraysCounter, " //Maximum value: ");
    std::cout
        << "Number of active LED matrices successfully written in params.vh."
        << std::endl;
    printVerilogDefine(file, "NB_SWITCH_SETS ", counters.switchesCounter, " //Maximum value: ");
    std::cout
        << "Number of active switch sets successfully written in params.vh."
        << std::endl;
    printVerilogDefine(file, "NB_DPADS ", counters.dpadsCounter, " //Maximum value: ");
    std::cout << "Number of active D-Pads successfully written in params.vh."
              << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write the number of active "
                 "peripherals"
              << std::endl;
  }
}

void writePeriphParams(std::shared_ptr<std::ofstream> file) {
  auto activePeripherals = Ripes::IOManager::get().getPeripherals();
  if (file->is_open()) {
    for (Ripes::IOBase *peripheral : activePeripherals) { //era const
      if (typeid(*peripheral) == typeid(Ripes::IOLedMatrix)){
        for (int param = 0; param < 3; param ++ ){
          std::string peripheralBaseName = "LED_MATRIX";
          std::string peripheralID = std::to_string(peripheral->getm_id());
          std::string peripheralParamSmall = peripheral->getParams()[param].name.toStdString();
          std::string peripheralParamCapital;
          std::transform(peripheralParamSmall.begin(), peripheralParamSmall.end(), std::back_inserter(peripheralParamCapital), ::toupper);
          std::replace(peripheralParamCapital.begin(), peripheralParamCapital.end(), ' ', '_');
          std::string completePeripheralParam = peripheralBaseName + "_" + peripheralID + "_" + peripheralParamCapital;
          int paramValue = peripheral->getParams()[param].value.toInt();
          printVerilogDefine(file, completePeripheralParam, paramValue);
          std::cout
              << completePeripheralParam << " successfully written in params.vh."
              << std::endl;
        }
      }
      if (typeid(*peripheral) == typeid(Ripes::IOSwitches)){
        std::string peripheralBaseName = "SWITCHES";
        std::string peripheralID = std::to_string(peripheral->getm_id());
        std::string peripheralParam = "N";
        std::string completePeripheralParam = peripheralBaseName + "_" + peripheralID + "_" + peripheralParam;
        int paramValue = peripheral->getParams()[0].value.toInt();
        printVerilogDefine(file, completePeripheralParam, paramValue);
        std::cout
            << "Parameters of " << completePeripheralParam << " successfully written in params.vh."
            << std::endl;
      }


      /*auto symbols = Ripes::IOManager::get().assemblerSymbolsForPeriph(peripheral);
      for (const auto &symbol : symbols) {
        std::cout << "#define " << symbol.first.v.toStdString() << " 0x" << QString::number(symbol.second, 16).toStdString() << std::endl;
      }

      auto vediamocose = peripheral->getParams()[2]; //this is the true SIZE value for LEDS
      qDebug() << "just for debug";*/

    }
    std::cout
        << "Parameters of all peripherals successfully written in params.vh."
        << std::endl;
  } else {
    std::cerr << "Ripes couldn't open params.vh to write params of the "
                 "peripherals"
              << std::endl;
  }
  }



