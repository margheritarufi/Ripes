//@file hwdescription.cpp
//@author Margherita Rufi
//@version 1.0 2023-11-13
//@brief This file contains the functions used to copy the files from the
//resources
// folder to the selected directory.

#include "hwdescription.h"
#include <QCoreApplication>
#include <QFile>
#include <QResource>
#include <filesystem>
#include <fstream>
#include <iostream>

std::string staticFilesFolderName = "hw";

// @brief downloadStaticFiles
// This function copies the static files from the resources folder to the
// selected directory. This function is called by the downloadFiles() function
// in the hwdescription.cpp file.
//
// @param void
// @return void
void downloadStaticFiles() {
  QString resourcePath;

  // Pick the right static hw description resource folder
  if (thisID != Ripes::RV32_SS && thisID != Ripes::RV64_SS) {
    resourcePath = ":/multiplestageshw";
  } else {
    resourcePath = ":/singlestagehw";
  }

  QString destinationPath = QString::fromStdString(
      selectedDirectory + "/" + folderName + "/" + staticFilesFolderName);

  QDir sourceDirectory(resourcePath);
  QDir destinationDirectory(destinationPath);

  // Make sure the source directory exists
  if (!sourceDirectory.exists()) {
    std::cerr << "Source directory does not exist: "
              << ":/hwdescriptionstatic" << std::endl;
    return;
  }

  if (!destinationDirectory.exists()) {
    destinationDirectory.mkpath(".");
  } else {
    // Remove the existing folder and create a new one
    if (destinationDirectory.removeRecursively()) {
      destinationDirectory.mkpath(".");
    } else {
      std::cerr
          << "Error removing existing folder to substitute with the new one:"
          << selectedDirectory + "/" + folderName + "/" + staticFilesFolderName;
      return;
    }
  }

  // Iterate over the files in the source directory
  QStringList files = sourceDirectory.entryList(QDir::Files);
  for (const QString &file : files) {
    // Construct full paths for source and destination files
    QString sourceFilePath = sourceDirectory.filePath(file);
    QString destinationFilePath = destinationDirectory.filePath(file);

    // Copy the file
    if (QFile::copy(sourceFilePath, destinationFilePath)) {
      std::cout << "File copied successfully:"
                << destinationFilePath.toStdString() << std::endl;
    } else {
      std::cerr << "Error copying file:" << sourceFilePath.toStdString()
                << std::endl;
    }
  }
}
