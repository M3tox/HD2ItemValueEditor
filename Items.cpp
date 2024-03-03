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

    maxElements = fileSize / 504;
    TotalSpace = new uint8_t[fileSize];
    uint32_t* curr = (uint32_t*)TotalSpace;

    for (uint32_t i = 0; (i < maxElements) && file.good(); i++) {
        //int32_t buffer;
        file.read((char*)curr, 4);
        if (*curr++) {
            // Item OK
            items[i].recordType = (int32_t*)curr++;
            file.read((char*)items[i].recordType, sizeof(items[i].recordType));
            
            items[i].itemData = (ItemBase*)curr;
            file.read((char*)items[i].itemData, 500);
            curr += 125;
            continue;
        }
        // Item EMPTY
        items[i].recordType = (int32_t*)(curr-1);
        
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

}

Items::~Items() {
    delete TotalSpace;
}


void Items::writeCSV(const char* fileName)
{
    std::ofstream file(fileName, std::ios::binary, std::ios::trunc);

    file.close();
}

void Items::writeBinaryFile(const char* fileName)
{

    std::ofstream file(fileName, std::ios::binary, std::ios::trunc);
    file.write((char*)TotalSpace, fileSize);
    file.close();
}
