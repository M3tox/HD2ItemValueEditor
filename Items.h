#pragma once

#include <fstream>
#include <vector>
#include <string>

#define STRINGSIZE 20

class Items
{
public:
    enum RECORD_TYPE {
        AMMO,
        WEAPONS,
        GEAR,
        EMPTY
    };

    enum ITEM_TYPE {
        STANDARD,
        MELEE,
        EXPLOPASSIVE,
        EXPLOACTIVE,
        FIREARM,
        BINOCULAR,
        UNIFORM,
        HEALING,
        BACKPACK,
        HEADGEAR
    };
    // default structure
    struct ItemBase {
        char strModelFPV[STRINGSIZE];
        char strImage[STRINGSIZE];
        char strModel[STRINGSIZE];
        char unknownRef[STRINGSIZE];
        char labelName[STRINGSIZE];
        int32_t textRefID;
        float weight;
        int32_t inventorySlotType;
        int32_t category;
        int32_t unknownI3;
        int32_t unknownI4;
        int32_t teamID;
        int32_t itemType;
        int32_t unknownI5;  // always 0

    };
    struct Ammo : public ItemBase {
        int32_t unused[8];
        float ammoCapacity;
    };

    struct AmmoData {
        int32_t unused[8];
        int32_t Unknown1;
        int32_t AmmoType;
        int32_t Unknown3;
        int32_t AmmoID;
    };

    // this block depends in a certain field, its either there or not
    struct OptionalExtra {
        int32_t unknownI1;
        int32_t unknownI2;
        int32_t unknownI3;
        float unknownF;
    };

    struct Weapon : public ItemBase {
        int32_t weaponType;
        float unknownF1;
        float Damage;
        int32_t unknownI1;
        float fireDelay;
        int32_t unknownI2;
        bool reloadable;
    };

    // No 1 Knifes, fists etc.
    struct Melee : public Weapon {
        float unknownF2;
        int32_t unknownI3;
        int32_t unused[8];
        int32_t unknownI4;
    };
    // No 2 mines and charges
    struct PassiveExplosive : public Weapon {
        int32_t unknownI3;
        float unknownF2;

        int32_t extra2Type;
        // plus extra data, if type active
    };
    // No 3 grenades, bazooka, panzerfaust, panzerschreck, tank canons, bazooka etc.
    struct ActiveExplosive : public Weapon {
        char shortName[8];
        int32_t unknownI4;
        int32_t unknownI5;
        float unknownF4;
        float velocity;
        float idealRange;
        float unAimedDistance;
        float aimedDistance;
        int32_t unknownI6;
        int32_t unknownI7;
        int32_t unknownI8;
        int32_t unknownI9;
        int32_t unknownI10;
        float unknownF8;
        float unknownF9;
        int32_t unused[10];
        int32_t extra2Type;
        // plus extra data, if type active
    };
    // No 4 Pistoles, rifles, automatic guns etc.
    struct Firearm : public Weapon {
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

        float zoomFactor;
        int32_t unused[10];
        // this field is 5 if category field is NOT 5, otherwise its always 0
        int32_t optionalExtra;
        // plus extra data, if type active
    };

    struct GearExtra : public AmmoData {
        int32_t unknownI8;  // always 0
    };

    struct Gear : public ItemBase {
        int32_t itemType;
    };

    // No 5 
    struct Binocular : public Gear {
        float unknownF2;
        float zoomfactor; // ?
        int32_t unknownI4;

        GearExtra gextra;
    };

    // No 6
    struct Uniform : public Gear {
        float unknownF2;
        float unknownF3;

        GearExtra gextra;
    };

    // No 7
    struct Healing : public Gear {
        float healpower;
        float unknownF3;

        GearExtra gextra;
    };

    // No 8
    struct Backpack : public Gear {
        int32_t unknownI4;
        float unknownF3;
        float unknownF4;
        int32_t unknownI5;
        float unknownF5;
        int32_t unknownI6;

        GearExtra gextra;
    };

    // No 9
    struct Headgear : public Gear {
        float unknownF2;
        float unknownF3;
        int32_t unknownI4;

        GearExtra gextra;
    };

    // No 0 and 10
    struct Standard : public ItemBase, public GearExtra {};


    // use this if you want to read file
    Items(const char* path);
    // use this is if file is already in RAM (e.g. directly from .dta)
    Items(uint8_t* data, uint32_t dataSize);
    ~Items();

    void writeCSV(const char* fileName);
    void writeBinaryFile(const char* fileName);
    bool isGood() { return good; };

    uint32_t getItemCount() const { return maxElements; };
    //uint32_t getFieldCount() const { return items.back().MAXfields; };
    inline const char* getItemName(uint32_t itemID) const { return items[itemID].itemData->labelName; };

