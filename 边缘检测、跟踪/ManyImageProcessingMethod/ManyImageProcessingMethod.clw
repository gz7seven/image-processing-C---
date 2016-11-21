; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CManyImageProcessingMethodView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ManyImageProcessingMethod.h"
LastPage=0

ClassCount=6
Class1=CManyImageProcessingMethodApp
Class2=CManyImageProcessingMethodDoc
Class3=CManyImageProcessingMethodView
Class4=CMainFrame

ResourceCount=3
Resource1=IDR_MAINFRAME
Resource2=IDR_MANYIMTYPE
Class5=CChildFrame
Class6=CAboutDlg
Resource3=IDD_ABOUTBOX

[CLS:CManyImageProcessingMethodApp]
Type=0
HeaderFile=ManyImageProcessingMethod.h
ImplementationFile=ManyImageProcessingMethod.cpp
Filter=N

[CLS:CManyImageProcessingMethodDoc]
Type=0
HeaderFile=ManyImageProcessingMethodDoc.h
ImplementationFile=ManyImageProcessingMethodDoc.cpp
Filter=N

[CLS:CManyImageProcessingMethodView]
Type=0
HeaderFile=ManyImageProcessingMethodView.h
ImplementationFile=ManyImageProcessingMethodView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=ID_Math_Morph_Edge_Detection


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T


[CLS:CChildFrame]
Type=0
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp
Filter=M


[CLS:CAboutDlg]
Type=0
HeaderFile=ManyImageProcessingMethod.cpp
ImplementationFile=ManyImageProcessingMethod.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_PRINT_SETUP
Command4=ID_FILE_MRU_FILE1
Command5=ID_APP_EXIT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_APP_ABOUT
CommandCount=8

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_MANYIMTYPE]
Type=1
Class=CManyImageProcessingMethodView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_Image_Processing_Edge_Grads
Command12=ID_Image_Processing_Edge_Rober
Command13=ID_Image_Processing_Edge_Laplace
Command14=ID_Math_Morph_Dilate
Command15=ID_Math_Morph_Erode
Command16=ID_Math_Morph_Edge_Detection
Command17=ID_EDIT_UNDO
Command18=ID_EDIT_CUT
Command19=ID_EDIT_COPY
Command20=ID_EDIT_PASTE
Command21=ID_VIEW_TOOLBAR
Command22=ID_VIEW_STATUS_BAR
Command23=ID_WINDOW_NEW
Command24=ID_WINDOW_CASCADE
Command25=ID_WINDOW_TILE_HORZ
Command26=ID_WINDOW_ARRANGE
Command27=ID_APP_ABOUT
CommandCount=27

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

