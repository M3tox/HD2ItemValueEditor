#pragma once

#include <wx/wx.h>
#include <wx/collpane.h>
#include <wx/filedlg.h>
#include <wx/wfstream.h>

#include "Items.h"

static wxScrolledWindow* scrolleditemList;

class cMain : public wxFrame
{
public:
	cMain();
	~cMain();

	class wAbout : public wxFrame
	{
	public:
		wAbout(cMain* ref) : wxFrame(ref, 11000, "About", wxDefaultPosition, wxSize(250, 150), wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX) | wxFRAME_FLOAT_ON_PARENT)
		{
			Centre();
			SetBackgroundColour(wxColor(0xFF, 0xFF, 0xFF));
			text = new wxStaticText(this, wxID_ANY, "H&D2 Item Value Editor, version 0.96\nSupports \"items.sav\" files\nProgrammed by Metox\nCopyright (c) 2024", wxPoint(10, 10));
			okBtn = new wxButton(this, 11001, "OK", wxPoint(100, 75));
		}

		void onOk(wxCommandEvent& evt) {
			Close();
		}

		wxDECLARE_EVENT_TABLE();

	private:
		wxStaticText* text = nullptr;
		wxButton* okBtn = nullptr;
	};


	void OnMenuOpenFile(wxCommandEvent& evt);
	void OnMenuSaveFile(wxCommandEvent& evt);
	void OnMenuSaveFileAs(wxCommandEvent& evt);
	void OnMenuExit(wxCommandEvent& evt);
	void OnExportTxt(wxCommandEvent& evt);
	void OnAbout(wxCommandEvent& evt);
	void OnValueChange(wxCommandEvent& evt);
	void OnClose(wxCloseEvent& evt);
	void OnSelectionChange(wxCommandEvent& evt);

	wxDECLARE_EVENT_TABLE();

private:

	enum {
		TyCHAR,
		TyINT,
		TyFLOAT,
		TyBOOL
	};

	struct Section {
		Section(cMain* mainRef, wxBoxSizer* Area, const char* label, wxString val, uint16_t originType, void* originLoc);
		Section(cMain* mainRef, wxBoxSizer* Area, const char* label, const std::vector<wxString>& strings, int32_t& select, int32_t altSelect = -1);
		~Section() {
			Area->Remove(colSizer);

			delete number;
			delete txtLabel;
			delete txtField;

			delete dropDownField;
		}
		
		bool updateInternalData();

		wxStaticText* number;
		wxStaticText* txtLabel;
		wxTextCtrl* txtField;
		// alternative
		wxComboBox* dropDownField;
	private:
		wxBoxSizer* colSizer;
		wxBoxSizer* Area;

		cMain* mainRef;
		uint16_t sectionID;
		uint16_t originType;
		void* originLoc;

		bool updateField(int32_t* dest, wxString newValue);
		bool updateField(bool* dest, wxString newValue);
		bool updateField(float* dest, wxString newValue);
		bool updateField(char* dest, wxString newValue);
	};

	wxButton* m_btn1 = nullptr;
	wxButton* m_btn2 = nullptr;

	wxTextCtrl* m_txt1 = nullptr;

	Items* ItemValues = nullptr;

	wxMenuBar* m_MenuBar = nullptr;
	wxMenu* menuFile = nullptr;
	wxMenu* menuExtras = nullptr;

	wxPanel* topInfoPlane = nullptr;
	wxStaticText* infoText = nullptr;
	wxComboBox* selector = nullptr;


	wxBoxSizer* vbp = nullptr;


	uint32_t selectedItem = 0;
	//std::vector<wxTextCtrl*>defaultFields;
	//Section* mainSections[12];
	std::vector<Section*>sections;

	wxBoxSizer* primaryArea = nullptr;


	wAbout* about = nullptr;

	bool dataChanged = false;

	wxString pathOpened;

	void drawDataFields(bool skipMsg = false);
	void refreshDataFields(Items::ItemBase* itemRef);

	void addField(float& val);
	void addField(int32_t& val);
	void addField(bool& val);
	void addField(const char*& val);
	void addField(const char* label, float& val);
	void addField(const char* label, int32_t& val);
	void addField(const char* label, bool& val);
	void addField(const char* label, const char* val);
	void addField(const char* label, wxString val, uint16_t originType, void* originLoc);

	void addDropDown(const char* label, const std::vector<wxString>& strings, int32_t& select, int32_t altSelect = -1);


};

