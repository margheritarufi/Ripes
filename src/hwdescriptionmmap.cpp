//@file hwdescriptionmmap.cpp
//@author Margherita Rufi
//@version 1.0 2023-11-26
//@brief This file contains the functions used to write the memory map in the
// memorymap.xml file.

#include "hwdescriptionmmap.h"
#include "hwdescription.h"
#include "hwdescriptioncache.h"
#include "hwdescriptionioperiph.h"
#include "io/iomanager.h"
#include "ripes_types.h"
#include <QDebug>
#include <iomanip>
#include <memory>

std::string memoryMapFileName = "memorymap.xml";

// @brief createMemoryMapFile
// This function creates the memorymap.xml file in the selected directory.
//
// @param void
// @return std::shared_ptr<std::ofstream> with the pointer to the created
// memorymap.xml file
std::shared_ptr<std::ofstream> createMemoryMapFile() {
  // Create the memorymap.xml file
  auto file = std::make_shared<std::ofstream>(
      selectedDirectory + "/" + folderName + "/memorymap.xml", std::ios::out);

  // Write a presentation comment in the file
  if (file->is_open()) {
    (*file) << "<<!--This file describes the memory map -->" << std::endl;
    std::cout << "memorymap.xml created successfully." << std::endl;
  } else {
    std::cerr << "Ripes couldn't create memorymap.xml" << std::endl;
  }
  return file;
}

// @brief writeMemoryMap
// This function writes the memory map in memorymap.xml.
//
// @param file is a pointer to the file where the memory map will be written.
// @return void
void writeMemoryMap(std::shared_ptr<std::ofstream> file) {
  const Ripes::MemoryMap *myMemMap = &Ripes::IOManager::get().memoryMap();

  //auto activePeripherals = Ripes::IOManager::get().getPeripherals();

  // Generate the XML content
  if (file->is_open()) {
    (*file) << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    (*file) << "<memorymap>" << std::endl;
    (*file) << "  <name>MemoryMap</name>" << std::endl;
    (*file) << "  <description>Memory map with name of memory section, start "
               "address and size in Bytes"
            << std::endl;
    (*file) << "  <memory_regions>" << std::endl;

    //Main memory region
    (*file) << "    <region>" << std::endl;
    (*file) << "      <name>Main Memory</name>" << std::endl;
    (*file) << "      <start_address>x00000000</start_address>" << std::endl;
    (*file) << "      <size>4026531840</size>"
            << std::endl;
    (*file) << "    </region>" << std::endl;

    // Create individual memory region elements
    for (const auto &entry : *myMemMap) {
      if (entry.second.name != ".text" && entry.second.name != ".data" &&
          entry.second.name != ".bss") {
        (*file) << "    <region>" << std::endl;
        (*file) << "      <name>" << entry.second.name.toStdString()
                << "</name>" << std::endl;
        (*file) << "      <start_address>x" << std::hex
                << entry.second.startAddr << "</start_address>" << std::endl;
        (*file) << "      <size>" << std::dec << entry.second.size << "</size>"
                << std::endl;
        (*file) << "    </region>" << std::endl;
      }
    }
    (*file) << "  </memory_regions>" << std::endl;
    (*file) << "</memorymap>" << std::endl;

    sendOutputStream("Memory map", memoryMapFileName);
  } else {
    sendErrorStream("Memory map", memoryMapFileName);
  }
}
