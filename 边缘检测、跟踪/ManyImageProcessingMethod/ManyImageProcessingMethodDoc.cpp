// ManyImageProcessingMethodDoc.cpp : implementation of the CManyImageProcessingMethodDoc class
//

#include "stdafx.h"
#include "ManyImageProcessingMethod.h"

#include "ManyImageProcessingMethodDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CManyImageProcessingMethodDoc

IMPLEMENT_DYNCREATE(CManyImageProcessingMethodDoc, CDocument)

BEGIN_MESSAGE_MAP(CManyImageProcessingMethodDoc, CDocument)
	//{{AFX_MSG_MAP(CManyImageProcessingMethodDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManyImageProcessingMethodDoc construction/destruction

CManyImageProcessingMethodDoc::CManyImageProcessingMethodDoc()
{
	// TODO: add one-time construction code here

}

CManyImageProcessingMethodDoc::~CManyImageProcessingMethodDoc()
{
}

BOOL CManyImageProcessingMethodDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CManyImageProcessingMethodDoc serialization

void CManyImageProcessingMethodDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CManyImageProcessingMethodDoc diagnostics

#ifdef _DEBUG
void CManyImageProcessingMethodDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CManyImageProcessingMethodDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CManyImageProcessingMethodDoc commands
