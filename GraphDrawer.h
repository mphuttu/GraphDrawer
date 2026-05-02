
// GraphDrawer.h : main header file for the GraphDrawer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CGraphDrawerApp:
// See GraphDrawer.cpp for the implementation of this class
//

class CGraphDrawerApp : public CWinApp
{
public:
	CGraphDrawerApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CGraphDrawerApp theApp;
