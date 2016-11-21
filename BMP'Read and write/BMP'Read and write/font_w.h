/*     FONT_W.H     */

#ifndef		_INC_FONTH
#define		_INC_FONTH

char fontfile[40];
FILE *fp16;
char fontfile24[40];
FILE *fp24;

void disp_icona(int x,int y,int Dx,int Dy,BYTE *buff,int flag);
void disp_icon(int x,int y,int Dx,int Dy,BYTE *buff);
void Asc16(int ascii,int x,int y);
void Chn16(int gb1,int gb2,int x,int y);
void Str16(char *buf,int x,int y);
void Num16(int n,int x,int y,int flag);
void open16zk(void);
void close16zk(void);
void Asc24(int asc,int x,int y);             /*  显示12x24西文字符  */
void Chn24(int gb1,int gb2,int x,int y);         /*  显示24x24汉字  */
void Str24(char *buf,int x,int y);
void Num24(int n,int x,int y,int flag);
void Chn24_48(int gb1,int gb2,int x,int y);      /*  显示24x48汉字  */
void Str24_48(char *buf,int x,int y);
void open24zk(void);
void close24zk(void);

void pushcur(int x,int y);
void popcur(void);
void movcur(int x,int y);

#endif  //!_INC_FONTH
 