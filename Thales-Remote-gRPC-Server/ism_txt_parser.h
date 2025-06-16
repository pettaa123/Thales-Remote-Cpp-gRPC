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

    bool headerSkipped = false;

    while (std::getline(file, line)) {
        if (!headerSkipped) {
            headerSkipped = true;
            continue;
        }

        std::istringstream iss(line);
        DataEntry entry;

        if (iss >> entry.number >> entry.frequency >> entry.impedance >> entry.phase) {
            data.push_back(entry);
        }
    }

    return data;
}

std::string writeToCSV(const std::string& filename, const std::vector<DataEntry>& data) {
    std::string csvFilename = filename + ".csv";
    std::ofstream outFile(csvFilename);
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
            std::string csvFilename = writeToCSV(entry.path().stem().string(), fileData);
            csvFiles.push_back(csvFilename);
        }
    }

    return csvFiles;
}


