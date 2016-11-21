// ManyImageProcessingMethod.h : main header file for the MANYIMAGEPROCESSINGMETHOD application
//

#if !defined(AFX_MANYIMAGEPROCESSINGMETHOD_H__F9E040C3_A979_42CF_B9BB_9E3C131E8203__INCLUDED_)
#define AFX_MANYIMAGEPROCESSINGMETHOD_H__F9E040C3_A979_42CF_B9BB_9E3C131E8203__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CManyImageProcessingMethodApp:
// See ManyImageProcessingMethod.cpp for the implementation of this class
//

class CManyImageProcessingMethodApp : public CWinApp
{
public:
	CManyImageProcessingMethodApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManyImageProcessingMethodApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CManyImageProcessingMethodApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANYIMAGEPROCESSINGMETHOD_H__F9E040C3_A979_42CF_B9BB_9E3C131E8203__INCLUDED_)
