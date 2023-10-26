#pragma once

#include <fstream>
#include <vector>
#include <string>

#define STRINGSIZE 20

class Items
{
public:
    // use this if you want to read file
    Items(const char* path);
    // use this is if file is already in RAM (e.g. directly from .dta)
    Items(uint8_t* data, uint32_t dataSize);
    ~Items();

    void writeCSV(const char* fileName);
    void writeBinaryFile(const char* fileName);
    bool isGood() { return good; };

    uint32_t getItemCount() const { return items.size(); };
    uint32_t getFieldCount() const { return items.back().MAXfields; };
    inline const char* getItemName(uint32_t itemID) const { return items.at(itemID).items.labelName; };
    inline bool getFieldInformation(uint32_t itemID, uint32_t fieldID, std::string& descr, std::string& value) {
        return items.at(itemID).getFieldInformation(fieldID, descr, value);
    }
    inline bool getFieldInformation(uint32_t itemID, uint32_t fieldID, std::string& value) {
        return items.at(itemID).getFieldInformation(fieldID, value);
    }

    inline bool setFieldInformation(uint32_t itemID, const uint32_t fieldID, const std::string& value) {
        return items.at(itemID).setFieldInformation(fieldID, value);
    }
    inline bool isFieldActive(uint32_t itemID, const uint32_t fieldID) {
        return items.at(itemID).fieds[items.at(itemID).recordType][fieldID].enabled;
    }
    inline int32_t getRecordType(uint32_t itemID) {
        return items.at(itemID).recordType;
    }

    inline bool setRecordType(uint32_t itemID, int32_t recType) {
        if (recType > 3) return false;
        items.at(itemID).recordType = recType;
        return true;
    }

private:

    enum RECORD_TYPE {
        AMMO,
        ANIMATED,
        STATIC,
        EMPTY
    };

    struct Item {
        enum FIELDTYPE {
            ICHAR,
            IINT,
            IFLOAT,
            IBOOL
        };
        struct ItemRecord {
            char strModelFPV[STRINGSIZE];
            char strImage[STRINGSIZE];
            char strModel[STRINGSIZE];
            char unknownRef[STRINGSIZE];
            char labelName[STRINGSIZE];
            int32_t textRefID;
            float weight;
            int32_t inventorySlotType;
            int32_t unknown2;
            int32_t unknown3;
            int32_t unknown4;
            int32_t teamID;
            int32_t unknown6;
            int32_t unknown7;

            int32_t unknown7_5;
            float unknownF1;
            float damage;
            int32_t unknown8;
            float attackDelay;
            int32_t unknown9;
            int32_t canReload;  // probably just 1 byte used
            float unknownF3_5;
            int32_t unknown12;
            int32_t unknown13;
            float unknownF4;
            float velocity;
            float idealRange;
            float unAimedDistance;
            float aimedDistance;
            float unknownF9;
            int32_t unknown14;
            int32_t unknown15;
            char shortName[8];
            float unknownF10;

            int32_t unknownBool2;
            int32_t unknown16;
            int32_t unknown17;
            int32_t unknown18;
            int32_t unknown19;
            int32_t unknownBool3;

            int32_t unknownInts1[13];
            int32_t WeaponCarryMode;
            int32_t WeaponBehaive;
            int32_t Scope;
            int32_t AmmoID;

            int32_t unknownInts2[47];
        };

        struct FieldDefs {
            char descr[20];
            uint16_t dataType;
            uint32_t position;
            bool enabled;
        };

        int32_t recordType;

