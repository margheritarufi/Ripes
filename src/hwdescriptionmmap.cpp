#include "hwdescription.h"
#include "hwdescriptioncache.h"
#include "hwdescriptionioperiph.h"
#include "hwdescriptionmmap.h"
#include "io/iomanager.h"
#include "ripes_types.h"
#include <QDebug>
#include <iomanip>

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
