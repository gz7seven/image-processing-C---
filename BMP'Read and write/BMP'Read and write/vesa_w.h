/*      VESA_W.H      */

#ifndef		_INC_VESAH
#define		_INC_VESAH

#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <math.h>
#include <windows.h>
#include "image32.h"

#define  CODEFILE	 "CodeFile32"
#define  picpath     "c:\\"

enum COLORS { BLACK,BLUE,GREEN,CYAN,
	      RED,MAGENTA,BROWN,LIGHTGRAY,
	      DARKGRAY,LIGHTBLUE,LIGHTGREEN,LIGHTCYAN,
	      LIGHTRED,LIGHTMAGENTA,YELLOW,WHITE
};

struct VGA_PAL {
  BYTE r,g,b;
};

union  COLOR_V {
  BYTE  c1;
  WORD  c2;
  BYTE  c3[3];
  BYTE  c4[4];
};

struct DOT
{
  int  x,y;
};

extern char filepath[];
extern char fontpath[];

struct  VGA_PAL T_pal[256];
union   COLOR_V T_color[257];

WORD swide,sdeep;
WORD T_bpl,T_bpp,T_bitpp;
WORD actbank[2],tab_bx[2],alpha;
WORD T_fcolor,T_bcolor;

int  VesaOn(int mode);			   /*设置 Super VGA 视频模式*/
void VesaOff(void);
BYTE *Addr(int x,int y);
void put_pixel(int x,int y,int g);		        /*写像素数据*/
int  get_pixel(int x,int y);			        /*读像素数据*/
void get_image(int x,int y,int Dx,int Dy,BYTE *buf);
void put_image(int x,int y,int Dx,int Dy,BYTE *buf);
void set_palette(int n,int r,int g,int b);     /*设置单个调色板寄存器*/
void get_palette(int n,int *rp,int *gp,int *bp);
void set_allpalette(struct VGA_PAL *pal);      /*设置全部调色板寄存器*/
void get_allpalette(struct VGA_PAL *pal);  /*读出全部调色板寄存器数据*/
void set_color(int c);					/*设置前景色*/
void set_bkcolor(int c);					/*设置背景色*/
int  get_color(void);					/*查询前景色*/
int  get_bkcolor(void);					/*查询背景色*/
void fillw(int x,int y,int Dx,int Dy,int c);	      /*矩形窗口填色*/
void cls(int c);						            /*全屏填色*/
void line_H(int x,int y,int w);				  /*画水平线*/
void line_V(int x,int y,int l);				  /*画垂直线*/
void box(int x,int y,int Dx,int Dy,int w,int c);	  /*画有宽度的矩形框*/
int  line_coor(int x,int y,int x1,int y1,struct DOT *dot);
void line_L(int x,int y,int x1,int y1);		            /*画直线*/
int  circle_coor(int r,struct DOT *dot);
void CircPnt(int x,int y,int xc,int yc,int c,int flag);
void circle_C(int x,int y,int r);			   /*画圆周*/
void circle_F(int x,int y,int r,int g);			  /*圆内填色*/
void palette(int x,int y,int w,int h,int flag);
                                     /*  用块充填子程序画256个彩色方块  */
void set_Vpalette(BYTE *pal,union COLOR_V *color);     /*设置虚拟调色板*/
int  searchpath(void);

#endif  //!_INC_VESAH