        ItemRecord items;
        // 104 fields in total, last 41 not used (always uninitialized)
        static const uint32_t MAXfields = 104-41;
        FieldDefs fieds[4][MAXfields]
        { 
            {
                // AMMO
                {"FPV Model ref",ICHAR,0, true},
                {"image reference",ICHAR,20, true},
                {"Model reference",ICHAR,40, true},
                {"Unused reference",ICHAR,60, true},
                {"Label name",ICHAR,80, true},
                {"Text Reference ID",IINT,100, true},
                {"Item Weight",IFLOAT,104, true},
                {"Inventory slot",IINT,108, true},
                {"Category",IINT,112, true},
                {"Unknown",IINT,116, true},
                {"Unknown",IINT,120, true},
                {"Team ID",IINT,124, true},
                {"Unknown",IINT,128, true},
                {"Unknown",IINT,132, true},
                {"Unknown",IINT,136, false},
                {"Unknown",IFLOAT,140, false},
                {"Damage",IFLOAT,144, false},
                {"Unknown",IINT,148, false},
                {"Attack Delay",IFLOAT,152, false},
                {"Unknown",IINT,156, false},
                {"Reloadable?",IBOOL,160, false},
                {"Unknown",IFLOAT,164, false},
                {"Capacity",IFLOAT,168, true},
                {"Unknown",IINT,172, false},
                {"Unknown",IFLOAT,176, false},
                {"Velocity m/s",IFLOAT,180, false},
                {"Ideal Distance",IFLOAT,184, false},
                {"Unaimed Distance",IFLOAT,188, false},
                {"aimed Distance",IFLOAT,192, false},
                {"Unknown",IFLOAT,196, false},
                {"Unknown",IINT,200, false},
                {"Unknown",IINT,204, false},
                {"Short Reference",ICHAR,208, false},
                {"Zoom factor",IFLOAT,216, false},
                {"Unknown",IBOOL,220, false},
                {"Unknown",IINT,224, false},
                {"Unknown",IINT,228, false},
                {"Unknown",IINT,232, false},
                {"Unknown",IINT,236, false},
                {"Unknown",IBOOL,240, false},
                {"Unknown",IINT,244, false},
                {"Unknown",IINT,248, false},
                {"Unknown",IINT,252, false},
                {"Unknown",IINT,256, false},
                {"Unknown",IINT,260, false},
                {"Unknown",IINT,264, false},
                {"Unknown",IINT,268, false},
                {"Unknown",IINT,272, false},
                {"Unknown",IINT,276, false},
                {"Unknown",IINT,280, false},
                {"Unknown",IINT,284, false},
                {"Unknown",IINT,288, false},
                {"Unknown",IINT,292, false},
                {"Carry slot",IINT,296, false},
                {"Unknown",IINT,300, false},
                {"Unknown",IINT,304, false},
                {"Ammo ID",IINT,308, false},
                {"Carry slot2",IINT,312, false},
                {"Unknown",IINT,316, false},
                {"Unknown",IINT,320, false},
                {"Ammo ID2",IINT,324, false},
                {"Unknown",IINT,328, false},
                {"Unknown",IINT,332, false},
            },
            {
                // ANIMATED
                {"FPV Model ref",ICHAR,0, true},
                {"image reference",ICHAR,20, true},
                {"Model reference",ICHAR,40, true},
                {"Unused reference",ICHAR,60, true},
                {"Label name",ICHAR,80, true},
                {"Text Reference ID",IINT,100, true},
                {"Item Weight",IFLOAT,104, true},
                {"Inventory slot",IINT,108, true},
                {"Category",IINT,112, true},
                {"Unknown",IINT,116, true},
                {"Unknown",IINT,120, true},
                {"Team ID",IINT,124, true},
                {"Unknown",IINT,128, true},
                {"Unknown",IINT,132, true},
                {"Unknown",IINT,136, true},
                {"Unknown",IFLOAT,140, true},
                {"Damage",IFLOAT,144, true},
                {"Unknown",IINT,148, true},
                {"Fire trigger delay",IFLOAT,152, true},
                {"Unknown",IINT,156, true},
                {"Reloadable?",IBOOL,160, true},
                {"Unknown",IFLOAT,164, true},
                {"Unknown",IINT,168, true},
                {"Unknown",IINT,172, true},
                {"Unknown",IFLOAT,176, true},
                {"Velocity m/s",IFLOAT,180, true},
                {"Ideal Distance",IFLOAT,184, true},
                {"Unaimed Distance",IFLOAT,188, true},
                {"aimed Distance",IFLOAT,192, true},
                {"Unknown",IFLOAT,196, true},
                {"Unknown",IINT,200, true},
                {"Unknown",IINT,204, true},
                {"Short Reference",ICHAR,208, true},
                {"Zoom factor?",IFLOAT,216, true},
                {"Unknown",IBOOL,220, true},
                {"Unknown",IINT,224, true},
                {"Unknown",IINT,228, true},
                {"Unknown",IINT,232, true},
                {"Unknown",IINT,236, true},
                {"Unknown",IBOOL,240, true},
                {"Unknown",IINT,244, true},
                {"Unknown",IINT,248, true},
                {"Unknown",IINT,252, true},
                {"Unknown",IFLOAT,256, true},
                {"Unknown",IINT,260, true},
                {"Unknown",IINT,264, true},
                {"Unknown",IINT,268, true},
                {"Unknown",IINT,272, true},
                {"Unknown",IFLOAT,276, true},
                {"Unknown",IINT,280, true},
                {"Unknown",IFLOAT,284, true},
                {"Unknown",IFLOAT,288, true},
                {"Unknown",IFLOAT,292, true},
                {"Carry slot",IINT,296, true},
                {"Unknown",IINT,300, true},
                {"Unknown",IINT,304, true},
                {"Ammo ID",IINT,308, true},
                {"Carry slot2",IINT,312, true},
                {"Unknown",IINT,316, true},
                {"Unknown",IINT,320, true},
                {"Ammo ID2",IINT,324, true},
                {"Unknown",IINT,328, true},
                {"Ammo ID3",IINT,332, true},
            },
            {
                // STATIC
                {"FPV Model ref",ICHAR,0, true},
                {"image reference",ICHAR,20, true},
                {"Model reference",ICHAR,40, true},
                {"Unused reference",ICHAR,60, true},
                {"Label name",ICHAR,80, true},
                {"Text Reference ID",IINT,100, true},
                {"Item Weight",IFLOAT,104, true},
                {"Inventory slot",IINT,108, true},
                {"Category",IINT,112, true},
                {"Unknown",IINT,116, true},
                {"Unknown",IINT,120, true},
                {"Team ID",IINT,124, true},
                {"Unknown",IINT,128, true},
                {"Unknown",IINT,132, true},
                {"Unknown",IINT,136, true},
                {"Unknown",IFLOAT,140, true},
                {"Damage",IFLOAT,144, true},
                {"Unknown",IINT,148, true},
                {"Attack Delay",IFLOAT,152, true},
                {"Unknown",IINT,156, true},
                {"Reloadable?",IBOOL,160, true},
                {"Unknown",IFLOAT,164, true},
                {"Unknown",IINT,168, true},
                {"Unknown",IINT,172, true},
                {"Unknown",IINT,176, true},
                {"Velocity m/s",IINT,180, true},
                {"Ideal Distance",IINT,184, true},
                {"Unaimed Distance",IINT,188, true},
                {"aimed Distance",IINT,192, true},
                {"Unknown",IINT,196, true},
                {"Unknown",IINT,200, true},
                {"Unknown",IINT,204, true},
                {"Short Reference",ICHAR,208, false},
                {"Zoom factor",IFLOAT,216, false},
                {"Unknown",IBOOL,220, false},
                {"Unknown",IINT,224, false},
                {"Unknown",IINT,228, false},
                {"Unknown",IINT,232, false},
                {"Unknown",IINT,236, false},
                {"Unknown",IBOOL,240, false},
                {"Unknown",IINT,244, false},
                {"Unknown",IINT,248, false},
                {"Unknown",IINT,252, false},
                {"Unknown",IINT,256, false},
                {"Unknown",IINT,260, false},
                {"Unknown",IINT,264, false},
                {"Unknown",IINT,268, false},
                {"Unknown",IINT,272, false},
                {"Unknown",IINT,276, false},
                {"Unknown",IINT,280, false},
                {"Unknown",IINT,284, false},
                {"Unknown",IINT,288, false},
                {"Unknown",IINT,292, false},
                {"Carry slot",IINT,296, false},
                {"Unknown",IINT,300, false},
                {"Unknown",IINT,304, false},
                {"Ammo ID",IINT,308, false},
                {"Unknown",IINT,312, false},
                {"Unknown",IINT,316, false},
                {"Unknown",IINT,320, false},
                {"Unknown",IINT,324, false},
                {"Unknown",IINT,328, false},
                {"Unknown",IINT,332, false},
            },
            {
                // EMPTY
                {"FPV Model ref",ICHAR,0, false},
                {"image reference",ICHAR,20, false},
                {"Model reference",ICHAR,40, false},
                {"Unused reference",ICHAR,60, false},
                {"Label name",ICHAR,80, false},
                {"Text Reference ID",IINT,100, false},
                {"Item Weight",IFLOAT,104, false},
                {"Inventory slot",IINT,108, false},
                {"Category",IINT,112, false},
                {"Unknown",IINT,116, false},
                {"Unknown",IINT,120, false},
                {"Team ID",IINT,124, false},
                {"Unknown",IINT,128, false},
                {"Unknown",IINT,132, false},
                {"Unknown",IINT,136, false},
                {"Unknown",IFLOAT,140, false},
                {"Damage",IFLOAT,144, false},
                {"Unknown",IINT,148, false},
                {"Attack Delay",IFLOAT,152, false},
                {"Unknown",IINT,156, false},
                {"Reloadable?",IBOOL,160, false},
                {"Unknown",IFLOAT,164, false},
                {"Unknown",IINT,168, false},
                {"Unknown",IINT,172, false},
                {"Unknown",IFLOAT,176, false},
                {"Velocity m/s",IFLOAT,180, false},
                {"Ideal Distance",IFLOAT,184, false},
                {"Unaimed Distance",IFLOAT,188, false},
                {"aimed Distance",IFLOAT,192, false},
                {"Unknown",IFLOAT,196, false},
                {"Unknown",IINT,200, false},
                {"Unknown",IINT,204, false},
                {"Short Reference",ICHAR,208, false},
                {"Zoom factor",IFLOAT,216, false},
                {"Unknown",IBOOL,220, false},
                {"Unknown",IINT,224, false},
                {"Unknown",IINT,228, false},
                {"Unknown",IINT,232, false},
                {"Unknown",IINT,236, false},
                {"Unknown",IBOOL,240, false},
                {"Unknown",IINT,244, false},
                {"Unknown",IINT,248, false},
                {"Unknown",IINT,252, false},
                {"Unknown",IINT,256, false},
                {"Unknown",IINT,260, false},
                {"Unknown",IINT,264, false},
                {"Unknown",IINT,268, false},
                {"Unknown",IINT,272, false},
                {"Unknown",IINT,276, false},
                {"Unknown",IINT,280, false},
                {"Unknown",IINT,284, false},
                {"Unknown",IINT,288, false},
                {"Unknown",IINT,292, false},
                {"Carry Mode",IINT,296, false},
                {"Unknown",IINT,300, false},
                {"Unknown",IINT,304, false},
                {"Ammo ID",IINT,308, false},
                {"Unknown",IINT,312, false},
                {"Unknown",IINT,316, false},
                {"Unknown",IINT,320, false},
                {"Unknown",IINT,324, false},
                {"Unknown",IINT,328, false},
                {"Unknown",IINT,332, false},
            }
        };

