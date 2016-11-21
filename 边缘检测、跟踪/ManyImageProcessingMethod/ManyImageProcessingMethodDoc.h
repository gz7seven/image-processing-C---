// ManyImageProcessingMethodDoc.h : interface of the CManyImageProcessingMethodDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MANYIMAGEPROCESSINGMETHODDOC_H__7CF25FAE_2974_4604_92E3_C44654710868__INCLUDED_)
#define AFX_MANYIMAGEPROCESSINGMETHODDOC_H__7CF25FAE_2974_4604_92E3_C44654710868__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CManyImageProcessingMethodDoc : public CDocument
{
protected: // create from serialization only
	CManyImageProcessingMethodDoc();
	DECLARE_DYNCREATE(CManyImageProcessingMethodDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManyImageProcessingMethodDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CManyImageProcessingMethodDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CManyImageProcessingMethodDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANYIMAGEPROCESSINGMETHODDOC_H__7CF25FAE_2974_4604_92E3_C44654710868__INCLUDED_)
