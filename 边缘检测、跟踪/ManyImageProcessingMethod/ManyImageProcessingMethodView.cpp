// ManyImageProcessingMethodView.cpp : implementation of the CManyImageProcessingMethodView class
//

#include "stdafx.h"
#include "ManyImageProcessingMethod.h"
#include "ManyImageProcessingMethodDoc.h"
#include "ManyImageProcessingMethodView.h"

//#include "UserDefinedClass.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CManyImageProcessingMethodView

IMPLEMENT_DYNCREATE(CManyImageProcessingMethodView, CView)

BEGIN_MESSAGE_MAP(CManyImageProcessingMethodView, CView)
	//{{AFX_MSG_MAP(CManyImageProcessingMethodView)
	ON_COMMAND(ID_Math_Morph_Dilate, OnMathMorphDilate)
	ON_COMMAND(ID_Math_Morph_Erode, OnMathMorphErode)
	ON_COMMAND(ID_Image_Processing_Edge_Grads, OnImageProcessingEdgeGrads)
	ON_COMMAND(ID_Image_Processing_Edge_Rober, OnImageProcessingEdgeRober)
	ON_COMMAND(ID_Image_Processing_Edge_Laplace, OnImageProcessingEdgeLaplace)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_Math_Morph_Edge_Detection, OnMathMorphEdgeDetection)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManyImageProcessingMethodView construction/destruction

CManyImageProcessingMethodView::CManyImageProcessingMethodView()
{
	// TODO: add construction code here

}

CManyImageProcessingMethodView::~CManyImageProcessingMethodView()
{
}

BOOL CManyImageProcessingMethodView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CManyImageProcessingMethodView drawing

