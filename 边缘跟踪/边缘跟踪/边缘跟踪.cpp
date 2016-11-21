//////////////////////////////////////////////////////////////
//Name:dither.c
//Purpose: Pattern and Dithering
//Author: phoenix, CS, TshingHua, Beijing, P.R.C.
//Email: bjlufengjun@www.163.net or lufengjun@hotmail.com
//Date:April 3, 1998

//header file
#include "Bmp.h"
#include "memory.h"
#include "math.h"
#include "stdio.h"
//macro definition
#define WIDTHBYTES(i)    ((i+31)/32*4)
//function declaration
int PASCAL WinMain (HANDLE, HANDLE, LPSTR, int);
LRESULT CALLBACK MainWndProc(HWND , UINT,WPARAM, LPARAM);
BOOL LoadBmpFile (HWND hWnd,char *BmpFileName);
BOOL LimbPatternM3(HWND hWnd);
BOOL LimbPatternM4(HWND hWnd);
BOOL Steinberg(HWND hWnd);
BOOL Bmp2Txt(HWND hWnd);
//global variable declaration
BITMAPFILEHEADER   bf;
BITMAPINFOHEADER   bi;
HPALETTE           hPalette=NULL;
HBITMAP            hBitmap=NULL;
HGLOBAL            hImgData=NULL;
DWORD              NumColors;
DWORD              LineBytes;
HINSTANCE          ghInst;
DWORD              ImgWidth=0 , ImgHeight=0;
//template array
BYTE BayerPattern[8][8]={0,32,8,40,2,34,10,42,
						 48,16,56,24,50,18,58,26,
						 12,44,4,36,14,46,6,38,
						 60,28,52,20,62,30,54,22,
						 3,35,11,43,1,33,9,41,
						 51,19,59,27,49,17,57,25,
						 15,47,7,39,13,45,5,37,
						 63,31,55,23,61,29,53,21};
static char ch[95]=	{
					' ',
					'`','1','2','3','4','5','6','7','8','9','0','-','=','\\',
					'q','w','e','r','t','y','u','i','o','p','[',']',
					'a','s','d','f','g','h','j','k','l',';','\'',
					'z','x','c','v','b','n','m',',','.','/',
					'~','!','@','#','$','%','^','&','*','(',')','_','+','|',
					'Q','W','E','R','T','Y','U','I','O','P','{','}',
					'A','S','D','F','G','H','J','K','L',':','"',
					'Z','X','C','V','B','N','M','<','>','?'
					};
static int  gr[95]= {
					 0,
					 7,22,28,31,31,27,32,22,38,32,40, 6,12,20,38,32,26,20,24,40,
  					 29,24,28,38,32,32,26,22,34,24,44,33,32,32,24,16, 6,22,26,22,
					 26,34,29,35,10, 6,20,14,22,47,42,34,40,10,35,21,22,22,16,14,
					 26,40,39,29,38,22,28,36,22,36,30,22,22,36,26,36,25,34,38,24,
					 36,22,12,12,26,30,30,34,39,42,41,18,18,22
					 };
