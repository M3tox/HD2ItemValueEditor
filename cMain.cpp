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
EVT_COMBOBOX(SELECTREC, cMain::OnSelectRecChange)
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
	menuExtras->Append(EXPORTCSV, "Export as CSV file")->Enable(false);
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

	vbp = new wxBoxSizer(wxVERTICAL);

	vbp->Add(topInfoPlane, 0, wxGROW);

	this->SetSizer(vbp);

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
		wxString name("ID: " + std::to_string(i) + " | "+ ItemValues->getItemName(i));
		selector->AppendString(name);
	}
	selector->Select(0);

	// update fields
	refreshDisplayedData();

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
	refreshDisplayedData();
	evt.Skip();
}

void cMain::OnSelectRecChange(wxCommandEvent& evt)
{
	;
	if (!ItemValues->setRecordType(selectedItem, selectorRecord->GetSelection())) {
		wxMessageBox("ERROR: Invalid selection!", "Error Message", wxICON_ERROR | wxOK);
		return;
	}
	dataChanged = true;
	refreshDisplayedData();
	evt.Skip();
}

void cMain::refreshDisplayedData() {

	
	std::string descr;
	std::string val;

	if (scrolleditemList) {
		// IF its the exact same data structure, recycle the current one and only update the data fields!
		selectorRecord->Select(ItemValues->getRecordType(selectedItem));
		// update only data fields

		const uint32_t sectionCount = dataFields.size();
		for (uint32_t i = 0; i < sectionCount; i++) {
			if (!ItemValues->getFieldInformation(selectedItem, i, val)) {
				wxMessageBox("ERROR: Item index out of bounds!", "Error Message", wxICON_ERROR | wxOK);
				break;
			}

			dataFields.at(i)->ChangeValue(val);
			if (ItemValues->isFieldActive(selectedItem, i))
				dataFields.at(i)->Enable();
			else
				dataFields.at(i)->Disable();
		}
		vbp->Layout();
		return;
	}


	scrolleditemList = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	scrolleditemList->Hide();

	vbp->Add(scrolleditemList, 1, wxEXPAND);


	wxBoxSizer* itemSizer = new wxBoxSizer(wxVERTICAL);

	// add selector for record type via dropdown
	selectorRecord = new wxComboBox(scrolleditemList, SELECTREC, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY);
	selectorRecord->AppendString("AMMO");
	selectorRecord->AppendString("ANIMATED");
	selectorRecord->AppendString("STATIC");
	selectorRecord->AppendString("EMPTY");
	selectorRecord->Select(ItemValues->getRecordType(selectedItem));

	wxBoxSizer* colSizer0 = new wxBoxSizer(wxHORIZONTAL);
	colSizer0->AddSpacer(10);
	colSizer0->Add(new wxStaticText(scrolleditemList, wxID_ANY, "Record Type: "), 2, wxGROW | wxALL, 2);
	colSizer0->Add(selectorRecord, 2, wxGROW | wxALL, 2);

	itemSizer->Add(colSizer0, 1, wxEXPAND);

	// add a series of widgets
	const uint32_t sectionCount = ItemValues->getFieldCount();

	dataFields.reserve(sectionCount);
	for (uint32_t i = 0; i < sectionCount; i++) {

		wxBoxSizer* colSizer = new wxBoxSizer(wxHORIZONTAL);
		colSizer->AddSpacer(10);
		
		if (!ItemValues->getFieldInformation(selectedItem, i, descr, val)) {
			wxMessageBox("Item index out of bounds!", "Error Message", wxICON_ERROR | wxOK);
			break;
		}

		colSizer->Add(new wxStaticText(scrolleditemList, wxID_ANY, std::to_string(i+1)), 1, wxGROW | wxALL, 2);
		colSizer->Add(new wxStaticText(scrolleditemList, wxID_ANY, descr), 3, wxGROW | wxALL, 2);

		dataFields.emplace_back(new wxTextCtrl(scrolleditemList, VALUECHNG+ i, val));
		if (!ItemValues->isFieldActive(selectedItem, i))
			dataFields.back()->Disable();
		colSizer->Add(dataFields.back(), 3, wxGROW | wxALL, 2);
		
		itemSizer->Add(colSizer, 1, wxEXPAND);
	}


	scrolleditemList->SetSizer(itemSizer);

	scrolleditemList->SetScrollRate(20, 20);
	dataChanged = false;

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
	// cut down strings if too long
	if (field < 5) {
		if (dataFields.at(field)->GetValue().ToStdString().size() > 19)
			dataFields.at(field)->SetValue(dataFields.at(field)->GetValue().ToStdString().substr(0,19));	
	}

	// short reference shall not be longer than 7 characters
	if (field == 32) {
		if (dataFields.at(field)->GetValue().ToStdString().size() > 7)
			dataFields.at(field)->SetValue(dataFields.at(field)->GetValue().ToStdString().substr(0, 7));
	}

	if (!ItemValues->setFieldInformation(selectedItem, field, dataFields.at(field)->GetValue().ToStdString())) {
		wxMessageBox("Invalid format in field " + std::to_string(field+1) + "! Loading previous value.", "Warning Message", wxICON_WARNING | wxOK);
		std::string val;
		ItemValues->getFieldInformation(selectedItem, field, val);
		dataFields.at(field)->SetValue(val);
	}

	dataChanged = true;
	evt.Skip();
}