    inline int32_t* getRecordType(uint32_t itemID) {
        if (items[itemID].itemData) {
            return items[itemID].recordType;
        }
            
        return nullptr;
    }

    inline int32_t getItemType(uint32_t itemID) {
        if (items[itemID].itemData)
            return items[itemID].itemData->itemType;
        return 0;
    }

    inline ItemBase* getItemReference(uint32_t itemID) {
        return items[itemID].itemData;
    }

    inline bool setRecordType(uint32_t itemID, int32_t recType) {
        if (recType > 3) return false;
        *items[itemID].recordType = recType;
        return true;
    }

    inline bool setItemType(uint32_t itemID, int32_t itemType) {
        items[itemID].itemData->itemType = itemType;
        return true;
    }

    bool resetSubRecord(uint32_t itemID, uint16_t sectionID, int32_t* newRecordLocation) {

        if (sectionID == 1) {
            switch (*newRecordLocation) {
            case AMMO:
            {
                 Ammo* mRef = (Ammo*)getItemReference(itemID);
                 mRef->itemType = 0;
                 mRef->ammoCapacity = 0;
                 break;
            }
            case WEAPONS:
            {
                Melee* mRef = (Melee*)getItemReference(itemID);
                // base
                mRef->itemType = 1;
                mRef->weaponType = 0;
                mRef->unknownF1 = 0;
                mRef->Damage = 0;
                mRef->unknownI1 = 0;
                mRef->fireDelay = 0;
                mRef->unknownI2 = 0;
                mRef->reloadable = 0;

                // meele, default selection
                mRef->unknownF2 = 0.0f;
                mRef->unknownI3 = 0;
                mRef->unknownI4 = 0;
                break;
            }

            case GEAR:
            {
                Gear* mRef = (Gear*)getItemReference(itemID);
                mRef->itemType = 0;
                break;
            }
                
            case EMPTY:
                // do nothing, return out
                return true;
            }
        }
        else if (sectionID == 12) {
            switch (*getRecordType(itemID)) {
            case WEAPONS:

                switch (*newRecordLocation) {
                case MELEE:
                {
                    Melee* mRef = (Melee*)getItemReference(itemID);
                    mRef->unknownF2 = 0.0f;
                    mRef->unknownI3 = 0;
                    mRef->unknownI4 = 0;
                    break;
                }

                case EXPLOPASSIVE:
                {
                    PassiveExplosive* mRef = (PassiveExplosive*)getItemReference(itemID);
                    mRef->unknownI3 = 0;
                    mRef->unknownF2 = 0;
                    mRef->extra2Type = 0;
                    break;
                }
                case EXPLOACTIVE:
                {
                    ActiveExplosive* mRef = (ActiveExplosive*)getItemReference(itemID);
                    *mRef->shortName = 0;
                    mRef->unknownI4 = 0;
                    mRef->unknownI5 = 0;
                    mRef->unknownF4 = 0;
                    mRef->velocity = 0;
                    mRef->idealRange = 0;
                    mRef->unAimedDistance = 0;
                    mRef->aimedDistance = 0;
                    mRef->unknownI6 = 0;
                    mRef->unknownI7 = 0;
                    mRef->unknownI8 = 0;
                    mRef->unknownI9 = 0;
                    mRef->unknownI10 = 0;
                    mRef->unknownF8 = 0;
                    mRef->unknownF9 = 0;

                    mRef->extra2Type = 0;
                    break;
                }

                case FIREARM:
                {
                    Firearm* mRef = (Firearm*)getItemReference(itemID);
                    mRef->unknownF3_5 = 0;
                    mRef->unknown12 = 0;
                    mRef->unknown13 = 0;
                    mRef->unknownF4 = 0;
                    mRef->velocity = 0;
                    mRef->idealRange = 0;
                    mRef->unAimedDistance = 0;
                    mRef->aimedDistance = 0;
                    mRef->unknownF9 = 0;
                    mRef->unknown14 = 0;
                    mRef->unknown15 = 0;
                    *mRef->shortName = 0;
                    mRef->zoomFactor = 0;

                    mRef->optionalExtra = 0;
                    break;
                }

                case 6:
                    // TODO: Find a solution...
                    break;
                }

                break;
            case GEAR:
                switch (*newRecordLocation) {
                case BINOCULAR:
                {
                    Binocular* mRef = (Binocular*)getItemReference(itemID);
                    mRef->unknownF2 = 0;
                    mRef->zoomfactor = 0;
                    mRef->unknownI4 = 0;
                    break;
                }
                case UNIFORM:
                {
                    Uniform* mRef = (Uniform*)getItemReference(itemID);
                    mRef->unknownF2 = 0;
                    mRef->unknownF3 = 0;
                    break;
                }
                case HEALING:
                {
                    Healing* mRef = (Healing*)getItemReference(itemID);
                    mRef->healpower = 0;
                    mRef->unknownF3 = 0;
                    break;
                }
                case BACKPACK:
                {
                    Backpack* mRef = (Backpack*)getItemReference(itemID);
                    mRef->unknownI4 = 0;
                    mRef->unknownF3 = 0;
                    mRef->unknownF4 = 0;
                    mRef->unknownI5 = 0;
                    mRef->unknownF5 = 0;
                    mRef->unknownI6 = 0;
                    break;
                }
                case HEADGEAR:
                {
                    Headgear* mRef = (Headgear*)getItemReference(itemID);
                    mRef->unknownF2 = 0;
                    mRef->unknownF3 = 0;
                    break;
                }
                }

                break;
            }
        }
        else if (sectionID > 12) {
            // special Dropdown expected
            // some data must be shifted by 16 bytes either up or down, depending in the selection

            if (*newRecordLocation++) {

                // shift ammo data 16 bytes forward
                AmmoData* ad = (AmmoData*)newRecordLocation;
                memcpy(newRecordLocation + 4, ad, sizeof(AmmoData));

                // zero out new data
                OptionalExtra* opE = (OptionalExtra*)newRecordLocation;
                opE->unknownI1 = 0;
                opE->unknownI2 = 0;
                opE->unknownI3 = 0;
                opE->unknownF = 0;


            }
            else {
                // shift ammo data 16 bytes back
                AmmoData* ad = (AmmoData*)(newRecordLocation+4);
                memcpy(newRecordLocation, ad, sizeof(AmmoData));
                memset((uint8_t*)newRecordLocation + sizeof(AmmoData), 0xCD, 16);
            }
            
        }
        return true;
    }

