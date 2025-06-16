#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

struct DataEntry {
    int number;
    double frequency;
    double impedance;
    double phase;
};

std::vector<DataEntry> parseFile(const std::string& filename) {
    std::vector<DataEntry> data;
    std::ifstream file(filename);
    std::string line;

    // Find header line
    while (std::getline(file, line)) {
        if (line.find("Number") != std::string::npos && line.find("Frequency/Hz") != std::string::npos) {
            break; // Found header, exit loop
        }
    }

    // Read actual data
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        DataEntry entry;

        if (iss >> entry.number >> entry.frequency >> entry.impedance >> entry.phase) {
            data.push_back(entry);
        }
    }

    return data;
}


std::string writeToCSV(const std::string& directoryPath, const std::string& filename, const std::vector<DataEntry>& data) {
    std::string csvFilename = directoryPath + "/" + filename + ".csv"; // Store CSV in directoryPath
    std::ofstream outFile(csvFilename);

    if (!outFile) {
        throw std::runtime_error("Failed to open file: " + csvFilename);
    }

    outFile << "Number,Frequency/Hz,Impedance/Ohm,Phase/deg\n";

    for (const auto& entry : data) {
        outFile << entry.number << "," << entry.frequency << "," << entry.impedance << "," << entry.phase << "\n";
    }

    return csvFilename;
}

std::vector<std::string> processDirectory(const std::string& directoryPath) {
    std::vector<std::string> csvFiles;

    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            std::vector<DataEntry> fileData = parseFile(entry.path().string());
            std::string csvFilename = writeToCSV(directoryPath,entry.path().stem().string(), fileData);
            csvFiles.push_back(csvFilename);
        }
    }

    return csvFiles;
}


