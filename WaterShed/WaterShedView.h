// WaterShedView.h : interface of the CWaterShedView class
// Download by http://www.codefans.net
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WATERSHEDVIEW_H__26511C8F_CCD4_4B5F_8B6E_468807A22287__INCLUDED_)
#define AFX_WATERSHEDVIEW_H__26511C8F_CCD4_4B5F_8B6E_468807A22287__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "WaterShedDoc.h"

class CWaterShedView : public CScrollView
{
protected: // create from serialization only
	CWaterShedView();
	DECLARE_DYNCREATE(CWaterShedView)

// Attributes
public:
	CWaterShedDoc* GetDocument();

// Operations
public:
	INT sX, sY;//����λ�ã�
	INT iX, iY;//ͼ����ʾλ��, ���ĸ�������������ǰ���λ��ת��Ϊͼ��դ��λ�ã�
	INT cX, cY;//��ǰ�����ͼ���е�λ�ã�
	INT neiWidth, neiHeight;//�����ߣ�
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaterShedView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWaterShedView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWaterShedView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in WaterShedView.cpp
inline CWaterShedDoc* CWaterShedView::GetDocument()
   { return (CWaterShedDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATERSHEDVIEW_H__26511C8F_CCD4_4B5F_8B6E_468807A22287__INCLUDED_)