void CManyImageProcessingMethodView::OnDraw(CDC* pDC)
{
	CManyImageProcessingMethodDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	switch(Image.iShowFlag)
	{
		case showOri:
			Image.ShowOri(pDC);
			break;
		case showEdge:
			Image.ShowEdge(pDC);
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CManyImageProcessingMethodView printing

BOOL CManyImageProcessingMethodView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CManyImageProcessingMethodView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CManyImageProcessingMethodView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CManyImageProcessingMethodView diagnostics

#ifdef _DEBUG
void CManyImageProcessingMethodView::AssertValid() const
{
	CView::AssertValid();
}

void CManyImageProcessingMethodView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CManyImageProcessingMethodDoc* CManyImageProcessingMethodView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CManyImageProcessingMethodDoc)));
	return (CManyImageProcessingMethodDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CManyImageProcessingMethodView message handlers

void CManyImageProcessingMethodView::OnMathMorphDilate() //膨胀
{ /*   
	CString str;
	str.Format("tst1.a=%d,tst1.b=%d",tst1.a,tst1.b);
	AfxMessageBox(str);*/

	Mask msk;
	int y=0,x=0,kk,i;
	msk.cols=3;msk.rows =3;
	msk.kern =(BYTE *) malloc(msk.cols *msk.rows );

//	    0 1 0
//	    1 1 1
//		0 1 0 	
	for(y=0;y<msk.cols ;y++)	//建立模板
		for(x=0;x<msk.rows ;x++)
		{
			kk=y*msk.rows +x;

			if(kk % 2 !=0)
				msk.kern [kk]=black;
			else if(kk==4)
				msk.kern [kk]=black;
			else
				msk.kern [kk]=white;
		}
	for(int i=0;i<Image.Imagedata[0].ImageHeight*Image.Imagedata[0].ImageWidth;i++)
	    Image.Imagedata[1].data[i]=Image.Imagedata[0].data[i];
	Image.Math_Morph_Dilate(msk,Image.Imagedata[1]);
	for( i=0;i<Image.Imagedata[0].ImageHeight*Image.Imagedata[0].ImageWidth;i++)
	    Image.Imagedata[0].data[i]=Image.Imagedata[1].data[i];
	Image.iShowFlag=showOri;
	Invalidate(true);	
	
}

void CManyImageProcessingMethodView::OnMathMorphErode() //腐蚀
{
	// 对图像进行腐蚀运算
    Mask msk;
	int y,x,kk,i;
	msk.cols=3;msk.rows =3;
	msk.kern =(BYTE *) malloc(msk.cols *msk.rows );


//	    0 1 0
//	    1 1 1
//		0 1 0 	
	for(y=0;y<msk.cols ;y++)	//建立模板
		for(x=0;x<msk.rows ;x++)
		{
			kk=y*msk.rows +x;

			if(kk % 2 !=0)
				msk.kern [kk]=black;
			else if(kk==4)
				msk.kern [kk]=black;
			else
				msk.kern [kk]=white;
		}
/*
//    0 1 0
//    0 1 1
//    0 0 0
	for(y=0;y<msk.cols ;y++)//建立模板
		for(x=0;x<msk.rows ;x++)
		{
			kk=y*msk.rows +x;
			msk.kern [kk]=back_color;
		}
//	msk.kern [1]=fore_color; 
	msk.kern [4]=msk.kern [5]=fore_color;
*/
//	Math_Image.Image_Process_Binary_Erosion(msk,Math_Image.Imagedata[0],Math_Image.ImageHeight,Math_Image.ImageWidth,0,Math_Image.ImageHeight,0,Math_Image.ImageWidth);
    for(int i=0;i<Image.Imagedata[0].ImageHeight*Image.Imagedata[0].ImageWidth;i++)
	    Image.Imagedata[2].data[i]=Image.Imagedata[0].data[i];
	Image.Math_Morph_Erode(msk,Image.Imagedata[2]);
	for( i=0;i<Image.Imagedata[0].ImageHeight*Image.Imagedata[0].ImageWidth;i++)
	    Image.Imagedata[0].data[i]=Image.Imagedata[2].data[i];
	Image.iShowFlag=showOri;
	Invalidate(true);
	
}

void CManyImageProcessingMethodView::OnImageProcessingEdgeGrads() 
{
	// TODO: Add your command handler code here
	
}

void CManyImageProcessingMethodView::OnImageProcessingEdgeRober() 
{
	// TODO: Add your command handler code here
	
}

void CManyImageProcessingMethodView::OnImageProcessingEdgeLaplace() 
{
	// TODO: Add your command handler code here
	Image.Edge_Laplace(Image.Imagedata[0],Image.Edgedata);
	Image.iShowFlag=showEdge;
	Invalidate(true);	
}

void CManyImageProcessingMethodView::OnFileOpen() //打开图像文件
{
	CFileDialog FileDialog(TRUE);
	if(FileDialog.DoModal()==IDOK)
		Image.Load(FileDialog.GetFileName());
	Image.iShowFlag=showOri;
	Invalidate(true);			
}

void CManyImageProcessingMethodView::OnFileSave() //保存图像文件
{
	CFileDialog FileDialog(FALSE,"bmp",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Bitmap Files(*.bmp)|*.bmp||");
	if(FileDialog.DoModal()==IDOK)
		Image.Save(FileDialog.GetFileName());	
	
}

void CManyImageProcessingMethodView::OnMathMorphEdgeDetection() 
{
	Mask msk;
	int y=0,x=0,kk,i;
	msk.cols=3;msk.rows =3;
	msk.kern =(BYTE *) malloc(msk.cols *msk.rows );

//	    0 1 0
//	    1 1 1
//		0 1 0 	
	for(y=0;y<msk.cols ;y++)	//建立模板
		for(x=0;x<msk.rows ;x++)
		{
			kk=y*msk.rows +x;

			if(kk % 2 !=0)
				msk.kern [kk]=black;
			else if(kk==4)
				msk.kern [kk]=black;
			else
				msk.kern [kk]=white;
		}

	for(int i=0;i<Image.Imagedata[0].ImageHeight*Image.Imagedata[0].ImageWidth;i++)
	    Image.Imagedata[1].data[i]=Image.Imagedata[0].data[i];
	for(i=0;i<Image.Imagedata[0].ImageHeight*Image.Imagedata[0].ImageWidth;i++)
	    Image.Imagedata[2].data[i]=Image.Imagedata[0].data[i];

	Image.Math_Morph_Dilate(msk,Image.Imagedata[1]);
	Image.Math_Morph_Erode(msk,Image.Imagedata[2]);
  
	for( i=0;i<Image.Imagedata[0].ImageHeight*Image.Imagedata[0].ImageWidth;i++)
	    Image.Imagedata[0].data[i]=abs(Image.Imagedata[1].data[i]-Image.Imagedata[2].data[i]);
    Image.iShowFlag=showOri;
	Invalidate(true);	
}
