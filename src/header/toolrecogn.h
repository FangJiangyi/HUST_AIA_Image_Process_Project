// Cross.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CCrossApp:
// See Cross.cpp for the implementation of this class
//

class CtoolrecognApp : public CWinApp
{
public:
	CtoolrecognApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	afx_msg void OnOpenlocalcam();
};

extern CtoolrecognApp theApp;