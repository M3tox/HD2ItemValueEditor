#include "Items.h"

Items::Items(const char* path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    // The program does not support all .sav files and should therefore check
    fileSize = file.tellg();
    if (fileSize % 504) {
        file.close();
        return;
    }

    // seems fine and back to begin
    good = true;
    file.seekg(0, std::ios::beg);

    const uint32_t maxElements = fileSize / 504;
    items.reserve(maxElements);
    while (file.good() && items.size() < maxElements) {
        int32_t buffer;
        file.read((char*) & buffer, sizeof(buffer));
        if (buffer) {
            // Item OK
            file.read((char*)&buffer, sizeof(buffer));
            items.emplace_back(Item(buffer));
            file.read((char*)&items.back().items, sizeof(items.back().items));
            continue;
        }
        // Item EMPTY
        items.emplace_back(Item(EMPTY));
        // this just "emulates" the debug symbols that are used in empty data fields
        memset(&items.back().items, 0xCD ,sizeof(items.back().items));
        memcpy(items.back().items.labelName, "EMPTY SLOT", 11);
    }
    file.close();
}

Items::Items(uint8_t* data, uint32_t dataSize) {

    if (fileSize % 504) {
        return;
    }

    fileSize = dataSize;
    good = true;

    int32_t* curr = (int32_t*)data;

    const uint32_t maxElements = fileSize / 504;
    items.reserve(maxElements);
    while (items.size() < maxElements) {

        if (*curr++) {
            // Item OK
            items.emplace_back(Item(*curr++));
            memcpy(&items.back().items, curr, sizeof(items.back().items));
            curr += 125;
            continue;
        }

        // Item EMPTY
        items.emplace_back(Item(EMPTY));
        items.back().items;
        memcpy(items.back().items.labelName, "EMPTY SLOT", 11);
    }
}

Items::~Items() {
    
}


void Items::writeCSV(const char* fileName)
{
    std::ofstream file(fileName, std::ios::binary, std::ios::trunc);

    file << "\"Item ID\",\"Record type\",\"";
    for (uint32_t i2 = 0; i2 < items.at(0).MAXfields; i2++) {
        std::string descr, val;
        items.at(0).getFieldInformation(i2, descr, val);
        file << descr << "\",\"";
    }
    file << "\"\r\n";
    
    static const std::string str[4]{ "AMMO","ANIMATED","STATIC","EMPTY" };
    for (uint32_t i = 0; i < items.size(); i++) {
        if (items.at(i).recordType == EMPTY)
            continue;
        file << "\""  << i << "\",\"";
        file << str[items.at(i).recordType];
        file << "\",\"";
        for (uint32_t i2 = 0; i2 < items.at(i).MAXfields; i2++) {
            std::string val;
            items.at(i).getFieldInformation(i2, val);
            file << val << "\",\"";
        }
        file << "\"\r\n";
    }
        
    file.close();
}

void Items::writeBinaryFile(const char* fileName)
{
    uint8_t* fileRef = new uint8_t[fileSize];
    uint32_t* curr = (uint32_t*)fileRef;
    for (auto& i : items) {
        if (i.recordType == EMPTY) {
            *curr++ = 0;
            continue;
        }
        *curr++ = 1;
        *curr++ = i.recordType;
        memcpy(curr, &i.items, sizeof(i.items));
        curr += 125;
    }


    std::ofstream file(fileName, std::ios::binary, std::ios::trunc);
    file.write((char*)fileRef, fileSize);
    file.close();

    delete[] fileRef;
}
