/*     FILE.H     */

#include "windows.h"

#ifndef		_INC_FILE
#define		_INC_FILE

struct TIF_IFH {
  char    ByteOrder[2];
  WORD    Version;
  DWORD   FirstIFD;
} ;

struct TIF_DE {
  WORD    Tag;
  WORD    Type;
  DWORD   Length;
  DWORD   ValOff;
} ;

struct PCX_HEAD {
  BYTE Header;
  BYTE Version;
  BYTE Encode;
  BYTE BitPerPixel;
  WORD X1;
  WORD Y1;
  WORD X2;
  WORD Y2;
  WORD Hres;
  WORD Vres;
  struct VGA_PAL pal[16];
  BYTE Resered1;
  BYTE NumOfPlanes;
  WORD BytesPerLine;
  WORD Vmode;
  BYTE Resered2[58];
} ;

struct PCX_END {
  BYTE P_flag;
  struct VGA_PAL pal[256];
} ;

struct GIF_GLOBAL {
  char version[3];
  char version1[3];
  WORD screen_width;
  WORD screen_depth;
  BYTE global_flag;
  BYTE back_color;
  BYTE zero;
} ;

struct GIF_LOCAL {
  char comma;
  WORD image_left;
  WORD image_top;
  WORD image_wide;
  WORD image_deep;
  BYTE local_flag;
} ;

struct TGA_HEAD {
  BYTE    IdLength;
  BYTE    ColorType;
  BYTE    ImageType;
  WORD    FirstColor;
  WORD    ColorLength;
  BYTE    ColorBit;
  WORD    X0;
  WORD    Y0;
  WORD    Width;
  WORD    Height;
  BYTE    Bits;
  BYTE    AttribBit;
} ;

struct TGA_PAL {
  BYTE   blue,green,red;
} ;

RGBQUAD  bgr[256];

WORD  Width,Height,BitCount,Planes,ImageType;
WORD  Compression,Orientation;
DWORD OffBits,ColorMap;
WORD  BytesPerLine,ColorNum;
extern WORD swide,sdeep;

void Trans_1to8(BYTE *sc,int BytesPerLine);
void Trans_8to1(BYTE *sc,int Width);
void Trans_4cto8(BYTE *sc,int BytesPerLine);
void Trans_8to4c(BYTE *sc,int Width);
void Trans_4pto8(BYTE *sc,BYTE *buf,int BytesPerLine);
void Trans_8to4p(BYTE *sc,BYTE *buf,int BytesPerLine);
void Trans_24to32(BYTE *sc,int BytesPerLine);
void Trans_32to24(BYTE *sc,int Width);
void trans_8toColor(BYTE* sc,int Dx);
void bgr2vga(RGBQUAD *bgr,struct VGA_PAL *pal,int n);
void vga2bgr(struct VGA_PAL *pal,RGBQUAD *bgr,int n);
int  loadBMPhead(char *file);                      /*  读BMP文件头  */
void loadBMPimage(char *file,int px,int py,int Dx,int Dy,int sx,int sy);
int  saveBMPhead(char *file);                      /*  写BMP文件头  */
void saveBMPimage(char *file,int x,int y,int Dx,int Dy,int bits);
void ggetch(void);                          /*  屏幕捕捉控制子程序  */

int  loadTIFhead(char *file);                	       /*读TIF文件头*/
void loadTIFimage(char *file,int px,int py,int Dx,int Dy,int sx,int sy);
int  saveTIFhead(char *file);			       /*写TIF文件头*/
void saveTIFimage(char *file,int x,int y,int Dx,int Dy,int bits);

int  loadPCXhead(char *file);                           /*读PCX文件头*/
void loadPCXimage(char *file,int px,int py,int Dx,int Dy,int sx,int sy);
int  savePCXhead(char *file);                           /*写PCX文件头*/
void savePCXimage(char *file,int x, int y,int Dx, int Dy,int bits);

int  loadGIFhead(char *file);                           /*读PCX文件头*/
void loadGIFimage(char *file,int px,int py,int Dx,int Dy,int sx,int sy);
int  saveGIFhead(char *file);                           /*写PCX文件头*/
void saveGIFimage(char *file,int x, int y,int Dx, int Dy,int bits);

int  loadTGAhead(char *file);                           /*读PCX文件头*/
void loadTGAimage(char *file,int px,int py,int Dx,int Dy,int sx,int sy);
int  saveTGAhead(char *file);                           /*写PCX文件头*/
void saveTGAimage(char *file,int x, int y,int Dx, int Dy,int bits);

int  LoadFileHead(char *file);
void LoadFileImage(char *file,int px,int py,int Dx,int Dy,int sx,int sy);
int  SaveFileHead(char *file);
void SaveFileImage(char *file,int x, int y,int Dx, int Dy,int bits);

#endif  //!_INC_FILE