///////////////////////////////////////////////////////////
int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
		    LPSTR lpszCmdLine, int nCmdShow)
{
	MSG       msg;
	WNDCLASS  wndclass;
	HWND      hWnd;

	ghInst=hInstance;
	if ( ! hPrevInstance ){
		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = MainWndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInstance;
		wndclass.hIcon = LoadIcon (NULL, IDI_APPLICATION);
		wndclass.hCursor = LoadCursor (NULL, IDC_ARROW);
		wndclass.hbrBackground = GetStockObject (WHITE_BRUSH);
		wndclass.lpszMenuName =  "BMPMENU";
		wndclass.lpszClassName = "phoenix ip system";
    }

    if ( ! RegisterClass (&wndclass) )
	    return FALSE;

	hWnd = CreateWindow ("phoenix ip system","Pattern and Dithering",
						 WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,
						 CW_USEDEFAULT,	CW_USEDEFAULT, NULL,NULL,
						 hInstance,	NULL);
	if (!hWnd)
		return FALSE;
	ShowWindow (hWnd, SW_SHOWMAXIMIZED);
	UpdateWindow (hWnd);
	
	while ( GetMessage (&msg, NULL, 0, 0) ){
		TranslateMessage (&msg);
		DispatchMessage (&msg);
    }

	return msg.wParam;
}
////////////////////////////////////////////////////////////////
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam)
{
    static  HDC    hDC,hMemDC;
    PAINTSTRUCT    ps;

    switch (message){
    case WM_PAINT:
	{         
		hDC = BeginPaint(hWnd, &ps);
		if (hBitmap)
		{ 
			hMemDC = CreateCompatibleDC(hDC);
			if (hPalette)
			{           
				SelectPalette (hDC, hPalette, FALSE); 
				SelectPalette (hMemDC, hPalette, FALSE);
				RealizePalette (hDC);
			}   
			SelectObject(hMemDC, hBitmap); 
			BitBlt(hDC, 0, 0, ImgWidth,ImgHeight, hMemDC, 0, 0, SRCCOPY);
			DeleteDC(hMemDC);
		}
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY: //注意释放内存和位图，调色板句柄
        if(hBitmap!=NULL)
	        DeleteObject(hBitmap);
	    
		if(hPalette!=NULL)                     
	        DeleteObject(hPalette);
		
		if(hImgData!=NULL){
			GlobalUnlock(hImgData);
			GlobalFree(hImgData);
		}	        
	    PostQuitMessage (0);
	    return 0;
	case WM_COMMAND:
	    switch (wParam){
	    case IDM_LOADBMP: 
			//注意重新分配内存和调色板，位图句柄时，先释放原来的
	        if(hBitmap!=NULL){     
				DeleteObject(hBitmap);
	            hBitmap=NULL;
	        }
	        if(hPalette!=NULL){                     
				DeleteObject(hPalette);
	            hPalette=NULL;
	        }
			if(hImgData!=NULL){
				GlobalUnlock(hImgData);
				GlobalFree(hImgData);  
				hImgData=NULL;
			}	        
			if(LoadBmpFile(hWnd,"c:\\test.bmp")) //成功，则重画窗口
                InvalidateRect(hWnd,NULL,TRUE);
	        break;
		case IDM_LIMBPATTERNM3:
			if(hImgData!=NULL){
				if(LimbPatternM3(hWnd))
					InvalidateRect(hWnd,NULL,TRUE);
			}
			else
				MessageBox(hWnd,"File not loaded yet!","Error Message",MB_OK|MB_ICONEXCLAMATION);
			break;
		case IDM_LIMBPATTERNM4:
			if(hImgData!=NULL){
				if(LimbPatternM4(hWnd))
					InvalidateRect(hWnd,NULL,TRUE);
			}
			else
				MessageBox(hWnd,"File not loaded yet!","Error Message",MB_OK|MB_ICONEXCLAMATION);
			break;
		case IDM_STEINBERG:
			if(hImgData!=NULL){
				if(Steinberg(hWnd));
					InvalidateRect(hWnd,NULL,TRUE);
			}
			else
				MessageBox(hWnd,"File not loaded yet!","Error Message",MB_OK|MB_ICONEXCLAMATION);
			break;
		case IDM_BMP2TXT:
			if(hImgData!=NULL){
				if(Bmp2Txt(hWnd))
			    	MessageBox(hWnd,"Finished!","Success Message",MB_OK);
			}
			else
				MessageBox(hWnd,"File not loaded yet!","Error Message",MB_OK|MB_ICONEXCLAMATION);
			break;
		case IDM_EXIT:
	        SendMessage(hWnd,WM_DESTROY,0,0L);
	        break;
	    }
	break;                
    }
    return DefWindowProc (hWnd, message, wParam, lParam);
}
////////////////////////////////////////////////////////////////
BOOL LoadBmpFile (HWND hWnd,char *BmpFileName)
{   
    HFILE              hf;
    LPBITMAPINFOHEADER lpImgData;
    LOGPALETTE         *pPal;
    LPRGBQUAD          lpRGB;
    HPALETTE           hPrevPalette; 
    HDC                hDc;
	HLOCAL             hPal;
	DWORD 		       ImgSize;
	DWORD              i;

    if((hf=_lopen(BmpFileName,OF_READ))==HFILE_ERROR){
        MessageBox(hWnd,"File c:\\test.bmp not found!","Error Message",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
	}
	_lread(hf,(LPSTR)&bf,sizeof(BITMAPFILEHEADER)); 
	_lread(hf,(LPSTR)&bi,sizeof(BITMAPINFOHEADER));
	ImgWidth=bi.biWidth;
	ImgHeight=bi.biHeight;
	LineBytes=(DWORD)WIDTHBYTES(bi.biWidth*bi.biBitCount);
	ImgSize=(DWORD)LineBytes*bi.biHeight;
    if(bi.biClrUsed!=0)
		NumColors=(DWORD)bi.biClrUsed;
	else
        switch(bi.biBitCount){
	       	case 1:
        	    NumColors=2;
        	    break;
        	case 4:
        	    NumColors=16;
        	    break;
        	case 8:
        	    NumColors=256;
        	    break;
        	case 24:
        	    NumColors=0;
        	    break;
              default:
                  MessageBox(hWnd,"Invalid color numbers!","Error Message",MB_OK|MB_ICONEXCLAMATION);
                  _lclose(hf);
                  return FALSE; 
        }
	if(bf.bfOffBits!=(DWORD)(NumColors*sizeof(RGBQUAD)+sizeof(BITMAPFILEHEADER)
							+sizeof(BITMAPINFOHEADER)))
	{
    	MessageBox(hWnd,"Invalid color numbers!","Error Message" ,MB_OK|
	               MB_ICONEXCLAMATION);
		_lclose(hf);
		return FALSE; 
	}
	bf.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+NumColors*sizeof(RGBQUAD)+ImgSize;
	if((hImgData=GlobalAlloc(GHND,(DWORD)(sizeof(BITMAPINFOHEADER)+
						     NumColors*sizeof(RGBQUAD)+ImgSize)))==NULL)
	{
    	MessageBox(hWnd,"Error alloc memory!","ErrorMessage",MB_OK|
                   MB_ICONEXCLAMATION);
	    _lclose(hf);
		return FALSE;
	}
  
	lpImgData=(LPBITMAPINFOHEADER)GlobalLock(hImgData); 
    _llseek(hf,sizeof(BITMAPFILEHEADER),FILE_BEGIN);
	_hread(hf,(char *)lpImgData,(long)sizeof(BITMAPINFOHEADER)
           +(long)NumColors*sizeof(RGBQUAD)+ImgSize);
	_lclose(hf);
    if(NumColors!=0)
	{                    
	    hPal=LocalAlloc(LHND,sizeof(LOGPALETTE) + NumColors* sizeof(PALETTEENTRY));
	    pPal =(LOGPALETTE *)LocalLock(hPal);
	    pPal->palNumEntries =(WORD) NumColors;
		pPal->palVersion    = 0x300;
	    lpRGB = (LPRGBQUAD)((LPSTR)lpImgData + (DWORD)sizeof(BITMAPINFOHEADER));
		for (i = 0; i < NumColors; i++) {
     		pPal->palPalEntry[i].peRed=lpRGB->rgbRed;
			pPal->palPalEntry[i].peGreen=lpRGB->rgbGreen;
			pPal->palPalEntry[i].peBlue=lpRGB->rgbBlue;
			pPal->palPalEntry[i].peFlags=(BYTE)0;
			lpRGB++;
		}
		hPalette=CreatePalette(pPal);
		LocalUnlock(hPal);
		LocalFree(hPal);
	}
	hDc=GetDC(hWnd);
	if(hPalette){
        hPrevPalette=SelectPalette(hDc,hPalette,FALSE);
		RealizePalette(hDc);
	}
	hBitmap=CreateDIBitmap(hDc,	(LPBITMAPINFOHEADER)lpImgData, (LONG)CBM_INIT,
				(LPSTR)lpImgData+sizeof(BITMAPINFOHEADER) +NumColors*sizeof(RGBQUAD),
   				(LPBITMAPINFO)lpImgData, DIB_RGB_COLORS);
	if(hPalette && hPrevPalette){
		SelectPalette(hDc,hPrevPalette,FALSE);
		RealizePalette(hDc);
	}
 
	ReleaseDC(hWnd,hDc);
	GlobalUnlock(hImgData);
	return TRUE; 
}
////////////////////////////////////////////////////////////////
BOOL LimbPatternM3(HWND hWnd)
{
	DWORD              BufSize;
    LPBITMAPINFOHEADER lpImgData;
	LPSTR              lpPtr;
	HLOCAL             hTempImgData;
	LPBITMAPINFOHEADER lpTempImgData;
	LPSTR              lpTempPtr;
	HDC                hDc;
	HFILE              hf;
	LONG               x,y;
	unsigned char      num;

	BufSize=bf.bfSize-sizeof(BITMAPFILEHEADER);

	if((hTempImgData=LocalAlloc(LHND,BufSize))==NULL)
    {
    	MessageBox(hWnd,"Error alloc memory!","Error Message",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }

    lpImgData=(LPBITMAPINFOHEADER)GlobalLock(hImgData);    
	lpTempImgData=(LPBITMAPINFOHEADER)LocalLock(hTempImgData);

	//copy image data
	memcpy(lpTempImgData,lpImgData,BufSize);

	for(y=0;y<bi.biHeight;y++){
		lpPtr=(char *)lpImgData+(BufSize-LineBytes-y*LineBytes);
		lpTempPtr=(char *)lpTempImgData+(BufSize-LineBytes-y*LineBytes);
		for(x=0;x<bi.biWidth;x++){
			num=(unsigned char)*lpPtr++;
			if ( (num>>2) > BayerPattern[y&7][x&7])
				*(lpTempPtr++)=(unsigned char)255;
			else *(lpTempPtr++)=(unsigned char)0;
			}
	}
	
    if(hBitmap!=NULL)
	    DeleteObject(hBitmap);
	hDc=GetDC(hWnd);	
	hBitmap=CreateDIBitmap(hDc,	(LPBITMAPINFOHEADER)lpTempImgData, (LONG)CBM_INIT,
				(LPSTR)lpTempImgData+sizeof(BITMAPINFOHEADER) +NumColors*sizeof(RGBQUAD),
   				(LPBITMAPINFO)lpTempImgData, DIB_RGB_COLORS);

    hf=_lcreat("c:\\limbm3.bmp",0);
	_lwrite(hf,(LPSTR)&bf,sizeof(BITMAPFILEHEADER)); 
	_lwrite(hf,(LPSTR)lpTempImgData,BufSize);
	_lclose(hf);

 	ReleaseDC(hWnd,hDc);
	LocalUnlock(hTempImgData);
	LocalFree(hTempImgData);
	GlobalUnlock(hImgData);
	return TRUE;
}
////////////////////////////////////////////////////////////////
BOOL LimbPatternM4(HWND hWnd)
{
	DWORD              BufSize;
    LPBITMAPINFOHEADER lpImgData;
	LPSTR              lpPtr;
	HLOCAL             hTempImgData;
	LPBITMAPINFOHEADER lpTempImgData;
	LPSTR              lpTempPtr;
	HDC                hDc;
	HFILE              hf;
	LONG               x,y;
	DWORD              i,j;
	unsigned char      num;
	BYTE			   M4Pattern[16][16];

	for(i=0;i<8;i++)
		for(j=0;j<8;j++)
			M4Pattern[i][j]=4*BayerPattern[i][j];

	for(i=8;i<16;i++)
		for(j=0;j<8;j++)
			M4Pattern[i][j]=4*BayerPattern[i-8][j]+2;

	for(i=0;i<8;i++)
		for(j=8;j<16;j++)
			M4Pattern[i][j]=4*BayerPattern[i][j-8]+3;

	for(i=8;i<16;i++)
		for(j=8;j<16;j++)
			M4Pattern[i][j]=4*BayerPattern[i-8][j-8]+1;

	BufSize=bf.bfSize-sizeof(BITMAPFILEHEADER);

	if((hTempImgData=LocalAlloc(LHND,BufSize))==NULL)
    {
    	MessageBox(hWnd,"Error alloc memory!","Error Message",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }

    lpImgData=(LPBITMAPINFOHEADER)GlobalLock(hImgData);    
	lpTempImgData=(LPBITMAPINFOHEADER)LocalLock(hTempImgData);

	//copy image data
	memcpy(lpTempImgData,lpImgData,BufSize);

	for(y=0;y<bi.biHeight;y++){
		lpPtr=(char *)lpImgData+(BufSize-LineBytes-y*LineBytes);
		lpTempPtr=(char *)lpTempImgData+(BufSize-LineBytes-y*LineBytes);
		for(x=0;x<bi.biWidth;x++){
			num=(unsigned char)*lpPtr++;
			if ( num > M4Pattern[y&15][x&15])
				*(lpTempPtr++)=(unsigned char)255;
			else *(lpTempPtr++)=(unsigned char)0;
			}
	}
	
    if(hBitmap!=NULL)
	    DeleteObject(hBitmap);
	hDc=GetDC(hWnd);	
	hBitmap=CreateDIBitmap(hDc,	(LPBITMAPINFOHEADER)lpTempImgData, (LONG)CBM_INIT,
				(LPSTR)lpTempImgData+sizeof(BITMAPINFOHEADER) +NumColors*sizeof(RGBQUAD),
   				(LPBITMAPINFO)lpTempImgData, DIB_RGB_COLORS);

    hf=_lcreat("c:\\limbm4.bmp",0);
	_lwrite(hf,(LPSTR)&bf,sizeof(BITMAPFILEHEADER)); 
	_lwrite(hf,(LPSTR)lpTempImgData,BufSize);
	_lclose(hf);

 	ReleaseDC(hWnd,hDc);
	LocalUnlock(hTempImgData);
	LocalFree(hTempImgData);
	GlobalUnlock(hImgData);
	return TRUE;
}
////////////////////////////////////////////////////////////////
BOOL Steinberg(HWND hWnd)
{
	DWORD              OffBits,BufSize,IntBufSize;
    LPBITMAPINFOHEADER lpImgData;
	HLOCAL             hTempImgData;
	LPBITMAPINFOHEADER lpTempImgData;
	LPSTR              lpPtr;
	LPSTR              lpTempPtr;
	HDC                hDc;
	HFILE              hf;
	LONG               x,y;
	unsigned char      num;
	float              e,f;
	HLOCAL             hIntBuf;
	int                *lpIntBuf,*lpIntPtr;
	int                tempnum;

	OffBits=bf.bfOffBits-sizeof(BITMAPFILEHEADER);
	BufSize=bf.bfSize-sizeof(BITMAPFILEHEADER);

	if((hTempImgData=LocalAlloc(LHND,BufSize))==NULL)
    {
    	MessageBox(hWnd,"Error alloc memory!","Error Message",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }

	IntBufSize=(DWORD)bi.biHeight*LineBytes*sizeof(int);
	if((hIntBuf=LocalAlloc(LHND,IntBufSize))==NULL)
    {
    	MessageBox(hWnd,"Error alloc memory!","Error Message",MB_OK|MB_ICONEXCLAMATION);
		LocalFree(hTempImgData);
        return FALSE;
    }

    lpImgData=(LPBITMAPINFOHEADER)GlobalLock(hImgData);    
	lpTempImgData=(LPBITMAPINFOHEADER)LocalLock(hTempImgData);
	lpIntBuf=(int *)LocalLock(hIntBuf);

	//copy header data
	memcpy(lpTempImgData,lpImgData,OffBits);

	//copy image data to int buffer
	for(y=0;y<bi.biHeight;y++){
		lpPtr=(char *)lpImgData+(BufSize-LineBytes-y*LineBytes);
		lpIntPtr=(int *)lpIntBuf+(bi.biHeight-1-y)*LineBytes;
		for(x=0;x<bi.biWidth;x++)
			*(lpIntPtr++)=(unsigned char)*(lpPtr++);
	}

	for(y=0;y<bi.biHeight;y++){  
		for(x=0;x<bi.biWidth;x++){
			lpIntPtr=(int *)lpIntBuf+(bi.biHeight-1-y)*LineBytes+x;
			num=(unsigned char)*lpIntPtr;
			if ( num > 128 ){
				*lpIntPtr=255;
				e=(float)(num-255.0);
			}
			else{
				*lpIntPtr=0;
				e=(float)num;
			}
			if(x<bi.biWidth-1){
				f=(float)*(lpIntPtr+1);
				f+=(float)( (3.0/8.0) * e);
				*(lpIntPtr+1)=(int)f;
			}
			if(y<bi.biHeight-1){
				f=(float)*(lpIntPtr-LineBytes);
				f+=(float)( (3.0/8.0) * e);
				*(lpIntPtr-LineBytes)=(int)f;

				f=(float)*(lpIntPtr-LineBytes+1);
				f+=(float)( (1.0/4.0) * e);
				*(lpIntPtr-LineBytes+1)=(int)f;
			}
		}
	}

	//copy new image data from int buffer
	for(y=0;y<bi.biHeight;y++){
		lpTempPtr=(char *)lpTempImgData+(BufSize-LineBytes-y*LineBytes);
		lpIntPtr=(int *)lpIntBuf+(bi.biHeight-1-y)*LineBytes;
		for(x=0;x<bi.biWidth;x++){
			tempnum=*(lpIntPtr++);
			if(tempnum>255) tempnum=255;
			else if (tempnum<0) tempnum=0;
			*(lpTempPtr++)=(unsigned char)tempnum;
		}
	}
	
    if(hBitmap!=NULL)
	    DeleteObject(hBitmap);
	hDc=GetDC(hWnd);	
	hBitmap=CreateDIBitmap(hDc,	(LPBITMAPINFOHEADER)lpTempImgData, (LONG)CBM_INIT,
				(LPSTR)lpTempImgData+sizeof(BITMAPINFOHEADER) +NumColors*sizeof(RGBQUAD),
   				(LPBITMAPINFO)lpTempImgData, DIB_RGB_COLORS);

    hf=_lcreat("c:\\steinberg.bmp",0);
	_lwrite(hf,(LPSTR)&bf,sizeof(BITMAPFILEHEADER)); 
	_lwrite(hf,(LPSTR)lpTempImgData,BufSize);
	_lclose(hf);

 	ReleaseDC(hWnd,hDc);
	GlobalUnlock(hImgData);
	LocalUnlock(hTempImgData);
	LocalFree(hTempImgData);
	LocalUnlock(hIntBuf);
	LocalFree(hIntBuf);
	return TRUE;
}
////////////////////////////////////////////////////////////////
BOOL Bmp2Txt(HWND hWnd)
{
	DWORD              OffBits,BufSize;
    LPBITMAPINFOHEADER lpImgData;
	LPSTR              lpPtr;
	HFILE              hf;
	int				   i, j, k,h,tint,grayindex;
	char               tchar;
	int	               TransHeight, TransWidth;

	for(i=0;i<94;i++)
		for(j=i+1;j<95;j++){
			if(gr[i]>gr[j]){
				tchar=ch[i],tint=gr[i];
				ch[i]=ch[j],gr[i]=gr[j];
				ch[j]=tchar,gr[j]=tint;
			}
		}

	OffBits=bf.bfOffBits-sizeof(BITMAPFILEHEADER);
	BufSize=bf.bfSize-sizeof(BITMAPFILEHEADER);
    lpImgData=(LPBITMAPINFOHEADER)GlobalLock(hImgData);
	TransWidth = bi.biWidth/8;
	TransHeight = bi.biHeight/16;
    hf=_lcreat("c:\\bmp2txt.txt",0);

	for(i=0;i<TransHeight;i++){	
		for(j=0;j<TransWidth;j++){
			grayindex=0;
			for(k=0;k<16;k++)
				for(h=0;h<8;h++){
					lpPtr=(char *)lpImgData+BufSize-LineBytes-(i*16+k)*LineBytes+j*8+h;
					grayindex+=(unsigned char)*lpPtr;
				}
			grayindex/=16*8;
			grayindex=gr[94]*grayindex/255;
			k=0;
			while(gr[k+1]<grayindex)
				k++;
			_lwrite(hf,(char *)&ch[k],sizeof(char)); 
		}
		tchar=(char)13;
		_lwrite(hf,(char *)&tchar,sizeof(char)); 
		tchar=(char)10;
		_lwrite(hf,(char *)&tchar,sizeof(char));
	}
	_lclose(hf);
	GlobalUnlock(hImgData);
	return TRUE;
}