    bool insertNewRecord(uint32_t itemID) {
        // item must be empty
        if (items[itemID].itemData != nullptr)
            return false;

        // find last non empty item
        uint32_t lastI = maxElements;
        while (lastI--) {
            if (items[lastI].itemData != nullptr)
                break;
        }

        int32_t* from = items[itemID].recordType;
        int32_t* to = ((int32_t*)items[lastI].itemData)+125 + (maxElements - lastI)-1;

        memcpy(from + 127, from,(to-from)*4);
        int32_t* curr = from;

        *from++ = 1;
        *from++ = GEAR;
        memset(from,0xCD,500);
        

        ItemBase* item = (ItemBase*)from;
        item->strModelFPV[0] = '\0';
        item->strImage[0] = '\0';
        item->strModel[0] = '\0';
        item->unknownRef[0] = '\0';
        memcpy(item->labelName, "New item", 15);
        item->textRefID = 0;
        item->weight = 0.0f;
        item->inventorySlotType = 0;
        item->category = 0;
        item->unknownI3 = 0;
        item->unknownI4 = 0;
        item->teamID = 0;
        item->itemType = 0;
        item->unknownI5 = 0;
        from = &item->unknownI5;
        from++;
        *from = 0;

        // extra data
        memset(from + 8, 0, 20);

        // update pointers
        for (uint32_t i = itemID; i < maxElements; i++) {
            if (*curr++) {
                // Item OK
                items[i].recordType = (int32_t*)curr++;
                items[i].itemData = (ItemBase*)curr;
                curr += 125;
                continue;
            }
            // Item EMPTY
            items[i].recordType = (int32_t*)(curr - 1);
            items[i].itemData = nullptr;
        }



        return true;
    }

    bool removeRecord(uint32_t itemID) {
        // find last non empty item
        uint32_t lastI = maxElements;
        while (lastI--) {
            if (items[lastI].itemData != nullptr)
                break;
        }

        int32_t* from = items[itemID].recordType;
        int32_t* to = ((int32_t*)items[lastI].itemData) + 125 + (maxElements - lastI) - 1;

        memcpy(from, from+126, (to-from-126)*4);
        *(from-1) = 0;
        items[itemID].itemData = nullptr;

        // Set rest to 0xCD
        memset(to-126, 0xCD, 126 * 4);

        // update pointers
        int32_t* curr = from;
        for (uint32_t i = itemID+1; i < maxElements; i++) {
            if (*curr++) {
                // Item OK
                items[i].recordType = (int32_t*)curr++;
                items[i].itemData = (ItemBase*)curr;
                curr += 125;
                continue;
            }
            // Item EMPTY
            items[i].recordType = (int32_t*)(curr - 1);
            items[i].itemData = nullptr;
        }

        return true;
    }

private:

    struct Item {
        int32_t* recordType;
        ItemBase* itemData = nullptr;
    };

    uint8_t* TotalSpace = nullptr;
    Item items[500];
    uint32_t maxElements;

    bool good = false;
    // HD2 without addon and with addon
    uint32_t fileSize;
};


