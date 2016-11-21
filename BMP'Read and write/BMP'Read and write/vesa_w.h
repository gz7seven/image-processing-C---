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

int  VesaOn(int mode);			   /*���� Super VGA ��Ƶģʽ*/
void VesaOff(void);
BYTE *Addr(int x,int y);
void put_pixel(int x,int y,int g);		        /*д��������*/
int  get_pixel(int x,int y);			        /*����������*/
void get_image(int x,int y,int Dx,int Dy,BYTE *buf);
void put_image(int x,int y,int Dx,int Dy,BYTE *buf);
void set_palette(int n,int r,int g,int b);     /*���õ�����ɫ��Ĵ���*/
void get_palette(int n,int *rp,int *gp,int *bp);
void set_allpalette(struct VGA_PAL *pal);      /*����ȫ����ɫ��Ĵ���*/
void get_allpalette(struct VGA_PAL *pal);  /*����ȫ����ɫ��Ĵ�������*/
void set_color(int c);					/*����ǰ��ɫ*/
void set_bkcolor(int c);					/*���ñ���ɫ*/
int  get_color(void);					/*��ѯǰ��ɫ*/
int  get_bkcolor(void);					/*��ѯ����ɫ*/
void fillw(int x,int y,int Dx,int Dy,int c);	      /*���δ�����ɫ*/
void cls(int c);						            /*ȫ����ɫ*/
void line_H(int x,int y,int w);				  /*��ˮƽ��*/
void line_V(int x,int y,int l);				  /*����ֱ��*/
void box(int x,int y,int Dx,int Dy,int w,int c);	  /*���п�ȵľ��ο�*/
int  line_coor(int x,int y,int x1,int y1,struct DOT *dot);
void line_L(int x,int y,int x1,int y1);		            /*��ֱ��*/
int  circle_coor(int r,struct DOT *dot);
void CircPnt(int x,int y,int xc,int yc,int c,int flag);
void circle_C(int x,int y,int r);			   /*��Բ��*/
void circle_F(int x,int y,int r,int g);			  /*Բ����ɫ*/
void palette(int x,int y,int w,int h,int flag);
                                     /*  �ÿ�����ӳ���256����ɫ����  */
void set_Vpalette(BYTE *pal,union COLOR_V *color);     /*���������ɫ��*/
int  searchpath(void);

#endif  //!_INC_VESAH
