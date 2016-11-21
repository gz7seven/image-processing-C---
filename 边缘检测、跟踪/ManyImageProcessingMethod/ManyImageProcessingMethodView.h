// ManyImageProcessingMethodView.h : interface of the CManyImageProcessingMethodView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MANYIMAGEPROCESSINGMETHODVIEW_H__9F721336_5A4D_492A_802B_CB17733E2697__INCLUDED_)
#define AFX_MANYIMAGEPROCESSINGMETHODVIEW_H__9F721336_5A4D_492A_802B_CB17733E2697__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UserDefinedClass.h"
enum {showOri,showEdge};
class CManyImageProcessingMethodView : public CView
{
protected: // create from serialization only
	CManyImageProcessingMethodView();
	DECLARE_DYNCREATE(CManyImageProcessingMethodView)

// Attributes
public:
	CManyImageProcessingMethodDoc* GetDocument();
    CImage_Class Image;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManyImageProcessingMethodView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CManyImageProcessingMethodView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CManyImageProcessingMethodView)
	afx_msg void OnMathMorphDilate();
	afx_msg void OnMathMorphErode();
	afx_msg void OnImageProcessingEdgeGrads();
	afx_msg void OnImageProcessingEdgeRober();
	afx_msg void OnImageProcessingEdgeLaplace();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnMathMorphEdgeDetection();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ManyImageProcessingMethodView.cpp
inline CManyImageProcessingMethodDoc* CManyImageProcessingMethodView::GetDocument()
   { return (CManyImageProcessingMethodDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANYIMAGEPROCESSINGMETHODVIEW_H__9F721336_5A4D_492A_802B_CB17733E2697__INCLUDED_)
