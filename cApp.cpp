#include "cApp.h"

wxIMPLEMENT_APP(cApp);


bool cApp::OnInit() {

	mainFrame = new cMain();
	mainFrame->Show();

	return true;
}
