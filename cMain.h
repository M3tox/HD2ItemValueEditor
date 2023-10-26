#pragma once

#include <wx/wx.h>
#include <wx/collpane.h>
#include <wx/filedlg.h>
#include <wx/wfstream.h>

#include "Items.h"

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
			text = new wxStaticText(this, wxID_ANY, "H&D2 Item Value Editor, version 0.90\nSupports \"items.sav\" files\nProgrammed by Metox\nCopyright (c) 2023", wxPoint(10, 10));
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

public:
	wxButton* m_btn1 = nullptr;
	wxButton* m_btn2 = nullptr;

	wxTextCtrl* m_txt1 = nullptr;

	Items* ItemValues = nullptr;

	void OnMenuOpenFile(wxCommandEvent& evt);
	void OnMenuSaveFile(wxCommandEvent& evt);
	void OnMenuSaveFileAs(wxCommandEvent& evt);
	void OnMenuExit(wxCommandEvent& evt);
	void OnExportTxt(wxCommandEvent& evt);
	void OnAbout(wxCommandEvent& evt);
	void OnValueChange(wxCommandEvent& evt);
	void OnClose(wxCloseEvent& evt);
	void OnSelectionChange(wxCommandEvent& evt);
	void OnSelectRecChange(wxCommandEvent& evt);

	wxDECLARE_EVENT_TABLE();

private:

	wxMenuBar* m_MenuBar = nullptr;
	wxMenu* menuFile = nullptr;
	wxMenu* menuExtras = nullptr;

	wxPanel* topInfoPlane = nullptr;
	wxStaticText* infoText = nullptr;
	wxComboBox* selector = nullptr;
	wxComboBox* selectorRecord = nullptr;

	wxBoxSizer* vbp = nullptr;
	wxScrolledWindow* scrolleditemList = nullptr;

	uint32_t selectedItem = 0;
	std::vector<wxTextCtrl*>dataFields;
	

	wAbout* about = nullptr;

	bool dataChanged = false;

	wxString pathOpened;

	void refreshDisplayedData();
};

