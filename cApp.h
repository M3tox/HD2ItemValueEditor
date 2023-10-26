#pragma once
#include "cMain.h"

class cApp : public wxApp
{

private:
	cMain* mainFrame = nullptr;

public:
	virtual bool OnInit();
};
