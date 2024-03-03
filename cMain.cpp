#include "cMain.h"

#define OPENFILE		10001
#define SAVEFILE		10002
#define SAVEFILEAS		10003
#define EXPORTCSV		10004
#define ABOUT			10005
#define EXITAPP			10006
#define VALUECHNG		20000
#define SELECTCHG		10008
#define SELECTREC		10009

wxBEGIN_EVENT_TABLE(cMain::wAbout, wxFrame)
EVT_BUTTON(11001, cMain::wAbout::onOk)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
EVT_MENU(OPENFILE, cMain::OnMenuOpenFile)
EVT_MENU(SAVEFILE, cMain::OnMenuSaveFile)
EVT_MENU(SAVEFILEAS, cMain::OnMenuSaveFileAs)
EVT_MENU(EXPORTCSV, cMain::OnExportTxt)
EVT_MENU(ABOUT, cMain::OnAbout)
EVT_MENU(EXITAPP, cMain::OnMenuExit)
EVT_TEXT(wxID_ANY, cMain::OnValueChange)
EVT_COMBOBOX(SELECTCHG, cMain::OnSelectionChange)
EVT_CLOSE(cMain::OnClose)
wxEND_EVENT_TABLE()

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "H&D2 Item Value Editor", wxDefaultPosition, wxSize(800, 600)) {

	Centre();
	// add menu bar
	m_MenuBar = new wxMenuBar();
	this->SetMenuBar(m_MenuBar);
	// add items
	menuFile = new wxMenu();
	menuFile->Append(OPENFILE, "Open File");
	menuFile->Append(SAVEFILE, "Save")->Enable(false);
	menuFile->Append(SAVEFILEAS, "Save as...")->Enable(false);
	menuFile->AppendSeparator();
	menuFile->Append(EXITAPP, "Exit");
	m_MenuBar->Append(menuFile, "File");

	menuExtras = new wxMenu();
	//menuExtras->Append(EXPORTCSV, "Export as CSV file")->Enable(false);
	menuExtras->Append(ABOUT, "About...");
	m_MenuBar->Append(menuExtras, "Extras");

	topInfoPlane = new wxPanel(this, wxID_ANY);
	topInfoPlane->SetBackgroundColour(wxColor(210, 210, 210));

	infoText = new wxStaticText(topInfoPlane, wxID_ANY, "No file loaded", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
	selector = new wxComboBox(topInfoPlane, SELECTCHG, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0 ,wxCB_READONLY);

	wxBoxSizer* hbp = new wxBoxSizer(wxHORIZONTAL);
	hbp->Add(infoText, 1, wxALIGN_CENTRE);
	hbp->Add(selector, 1, wxALIGN_CENTRE);
	topInfoPlane->SetSizerAndFit(hbp);

	scrolleditemList = nullptr;

	vbp = new wxBoxSizer(wxVERTICAL);

	vbp->Add(topInfoPlane, 0, wxGROW);

	this->SetSizer(vbp);


	primaryArea = new wxBoxSizer(wxVERTICAL);

	scrolleditemList = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	scrolleditemList->Hide();
	vbp->Add(scrolleditemList, 1, wxEXPAND);
}

cMain::~cMain() {

}

void cMain::OnMenuOpenFile(wxCommandEvent& evt) {
	if (dataChanged)
	{
		if (wxMessageBox(_("Current content has not been saved! Proceed?"), _("WARNING"),
			wxICON_QUESTION | wxYES_NO, this) == wxNO)
			return;
		//else: proceed asking to the user the new file to open
	}

	wxFileDialog openFileDialog(this, _("Open Items Configuration"), "", "", "Item file (*.sav)|*.sav", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed idea...

	// proceed loading the file chosen by the user;
	wxFileInputStream input_stream(openFileDialog.GetPath());
	if (!input_stream.IsOk())
	{
		wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
		return;
	}

	if (openFileDialog.GetPath().empty()) return;

	selectedItem = 0;
	delete ItemValues;
	pathOpened.assign(openFileDialog.GetPath());
	ItemValues = new Items(pathOpened.ToStdString().c_str());

	if (!ItemValues->isGood()) {
		wxMessageBox("Bad format: Can't interpret file!", "Error Message", wxICON_WARNING | wxOK);

		delete ItemValues;
		ItemValues = nullptr;
		return;
	}


	menuFile->FindItemByPosition(1)->Enable();
	menuFile->FindItemByPosition(2)->Enable();
	menuExtras->FindItemByPosition(0)->Enable();

	// update status text
	infoText->SetLabelText(openFileDialog.GetFilename());

	selector->Clear();
	// update dropdown menu
	for (uint32_t i = 0; i < ItemValues->getItemCount(); i++) {
		wxString name("ID: " + std::to_string(i) + " | ");
		if (ItemValues->getRecordType(i) == nullptr || *ItemValues->getRecordType(i) == Items::EMPTY)
			name.append("EMPTY SLOT");
		else
			name.append(ItemValues->getItemName(i));
		selector->AppendString(name);
	}
	selector->Select(0);

	// update fields
	drawDataFields();

	evt.Skip();
}

void cMain::OnMenuSaveFile(wxCommandEvent& evt) {

	ItemValues->writeBinaryFile(pathOpened.ToStdString().c_str());

	dataChanged = false;
	evt.Skip();
}

void cMain::OnMenuSaveFileAs(wxCommandEvent& evt) {

	wxFileDialog saveFileDialog(this, _("Save Item save file"), "", "", "Item file (*.sav)|*.sav", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (saveFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed idea...

	ItemValues->writeBinaryFile(saveFileDialog.GetPath().ToStdString().c_str());
	pathOpened = saveFileDialog.GetPath();
	infoText->SetLabelText(saveFileDialog.GetFilename());
	dataChanged = false;
	evt.Skip();
}

void cMain::OnExportTxt(wxCommandEvent& evt) {

	wxFileDialog saveFileDialog(this, _("Save CSV file"), "", "", "Text files (*.csv)|*.csv", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (saveFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed idea...


	ItemValues->writeCSV(saveFileDialog.GetPath().ToStdString().c_str());
	evt.Skip();
}

void cMain::OnMenuExit(wxCommandEvent& evt) {

	Close();
	evt.Skip();
}

void cMain::OnClose(wxCloseEvent& evt)
{
	if (evt.CanVeto() && dataChanged)
	{
		if (wxMessageBox("Current content has not been saved! Exit anyway?",
			"WARNING", wxICON_QUESTION | wxYES_NO) != wxYES)
		{
			evt.Veto();
			return;
		}
	}
	Destroy();
	evt.Veto();
}

void cMain::OnSelectionChange(wxCommandEvent& evt)
{
	selectedItem = selector->GetSelection();
	drawDataFields();
	evt.Skip();
}

void cMain::drawDataFields(bool skipMsg) {

	int32_t* recordType = ItemValues->getRecordType(selectedItem);
	if (recordType == nullptr || *recordType == Items::EMPTY) {
		scrolleditemList->Hide();

		if (skipMsg) return;

		int32_t msg = wxMessageBox("This Item Record is empty, would you like to create one?", "Item Record", wxYES_NO);
		if (msg == wxYES) {
			ItemValues->insertNewRecord(selectedItem);
			// refresh recordType with new location
			recordType = ItemValues->getRecordType(selectedItem);
			*recordType = Items::GEAR;
			selector->SetString(selectedItem, "ID: " + std::to_string(selectedItem) + " | New item");
		} else
			return;
	} else if (*recordType > 3) {
		wxMessageBox("Record type not defined! It was " + std::to_string(*recordType), "Error Message", wxICON_ERROR | wxOK);
		return;
	}

	//std::string descr;
	//std::string val;
	
	scrolleditemList->Hide();
	for (auto& s : sections)
		delete s;
	sections.clear();

	// generate the basic fields that are in all types
	// 
	// add selector for record type via dropdown
	std::vector<wxString> ddLabels;
	ddLabels = { "AMMO", "WEAPON", "GEAR", "EMPTY"};
	addDropDown("Record type: ", ddLabels, *recordType);

	Items::ItemBase* itemRef = ItemValues->getItemReference(selectedItem);
	addField("FPV Model reference", itemRef->strModelFPV);
	addField("Image reference", itemRef->strImage);
	addField("Model reference", itemRef->strModel);
	addField("Label", itemRef->labelName);
	addField("Text reference ID", itemRef->textRefID);
	addField("Item weight", itemRef->weight);
	addField("Inventory slot", itemRef->inventorySlotType);
	addField("Category", itemRef->category);
	addField("Armed sound reference ID", itemRef->unknownI4);
	addField("Team ID", itemRef->teamID);

	int32_t itemTypeToSelect = itemRef->itemType;
	ddLabels.clear();
	switch (*recordType) {
	case Items::AMMO:
		ddLabels = { "Standard (0)"};
		break;
	case Items::WEAPONS:
		ddLabels = {"Melee (1)", "Passive Explosives (2)", "Active Explosives (3)", "Firearm (4)","Special knives (6)"};
		itemTypeToSelect--;
		if (itemTypeToSelect == 5) itemTypeToSelect--;
		break;
	case Items::GEAR:
		ddLabels = { "Standard (0)", "Binocular (5)","Uniform (6)", "Healing (7)", "Backpack (8)", "Headgear (9)", "Special (10)" };
		if (itemTypeToSelect != 0) itemTypeToSelect -= 4;
		break;
	}
	addDropDown("Item type: ", ddLabels, itemRef->itemType, itemTypeToSelect);
	//ddLabels = { "Standard (0)", "Melee (1)", "Passive Explosives (2)", "Active Explosives (3)", "Firearm (4)",
	//			"Binocular (5)","Uniform (6)", "Healing (7)", "Backpack (8)", "Headgear (9)", "Unknown (10)" };



	//memcpy(mainSections, sections.data(),sizeof(mainSections));
	//sections.clear();

	scrolleditemList->SetSizer(primaryArea);
	scrolleditemList->SetScrollRate(20, 20);

	// now check what comes next and show specialized data
	refreshDataFields(itemRef);


	dataChanged = false;
}

void cMain::refreshDataFields(Items::ItemBase* itemRef) {
	
	//scrolleditemList->Hide();

	//for (auto& s : sections)
	//	delete s;
	//sections.clear();

	switch (int32_t selType = *ItemValues->getRecordType(selectedItem)) {
	case Items::AMMO:
	{
		Items::Ammo* ammoRef = (Items::Ammo*)itemRef;
		addField("Ammo capacity", ammoRef->ammoCapacity);
		break;
	}

	case Items::WEAPONS:
	{
		{
			Items::Weapon* mRef = (Items::Weapon*)itemRef;
			addField("Weapon Type", mRef->weaponType);
			addField(mRef->unknownF1);
			addField("Damage", mRef->Damage);
			addField("Shoot sound reference ID", mRef->unknownI1);
			addField("Fire trigger delay", mRef->fireDelay);
			addField(mRef->unknownI2);
			addField("Reload any moment / last patron",mRef->reloadable);
		}

		int32_t* posExtraData = nullptr;

		switch (int32_t itemType = itemRef->itemType) {
		case Items::MELEE:
		{
			Items::Melee* mRef = (Items::Melee*)itemRef;
			addField(mRef->unknownF2);
			addField(mRef->unknownI3);
			addField(mRef->unknownI4);
			break;
		}
		case Items::EXPLOPASSIVE:
		{
			Items::PassiveExplosive* mRef = (Items::PassiveExplosive*)itemRef;
			addField(mRef->unknownI3);
			addField(mRef->unknownF2);
			addField(mRef->extra2Type);
			break;
		}
		case Items::EXPLOACTIVE:
		{
			Items::ActiveExplosive* mRef = (Items::ActiveExplosive*)itemRef;
			addField("Short Reference", mRef->shortName);
			addField(mRef->unknownI4);
			addField(mRef->unknownI5);
			addField("Reload speed", mRef->unknownF4);
			addField("Velocity m/s", mRef->velocity);
			addField("Distance ideal", mRef->idealRange);
			addField("Distance blind", mRef->unAimedDistance);
			addField("Distance aimed", mRef->aimedDistance);
			addField(mRef->unknownI6);
			addField(mRef->unknownI7);
			addField(mRef->unknownI8);
			addField(mRef->unknownI9);
			addField(mRef->unknownI10);
			addField(mRef->unknownF8);
			addField(mRef->unknownF9);

			posExtraData = (int32_t*)&mRef->extra2Type;
			if (*posExtraData == 5)
				addDropDown("Optional extra fields", { "No","Yes" }, mRef->extra2Type, 1);
			else
				addDropDown("Optional extra fields", { "No","Yes" }, mRef->extra2Type);
			//addField("Optional extra fields", *posExtraData);

			break;
		}
		case Items::FIREARM:
		{
			Items::Firearm* mRef = (Items::Firearm*)itemRef;
			addField(mRef->unknownF3_5);
			addField(mRef->unknown12);
			addField(mRef->unknown13);
			addField(mRef->unknownF4);
			addField("Velocity m/s", mRef->velocity);
			addField("Distance ideal", mRef->idealRange);
			addField("Distance blind", mRef->unAimedDistance);
			addField("Distance aimed", mRef->aimedDistance);
			addField(mRef->unknownF9);
			addField(mRef->unknown14);
			addField(mRef->unknown15);
			addField("Short reference", mRef->shortName);
			addField("Zoom factor", mRef->zoomFactor);

			posExtraData = (int32_t*)&mRef->optionalExtra;
			if (*posExtraData == 5)
				addDropDown("Optional extra fields", { "No","Yes" }, mRef->optionalExtra, 1);
			else
				addDropDown("Optional extra fields", { "No","Yes" }, mRef->optionalExtra);
			//addField("Optional extra fields", *posExtraData);

			break;
		}
		case Items::UNIFORM:
		{
			Items::Uniform* mRef = (Items::Uniform*)itemRef;
			// TODO: Find a solution...
			break;
		}
		default:
			wxMessageBox("ERROR: Item type not defined! It was " + std::to_string(itemType) + ". Selected: " + std::to_string(selType), "Error Message", wxICON_ERROR | wxOK);
			return;
		}

		if (posExtraData) {
			if (*posExtraData++) {
				Items::OptionalExtra* opE = (Items::OptionalExtra*)posExtraData;
				addField(opE->unknownI1);
				addField(opE->unknownI2);
				addField(opE->unknownI3);
				addField(opE->unknownF);
				posExtraData += 4;
			}
			Items::AmmoData* ad = (Items::AmmoData*)posExtraData;
			addField(ad->Unknown1);
			addField("Ammo type", ad->AmmoType);
			addField(ad->Unknown3);
			addField("Ammo ID", ad->AmmoID);
		}

		break;
	}
	case Items::GEAR:
		Items::GearExtra* gextraReference = (Items::GearExtra*)(((int32_t*)&itemRef->unknownI5)+1);
		switch (itemRef->itemType) {
		case Items::BINOCULAR:
		{
			Items::Binocular* mRef = (Items::Binocular*)itemRef;
			addField(mRef->unknownF2);
			addField("Zoom? FOV?", mRef->zoomfactor);
			addField(mRef->unknownI4);
			gextraReference = &mRef->gextra;
			break;
		}
		case Items::UNIFORM:
		{
			Items::Uniform* mRef = (Items::Uniform*)itemRef;
			addField(mRef->unknownF2);
			addField(mRef->unknownF3);
			gextraReference = &mRef->gextra;
			break;
		}
		case Items::HEALING:
		{
			Items::Healing* mRef = (Items::Healing*)itemRef;
			addField("Healing effect", mRef->healpower);
			addField(mRef->unknownF3);
			gextraReference = &mRef->gextra;
			break;
		}
		case Items::BACKPACK:
		{
			Items::Backpack* mRef = (Items::Backpack*)itemRef;
			addField(mRef->unknownI4);
			addField(mRef->unknownF3);
			addField(mRef->unknownF4);
			addField(mRef->unknownI5);
			addField(mRef->unknownF5);
			addField(mRef->unknownI6);
			gextraReference = &mRef->gextra;
			break;
		}
		case Items::HEADGEAR:
		{
			Items::Headgear* mRef = (Items::Headgear*)itemRef;
			addField(mRef->unknownF2);
			addField(mRef->unknownF3);
			gextraReference = &mRef->gextra;
			break;
		}
		}

		addField(gextraReference->Unknown1);
		addField(gextraReference->AmmoType);
		addField(gextraReference->Unknown3);
		addField(gextraReference->AmmoID);
		addField(gextraReference->unknownI8);
	}


	scrolleditemList->Show();
	vbp->Layout();
}

void cMain::OnAbout(wxCommandEvent& evt) {
	// avoid starting multiple instances of the window
	if (about && !about->IsBeingDeleted())
		about->Destroy();

	about = new wAbout(this);
	about->Show();
}

void cMain::OnValueChange(wxCommandEvent& evt) {

	uint32_t field = evt.GetId() - VALUECHNG;
	if (field > 500) {
		evt.Skip();
		return;
	
	}

	dataChanged = true;
	sections.at(field)->updateInternalData();
	
	evt.Skip();
	return;
}

void cMain::addField(float& val) {
	addField("Unknown", val);
}
void cMain::addField(int32_t& val) {
	addField("Unknown", val);
}

void cMain::addField(bool& val) {
	addField("Unknown", val);
}

void cMain::addField(const char*& val) {
	addField("Unknown", val);
}

void cMain::addField(const char* label, float& val) {
	addField(label, wxString(std::to_string(val)), TyFLOAT, &val);
}
void cMain::addField(const char* label, int32_t& val) {
	addField(label, wxString(std::to_string(val)), TyINT, &val);
}

void cMain::addField(const char* label, bool& val) {
	addField(label, wxString(std::to_string(val)), TyBOOL, &val);
}

void cMain::addField(const char* label, const char* val) {
	addField(label, wxString(val), TyCHAR, (void* )val);
}

void cMain::addField(const char* label, wxString val, uint16_t originType, void* valueOrigin)
{
	sections.push_back(new Section(this, primaryArea, label, val, originType, valueOrigin));
	/*
	wxBoxSizer* colSizer = new wxBoxSizer(wxHORIZONTAL);
	colSizer->AddSpacer(10);

	colSizer->Add(new wxStaticText(scrolleditemList, wxID_ANY, std::to_string(defaultFields.size() + 1)), 1, wxGROW | wxALL, 2);
	colSizer->Add(new wxStaticText(scrolleditemList, wxID_ANY, label), 3, wxGROW | wxALL, 2);

	defaultFields.push_back(new wxTextCtrl(scrolleditemList, VALUECHNG + defaultFields.size(), val));
	colSizer->Add(defaultFields.back(), 3, wxGROW | wxALL, 2);
	
	area->Add(colSizer, 1, wxEXPAND);
	*/
}

void cMain::addDropDown(const char* label, const std::vector<wxString>& strings, int32_t& select, int32_t altSelect) {
	sections.push_back(new Section(this, primaryArea, label, strings, select, altSelect));
}



cMain::Section::Section(cMain* mainRef, wxBoxSizer* Area, const char* label, wxString val, uint16_t originType, void* originLoc)
	: mainRef(mainRef), Area(Area), originType(originType), originLoc(originLoc) {

	sectionID = mainRef->sections.size()+1;
	colSizer = new wxBoxSizer(wxHORIZONTAL);
	colSizer->AddSpacer(10);

	number = new wxStaticText(scrolleditemList, wxID_ANY, std::to_string(Area->GetItemCount() + 1));
	txtLabel = new wxStaticText(scrolleditemList, wxID_ANY, label);
	txtField = new wxTextCtrl(scrolleditemList, VALUECHNG + Area->GetItemCount(), val);

	colSizer->Add(number, 1, wxGROW | wxALL, 2);
	colSizer->Add(txtLabel, 3, wxGROW | wxALL, 2);
	colSizer->Add(txtField, 3, wxGROW | wxALL, 2);
	Area->Add(colSizer, 1, wxEXPAND);
	// not used
	dropDownField = nullptr;
}

cMain::Section::Section(cMain* mainRef, wxBoxSizer* Area, const char* label, const std::vector<wxString>& strings, int32_t& select, int32_t altSelect)
	: mainRef(mainRef), Area(Area), originType(TyINT), originLoc(&select) {

	sectionID = mainRef->sections.size()+1;
	colSizer = new wxBoxSizer(wxHORIZONTAL);
	colSizer->AddSpacer(10);

	number = new wxStaticText(scrolleditemList, wxID_ANY, std::to_string(Area->GetItemCount() + 1));
	txtLabel = new wxStaticText(scrolleditemList, wxID_ANY, label);
	dropDownField = new wxComboBox(scrolleditemList, VALUECHNG + Area->GetItemCount(), wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY);
	for (auto& s : strings)
		dropDownField->AppendString(s);

	if (altSelect != -1) dropDownField->Select(altSelect);
		else
	dropDownField->Select(select);

	colSizer->Add(number, 1, wxGROW | wxALL, 2);
	colSizer->Add(txtLabel, 3, wxGROW | wxALL, 2);
	colSizer->Add(dropDownField, 3, wxGROW | wxALL, 2);
	Area->Add(colSizer, 1, wxEXPAND);

	// not used
	txtField = nullptr;
}

bool cMain::Section::updateInternalData()
{
	if (txtField) {
		switch (originType) {
		case TyCHAR:
			return updateField((char*)originLoc, txtField->GetValue());
		case TyINT:
			return updateField((int32_t*)originLoc, txtField->GetValue());
		case TyBOOL:
			return updateField((bool*)originLoc, txtField->GetValue());
		case TyFLOAT:
			return updateField((float*)originLoc, txtField->GetValue());
		}
	}
	else if (dropDownField) {

		bool skipMsg = false;
		int32_t newVal = dropDownField->GetSelection();

		if (sectionID == 1) {
			 // record type selection
			//mainRef->ItemValues->setRecordType(mainRef->selectedItem, newVal);

			if (newVal == Items::EMPTY) {
				// empty selected, delete record and update selection menu
				mainRef->ItemValues->removeRecord(mainRef->selectedItem);
				mainRef->selector->SetString(mainRef->selectedItem, "ID: " + std::to_string(mainRef->selectedItem) + " | EMPTY SLOT");
				skipMsg = true;
			}
		}
		else {
			switch (*mainRef->ItemValues->getRecordType(mainRef->selectedItem)) {
			case Items::WEAPONS:
				// there is no 5 in this category, step up
				if (sectionID > 12) {
					// probably the "special" extra data section
					if (newVal == 1) newVal = 5;
				}
				else {
					newVal++;
					if (newVal == 5) newVal++;

					// also update field Q automatically.
					*(((int32_t*)originLoc) + 2) = newVal - 1;
				}
					
				break;
			case Items::GEAR:
				if (newVal != 0) newVal += 4;
				break;
			}
			
		}

		*(int32_t*)originLoc = newVal;
		mainRef->ItemValues->resetSubRecord(mainRef->selectedItem, sectionID, (int32_t*)originLoc);
		mainRef->drawDataFields(skipMsg);
		return true;
	}
	return false;
}

bool cMain::Section::updateField(int32_t* dest, wxString newValue)
{
	bool result;
	if (newValue.empty()) {
		*dest = 0;
		return true;
	}
	if (result = newValue.ToLong((long*)dest))	return result;
	wxMessageBox("Format error! Correcting to previous value.", "Warning Message", wxICON_WARNING | wxOK);
	return result;
}

bool cMain::Section::updateField(bool* dest, wxString newValue)
{
	bool result;
	if (newValue.empty()) {
		*dest = 0;
		return true;
	}
	int32_t output;
	if (result = newValue.ToLong((long*)&output)) {
		// normalize
		if (output >= 1) *dest = true;
		else if (output <= 0) *dest = false;
		txtField->ChangeValue(std::to_string(*dest));
		return result;
	}
	wxMessageBox("Format error! Correcting to previous value.", "Warning Message", wxICON_WARNING | wxOK);
	txtField->ChangeValue(std::to_string(*dest));
	return result;
}

bool cMain::Section::updateField(float* dest, wxString newValue)
{
	double tmp;
	bool out;

	if (newValue.empty()) {
		*dest = 0.0f;
		return true;
	}
	if (out = newValue.ToDouble(&tmp)) {
		*dest = (float)tmp;
		return out;
	}
	wxMessageBox("Format error! Correcting to previous value.", "Warning Message", wxICON_WARNING | wxOK);
	return out;
}

bool cMain::Section::updateField(char* dest, wxString newValue) {

	// make sure string does not get out of bounds
	if (newValue.size() > STRINGSIZE - 1) newValue = newValue.substr(0, STRINGSIZE - 1);
	// if label, also update the selection menu
	if (sectionID == 5) {
		mainRef->selector->SetString(mainRef->selectedItem, "ID: " + std::to_string(mainRef->selectedItem) + " | " + newValue);
	}
	memcpy(dest, newValue.data(), newValue.size() + 1);
	txtField->ChangeValue(newValue);
	return true;
}
