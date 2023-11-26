//@file hwdescriptionioperiph.cpp
//@author Margherita Rufi
//@version 1.0 2023-11-26
//@brief This file contains the functions used to write the parameters of the
// peripherals in the ripes_params.vh file.

#include "hwdescriptionioperiph.h"
#include "hwdescription.h"
#include "hwdescriptioncache.h"
#include "io/iomanager.h"
#include "ioledmatrix.h"
#include <QDebug>
#include <iomanip>

// @brief writeNbPeriph
// This function writes the number of active instantiations of each peripheral
// in ripes_params.vh. Three types of peripherals are consupported: LED
// matrices, switches and D-Pads.
//
// @param file is a pointer to the file where the number of active peripherals
// will be written.
// @return void
void writeNbPeriph(std::shared_ptr<std::ofstream> file) {
  // Create a struct to count the number of active instantiations of each
  // peripheral
  nbPeripheralsStruct counters;
  counters.ledArraysCounter = 0;
  counters.switchesCounter = 0;
  counters.dpadsCounter = 0;

  // Get the list of all active peripherals
  auto activePeripherals = Ripes::IOManager::get().getPeripherals();

  // For each active peripheral, check its base name and increment the
  // corresponding counter
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

  // Write the number of active instantiations of each peripheral in
  // ripes_params.vh
  if (file->is_open()) {
    (*file) << "\n" << std::endl;
    printVerilogDefine(file, "NB_LED_MATRICES ", counters.ledArraysCounter,
                       " //Maximum value: ");
    printVerilogDefine(file, "NB_SWITCH_SETS ", counters.switchesCounter,
                       " //Maximum value: ");
    printVerilogDefine(file, "NB_DPADS ", counters.dpadsCounter,
                       " //Maximum value: ");
    sendOutputStream("Number of active peripherals", paramsFileName);
  } else {
    sendErrorStream("Number of active peripherals", paramsFileName);
  }
}

// @brief writePeriphParams
// This function writes the parameters of each peripheral in ripes_params.vh.
// Each peripheral has a different number and type of parameters, so the
// function is implemented in a different way for each peripheral.
//
// @param file is a pointer to the file where the parameters of the peripherals
// will be written.
// @return void
void writePeriphParams(std::shared_ptr<std::ofstream> file) {
  // Get the list of all active peripherals
  auto activePeripherals = Ripes::IOManager::get().getPeripherals();

  // For each active peripheral, check its base name and write its parameters
  // in ripes_params.vh. Do some name formatting when needed.
  if (file->is_open()) {
    for (Ripes::IOBase *peripheral : activePeripherals) { // it was const
      // LED matrices have 3 parameters: SIZE, WIDTH and HEIGHT
      if (typeid(*peripheral) == typeid(Ripes::IOLedMatrix)) {
        for (int param = 0; param < 3; param++) {
          std::string peripheralBaseName = "LED_MATRIX";
          std::string peripheralID = std::to_string(peripheral->getm_id());
          std::string peripheralParamSmall =
              peripheral->getParams()[param].name.toStdString();

          // Transform the param name in capital letters and replace spaces with
          // underscores
          std::string peripheralParamCapital;
          std::transform(peripheralParamSmall.begin(),
                         peripheralParamSmall.end(),
                         std::back_inserter(peripheralParamCapital), ::toupper);
          std::replace(peripheralParamCapital.begin(),
                       peripheralParamCapital.end(), ' ', '_');

          std::string completePeripheralParam = peripheralBaseName + "_" +
                                                peripheralID + "_" +
                                                peripheralParamCapital;
          int paramValue = peripheral->getParams()[param].value.toInt();
          printVerilogDefine(file, completePeripheralParam, paramValue);
        }
      }

      // D-Pads have 1 parameter: N
      if (typeid(*peripheral) == typeid(Ripes::IOSwitches)) {
        std::string peripheralBaseName = "SWITCHES";
        std::string peripheralID = std::to_string(peripheral->getm_id());
        std::string peripheralParam = "N";
        std::string completePeripheralParam =
            peripheralBaseName + "_" + peripheralID + "_" + peripheralParam;
        int paramValue = peripheral->getParams()[0].value.toInt();
        printVerilogDefine(file, completePeripheralParam, paramValue);
      }

      /*auto symbols =
       * Ripes::IOManager::get().assemblerSymbolsForPeriph(peripheral);*/
    }
    sendOutputStream("Parameters of all peripherals", paramsFileName);
  } else {
    sendErrorStream("Parameters of all peripherals", paramsFileName);
  }
}