        Item(int32_t recordType)
            : recordType(recordType) {}

        // gets description and value AS STRING
        bool getFieldInformation(uint32_t fieldID, std::string& descr, std::string& value) {
            
            getFieldInformation(fieldID, value);
            descr.assign(fieds[recordType][fieldID].descr);
            
            return true;
        }

        // gets only value as string
        bool getFieldInformation(uint32_t fieldID, std::string& value) {

            char* curr = (char*)&items + fieds[recordType][fieldID].position;
            if (checkDWORD(curr)) {
                value.assign("");
                return true;
            }

            switch (fieds[recordType][fieldID].dataType) {
            case ICHAR:
                value.assign((char*)&items + fieds[recordType][fieldID].position);
                break;
            case IINT:
                value.assign(std::to_string(*(int32_t*)curr));
                break;
            case IFLOAT:
                value.assign(std::to_string(*(float*)curr));
                break;
            case IBOOL:
                value.assign(std::to_string((uint8_t)*curr));
                break;
            default:
                return false;
            }

            return true;
        }

        bool setFieldInformation(const uint32_t fieldID, const std::string& value) {
            if (fieldID > MAXfields) return false;

            switch (fieds[recordType][fieldID].dataType) {
            case ICHAR:
                memcpy((char*)&items + fieds[recordType][fieldID].position, value.c_str(), value.size()+1);
                break;
            case IINT:
                if (value.empty()) {
                    (*(int32_t*)((char*)&items + fieds[recordType][fieldID].position)) = 0;
                    break;
                }
                try {
                    (*(int32_t*)((char*)&items + fieds[recordType][fieldID].position)) = std::stoi(value);
                } catch(...) {
                    return false;
                }
                    
                break;
            case IFLOAT:
                if (value.empty()) {
                    (*(float*)((char*)&items + fieds[recordType][fieldID].position)) = 0.0f;
                    break;
                }
                try {
                    (*(float*)((char*)&items + fieds[recordType][fieldID].position)) = std::stof(value);
                }
                catch (...) {
                    return false;
                }
                break;
            case IBOOL:
                if (value.empty()) {
                    *((uint8_t*)&items + fieds[recordType][fieldID].position) = 0;
                    break;
                }
                try {
                    *((uint8_t*)&items + fieds[recordType][fieldID].position) = std::stoi(value);
                }
                catch (...) {
                    return false;
                }
                break;
            default:
                return false;
            }
            return true;
        }
        
    };

    std::vector<Item>items;
    bool good = false;
    uint32_t fileSize;

    static uint16_t checkDWORD(void* i) {
        // uninitialised heap memory
        if (*(int32_t*)i == 0xCDCDCDCD) return 2;
           //uninitialised stack memory
        else if (*(int32_t*)i == 0xCCCCCCCC) return 1;
            // freed memory
        else if (*(int32_t*)i == 0xFFFFFFFF) return 3;
        else return 0;
    }
};

