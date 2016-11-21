/******************
  IMAGE.H  头文件
******************/

#include <windows.h>

#ifndef		_INC_IMAGE
#define		_INC_IMAGE

struct IMAGE                                          //  DIB 数据结构
{
  HANDLE   		hdib;
  BITMAPINFO 	*lpbi;
  LPSTR    		lpbits;

  DWORD    		wid;
  DWORD    		hei;
  WORD     		bitcount;
  WORD     		bpl;
  WORD     		bpp;
} ;

void  CMUFileOpen(HWND hWnd,char *str);
void  CMUSaveFile(HWND hWnd,char *str);

int   ImageOpen(struct IMAGE * image,char *str);
void  ImageCreate(struct IMAGE * image,int wid,int hei,int bitcount);
void  ImageClose(struct IMAGE * image);
void  ImageSave(struct IMAGE * image,char *str);

BYTE  *Addr_w(struct IMAGE * image,int x,int y);
DWORD getpixel(struct IMAGE * image,int x,int y);
void  setpixel(struct IMAGE * image,int x,int y,DWORD c);
void  putpixel(HWND hWnd,struct IMAGE * image,int px,int py,
                                        DWORD c,int sx,int sy);
void  getimage(struct IMAGE * image,int x,int y,int Dx,int Dy,BYTE* buf);
void  setimage(struct IMAGE * image,int x,int y,int Dx,int Dy,BYTE* buf);
void  putimage(HWND hWnd,struct IMAGE * image,int px,int py,
                              int Dx,int Dy,BYTE* buf,int sx,int sy);
void  setwin(struct IMAGE * image,int x,int y,int Dx,int Dy,DWORD color);
void  fillwin(HWND hWnd,struct IMAGE * image,int x,int y,int Dx,int Dy,DWORD color);
void  getpalette(struct IMAGE * image,int n,int *pr,int *pg,int *pb);
void  setpalette(struct IMAGE * image,int n,int r,int g,int b);
void  putpalette(HWND hWnd,struct IMAGE * image,int n,int r,int g,int b);
 
COLORREF DWORD2CLRREF(DWORD c);

HBITMAP CreateDDB(HWND hWnd,int nHorz,int nVert);
void DeleteDDB(HBITMAP hBitmap);
HDC  CreateMemDC(void);
void DeleteMemDC(HDC hMemDC);
void ClearDDB(void);
void WriteDDB(struct IMAGE * image,int sx,int sy,
				             int px,int py,int Dx,int Dy);
void DisplayDDB(HWND hWnd);
void GetSysParam(HDC hdc);

void SetTitle(char *str);
void delay(int n);
int  kbhit(void);
int  getch(void); 
int  KeyCoor(int *xp,int *yp);
  
extern int  Mode_flag;

#endif  //!_INC_IMAGE
