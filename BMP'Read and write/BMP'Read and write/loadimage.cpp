/*    LOADBMP_W.C    */
/*     2002.01.16    */

#include "vesa_w.h"
#include "file_w.h"
#include "font_w.h"

int loadBMPhead(char *file)                        /*  读BMP文件头  */
{
  BITMAPFILEHEADER  bmphead;
  BITMAPINFOHEADER  bmi;
  FILE *fp;
  int  flag=0;
  DWORD dd;
  
  fp=fopen(file,"rb");                            /*  打开图像文件  */
  if (fp != NULL) {
    flag=1;
    fseek(fp,0L,SEEK_SET);
    fread(&bmphead,sizeof(BITMAPFILEHEADER),1,fp);/*  读位图文件头  */
    fread(&bmi,sizeof(BITMAPINFOHEADER),1,fp);    /*  读位图信息头  */
 					      /*  以下取出图像参数  */
    OffBits      = bmphead.bfOffBits;
    Width        = (WORD) bmi.biWidth;
    Height       = (WORD) bmi.biHeight;
    BitCount     = bmi.biBitCount;
    Planes       = bmi.biPlanes;
    Compression  = (WORD) bmi.biCompression;
    BytesPerLine = (Width*BitCount+31)/32*4;    /*  计算每行字节数  */
    ColorMap     = sizeof(BITMAPFILEHEADER) + bmi.biSize;
    ColorNum     = 0;

    if (BitCount<=8) {
      dd=bmi.biClrUsed;
	  if ((dd>0)&&(dd<256)) ColorNum = (WORD) dd;
	  else                  ColorNum = 1 << BitCount;
      fseek(fp,ColorMap,SEEK_SET);
      fread((BYTE*) bgr,4,ColorNum,fp);         /*  读入调色板数据  */
      bgr2vga(bgr,T_pal,ColorNum);
    }
    fclose(fp);                                   /*  关闭图像文件  */
  }
  return(flag);
}

void loadBMPimage(char *file,int px,int py,int Dx,int Dy,int sx,int sy)
{                                                      /*显示BMP图像*/
  int  i,dw,dh;
  BYTE *sc;
  FILE *fp;

  i=loadBMPhead(file);                                 /*读BMP文件头*/
  if (i==0)  return;

  dw=min(Width -px,Dx);    dw=min(dw,swide-sx);       /*确定视口尺寸*/
  dh=min(Height-py,Dy);    dh=min(dh,sdeep-sy);
  if (T_bitpp<=8) set_allpalette(T_pal);       /*设置VGA调色板寄存器*/
  else            px *= T_bpp;     /*乘上每象素字节数,用于真彩色显示*/
  sc = malloc(4*Width);                               /*申请存储单元*/

  fp=fopen(file,"rb");                                /*打开图形文件*/
  fseek(fp,OffBits,SEEK_SET);                         /*设置取数指针*/
  for(i=Height-1;i>=0;i--) {
    fread(sc,1,BytesPerLine,fp);                        /*读一行数据*/

    if (BitCount==1)  Trans_1to8(sc,BytesPerLine);      /*是二值图像*/
    else if (BitCount==4)  Trans_4cto8(sc,BytesPerLine);
							/*是16色图像*/
    else if ((BitCount==24)&&(T_bitpp==32))
       Trans_24to32(sc,BytesPerLine);
			 /*  是 32位真彩色显示模式，则进行格式转换  */
    if ((BitCount<=8)&&(T_bitpp>=24)) 
       trans_8toColor(sc,Width);

    if ((i>=py)&&(i<py+dh))
      put_image(sx,i+sy-py,dw,1,&sc[px]);             /*显示一行图像*/
  }
  fclose(fp);                                         /*关闭图像文件*/
  free(sc);                                           /*释放存储单元*/
}

int saveBMPhead(char *file)                        /*  写BMP文件头  */
{
  BITMAPFILEHEADER  bmphead = {
	    0x4d42,0,0,0,0  } ;               /*  位图文件头缺省值  */
  BITMAPINFOHEADER  bmi = {
	   40,0,0,1,0,0,0,0,0,0,0  } ;        /*  位图信息头缺省值  */
  FILE *fp;
  int  flag=0;

  BytesPerLine=(Width*BitCount+31)/32*4;        /*  计算每行字节数  */
  ColorMap = 54;
  OffBits  = 54;
  if (BitCount<=8) {
    ColorNum= 1 << BitCount;
    OffBits = 54 + 4*ColorNum;            /*  计算象素数据存放位置  */
  }

  fp=fopen(file,"w+b");                           /*  建立图像文件  */
  if (fp != NULL) {
    flag=1;
    bmi.biWidth        = Width;                   /*  设置图像参数  */
    bmi.biHeight       = Height;
    bmi.biBitCount     = BitCount;
    bmphead.bfOffBits = (DWORD) OffBits;

    fwrite(&bmphead,sizeof(BITMAPFILEHEADER),1,fp);
	                                              /*  写位图文件头  */
    fwrite(&bmi,sizeof(BITMAPINFOHEADER),1,fp);   /*  写位图信息头  */
    if (BitCount<=8) {
      get_allpalette(T_pal);                      /*  取调色板数据  */
      vga2bgr(T_pal,bgr,ColorNum);
      fwrite(bgr,4,ColorNum,fp);                           /*  写色调数据  */
    }
    fclose(fp);                                   /*  关闭图像文件  */
  }
  return(flag);
}

void saveBMPimage(char *file,int x,int y,int Dx,int Dy,int bits)
{                                                /*  存BMP图像文件  */
  int  i;
  BYTE *sc;
  FILE *fp;

  BitCount = bits;
  Width=min(Dx,swide-x);   Height=min(Dy,sdeep-y);
						  /*  确定图像窗口  */
  i=saveBMPhead(file);                             /*  写BMP文件头  */
  if (i==0)  return;                           /*  文件不存在,返回  */
 
  sc = malloc(4*Width);                           /*  申请存储单元  */
  fp=fopen(file,"r+b");                           /*  打开图像文件  */
  fseek(fp,OffBits,SEEK_SET);                     /*  设置存数指针  */
  for(i=Height-1;i>=0;i--) {
    get_image(x,i+y,Width,1,sc);                /*  取一行图像数据  */

    if (BitCount==1)      Trans_8to1(sc,Width);
					    /*  转换成二值图像数据  */
    else if (BitCount==4) Trans_8to4c(sc,Width);/*  转换成16色图像  */
    else if (T_bitpp==32) Trans_32to24(sc,Width);
				        /*  32位数据转换成24位数据  */
    fwrite(sc,1,BytesPerLine,fp);               /*  存一行图像数据  */
  }
  fclose(fp);                                     /*  关闭图像数据  */
  free(sc);                                       /*  释放存储单元  */
}

int loadTIFhead(char *file)                    	       /*读TIF文件头*/
{
  struct TIF_IFH head;
  struct TIF_DE dir[25];
  FILE *fp;
  WORD tif_pal[768];
  WORD i,j,k,m,n,t,nn,c1,c2,flag=0;
  BYTE *p;

  fp=fopen(file,"rb");				      /*打开图像文件*/
  if (fp!=NULL) {
    flag=1;
    ColorMap=0;
    fread(&head,8,1,fp);			      /*读图像文件头*/
    fseek(fp,head.FirstIFD,SEEK_SET);
    fread(&nn,2,1,fp);			            /*读文件目录项数*/
    fread(dir,12,nn,fp);			      /*读入文件目录*/
    for (i=0;i<nn;i++) {           /*由文件目录中取出TIF图像文件参数*/
      switch(dir[i].Tag) {
	case 0x100 :
	  Width=(WORD) dir[i].ValOff;        break;
	case 0x101 :
	  Height=(WORD) dir[i].ValOff;       break;
	case 0x102 :
	  if (dir[i].Length>2)  BitCount=24;
	  else       BitCount=(WORD) dir[i].ValOff;
	  break;
	case 0x103 :
	  Compression=(WORD) dir[i].ValOff;  break;
	case 0x106 :
	  ImageType=(WORD) dir[i].ValOff;    break;
	case 0x111 :
	  if (dir[i].Length==1)
	    OffBits=(WORD) dir[i].ValOff;
	  else {
	    fseek(fp,dir[i].ValOff,SEEK_SET);
	    fread(&OffBits,4,1,fp);
	  }
	  break;
	case 0x140 :
	  ColorMap=dir[i].ValOff;
	  n=(WORD) dir[i].Length;
	  fseek(fp,ColorMap,SEEK_SET);
	  fread(tif_pal,2,n,fp);	         /*读入TIF调色板数据*/
	  p=(BYTE *) T_pal;    n/=3;
	  for (m=0;m<n;m++)   			      /*色调数据转换*/
	    for (j=0,k=0;j<3;j++,k+=n)
	      p[3*m+j] = (tif_pal[m+k] >> 8) & 0xff;
          break;
	default :                    break;
      }
      if (ImageType<2) {		    /*设置黑白图像调色板数组*/
	p=(BYTE *) T_pal;
	if (BitCount==1) {
	  if (ImageType==0)  {  c1=0;    c2=255;  }
	  else               {  c1=255;  c2=0;    }
	  for (j=0;j<3;j++)   p[j]  =(BYTE) c2;
	  for (j=0;j<3;j++)   p[3+j]=(BYTE) c1;
        }
	else {
	  for (t=0;t<256;t++) {
	    k=t;
	    if (ImageType==0)   k=255-k;
	    for (j=0;j<3;j++)   p[3*t+j]=(BYTE) k;
	  }
        }
      }
    }
    fclose(fp);					      /*关闭图像文件*/
    if (BitCount==1)       BytesPerLine=(Width+7)/8;/*计算每行字节数*/
    else if (BitCount==4)  BytesPerLine=(Width+1)/2;
    else if (BitCount==8)  BytesPerLine=Width;
    else if (BitCount==24) BytesPerLine=3*Width;
    if (BitCount<=8) ColorNum=1<<BitCount;
	Planes=1;
  }
  return(flag);
}

void loadPackBitsline(BYTE *p,int wid,FILE *fp) /*紧缩位编码解码一行*/
{
  WORD n=0,i,c;
  char ch;

  do{
    ch=fgetc(fp);					  /*取一字节*/
    if (ch < 0) {				  /*是负数为行程编码*/
      i=-ch+1;					          /*得行程值*/
      c=fgetc(fp);				        /*取像素数值*/
      while(i--)  p[n++]=(BYTE) c;	/*像素数值重复若干次*/
    }
    else {				          /*否则为原像素数据*/
      i= ch+1;						  /*得字节数*/
      while(i--)  p[n++]=fgetc(fp);			  /*复制数据*/
    }
  }
  while(n<wid);				              /*不满一行继续*/
}

void loadTIFimage(char *file,int px,int py,int Dx,int Dy,int sx,int sy)
{						       /*显示TIF图像*/
  FILE *fp;
  int  i,j,k,m,n,dw,dh;
  BYTE *sc;

  i=loadTIFhead(file);				       /*读TIF文件头*/
  if (i==0)  return;

  dw=min(Width -px,Dx);   dw=min(dw,swide-sx); 	      /*确定视口尺寸*/
  dh=min(Height-py,Dy);   dh=min(dh,sdeep-sy);
  if (T_bitpp<=8) 
     set_allpalette(T_pal);       /*设置VGA调色板寄存器*/
  else { 
     px *= T_bpp;
     if (BitCount<=8)
        set_Vpalette((BYTE *)T_pal,(union COLOR_V *)bgr);   
  }
  sc=malloc(4*Width);				      /*申请存储单元*/

  fp=fopen(file,"rb");				      /*打开图像文件*/
  fseek(fp,OffBits,SEEK_SET);
  for (i=0;i<Height;i++) {
    if (Compression==1)
      fread(sc,1,BytesPerLine,fp);		    /*不压缩数据读入*/
    else if (Compression==32773U)
      loadPackBitsline(sc,BytesPerLine,fp);   	/*紧缩位编码解码一行*/

    if (BitCount==1)      Trans_1to8(sc,BytesPerLine);/*显示格式转换*/
    else if (BitCount==4) Trans_4cto8(sc,BytesPerLine);
    else if (BitCount==24) {
      n=3*(Width-1);    k=T_bpp*(Width-1);
      for (j=0;j<Width;j++,n-=3,k-=T_bpp) {
	    m=sc[n+2];
	    if (T_bitpp==32)  sc[k+1]=sc[n+1];
	    sc[k+2]=sc[n];    sc[k]=m;		      /*红蓝分量交换*/
      }
    }
    if ((BitCount<=8)&&(T_bitpp>=24))
       trans_8toColor(sc,Width);
 
    if ((i>=py)&&(i<py+dh))
      put_image(sx,i+sy-py,dw,1,&sc[px]);			  /*显示一行*/
  }
  fclose(fp);					      /*关闭图像文件*/
  free(sc);					      /*释放存储单元*/
}

int saveTIFhead(char *file)			       /*写TIF文件头*/
{
  struct TIF_IFH head;
  struct TIF_DE dir[]={				   /*TIF文件缺省参数*/
	 0x100,4,1,0,    0x101,4,1,0,
	 0x102,3,1,0,    0x103,3,1,0,
	 0x106,3,1,0,    0x111,4,1,0,
	 0x112,3,1,1,    0x115,3,1,0,
	 0x11c,3,1,1,    0x140,3,0x300,0
  };
  long long2,ll;
  FILE *fp;
  WORD tif_pal[768];
  int  i,j,k,m,n,flag=0;
  BYTE *p;

  fp=fopen(file,"w+b"); 			    /*打开图像文件头*/
  if (fp!=NULL) {
    flag=1;
    memset(head.ByteOrder,'I',2);
    head.Version  = 0x2a;     head.FirstIFD  = 8;
						  /*生成图像文件参数*/
    dir[0].ValOff = Width;    dir[1].ValOff = Height;
    dir[4].ValOff=0;          dir[7].ValOff = 1;
    if (BitCount==24) {
      dir[4].ValOff=2;        dir[7].ValOff = 3;
    }
    else if (BitCount==4)     dir[4].ValOff = 3;
    else if (BitCount==8)     dir[4].ValOff = 3;
    if (ImageType<2)          dir[4].ValOff = ImageType;
    dir[3].ValOff=Compression;

    n=9;
    if (dir[4].ValOff==3)     n++;

    long2=10+12*n+4;
    if (BitCount==24) {
      dir[2].Length = 3;      dir[2].ValOff = long2;
      long2+=6;
    }
    else                      dir[2].ValOff = BitCount;

    OffBits=long2;
    if (dir[4].ValOff==3) {
      if (BitCount==8)
	    OffBits+=768*2;
      else if (BitCount==4) {
	    OffBits+=48*2;
	    dir[9].Length=0x30;
      }
      dir[9].ValOff=long2;
    }

    dir[5].ValOff=OffBits;    ll=0;
    fwrite(&head,8,1,fp);			      /*写图像文件头*/
    fwrite(&n,2,1,fp);				      /*写图像目录数*/
    fwrite(dir,12,n,fp);        			/*写图像目录*/
    fwrite(&ll,4,1,fp);				  /*指向下一图像目录*/
    if (BitCount==24) {		      /*存真彩色图像的每个采样的位数*/
      for (j=0,m=8;j<3;j++)
	    fwrite(&m,2,1,fp);
    }
    if (dir[4].ValOff==3) {
      get_allpalette(T_pal);		              /*取调色板数据*/
      p=(BYTE *) T_pal;
      k=256;
      if (BitCount==4)  k=16;
      for (i=0;i<3;i++) {			      /*色调数据转换*/
	    for (j=0;j<k;j++) {
	      n=((int) p[3*j+i]) & 0xff;    m=n | (n<<8);
	      tif_pal[k*i+j]=m;
		}
      }
      if (BitCount==8)
	    fwrite(tif_pal,2,768,fp);
      else if (BitCount==4)                      	   /*写TIF调色板数据*/
	    fwrite(tif_pal,2,48,fp);
    }
    fclose(fp);					      /*关闭图像文件*/
    if (BitCount==1)       BytesPerLine=(Width+7)/8;  /*计算每字节数*/
    else if (BitCount==4)  BytesPerLine=(Width+1)/2;
    else if (BitCount==8)  BytesPerLine=Width;
    else if (BitCount==24) BytesPerLine=3*Width;
  }
  return(flag);
}

void savePackBitsline(BYTE *buf,int pwide,FILE *fp)
{				              /*用紧缩位编码编码一行*/
  int   j,k,l,gray,i=0,n=0;
  char  buf1[1000],buf2[200];

  for (j=0;j<pwide;j++) {			          /*编码一行*/
    gray=buf[j];              k=1;	              /*取一像素数据*/
    j++;
    while((j<pwide)&&(gray==buf[j])) {		      /*数值重复继续*/
      k++;           j++;
    }
    j--;
    while(k>128) {				   /*行程超过128截断*/
      buf1[n++]=(BYTE) 0xff;         buf1[n++]=gray;
      k-=128;
    }
    if (k==1) buf2[i++]=gray;		        /*每字节数存入缓冲区*/
    else if ((k==2)&&(i>0)) {		  /*行程为2,同时缓冲区有数据*/
      buf2[i++]=gray;  buf2[i++]=gray;              /*数据存入缓冲区*/
      k=0;
    }
    if (k>1) {					         /*行程大于1*/
      if (i>0) {				  /*缓冲区数据先输出*/
	    buf1[n++]=i-1;
	    for (l=0;l<i;l++)  buf1[n++]=buf2[l];
	    i=0;
      }
      buf1[n++]=-(k-1);   buf1[n++]=gray;		  /*行程编码*/
    }
  }
  if (i>0) {				        /*缓冲区剩余数据输出*/
    buf1[n++]=i-1;
    for (l=0;l<i;l++)  buf1[n++]=buf2[l];
  }
  fwrite(buf1,1,n,fp);					  /*数据存盘*/
}

void saveTIFimage(char *file,int x,int y,int Dx,int Dy,int bits)
{					             /*写TIF图形文件*/
  FILE *fp;
  WORD i,j,k,n,t;
  BYTE *sc;

  BitCount = bits;
  Width=min(Dx,swide-x);   Height=min(Dy,sdeep-y);
  i=saveTIFhead(file);				       /*写TIF文件头*/
  if (i==0)  return;

  sc=malloc(4*Width);				      /*申请存储单元*/
  fp=fopen(file,"r+b");				      /*打开图像文件*/
  fseek(fp,OffBits,SEEK_SET);			      /*设置存数指针*/
  for (i=0;i<Height;i++) {
    get_image(x,y+i,Width,1,sc);                  /*读入一行图像数据*/

    if (BitCount==1)       Trans_8to1(sc,Width);    /*转换成二值图像*/
    else if (BitCount==4)  Trans_8to4c(sc,Width);/*转换成16色彩色图像*/
    else if (T_bitpp > 16) {			        /*真彩色图像*/
      for (j=0,n=0,k=0;j<Width;j++,n+=3,k+=T_bpp) {
	    t=sc[k];
	    if (T_bitpp==32)  sc[n+1]=sc[k+1];
	    sc[n]=sc[k+2];    sc[n+2]=(BYTE) t;		      /*红蓝分量交换*/
      }
    }

    if (Compression==1)
      fwrite(sc,1,BytesPerLine,fp);	          /*不压缩, 直接存盘*/
    else if (Compression==32773U)
      savePackBitsline(sc,BytesPerLine,fp);           /*编码后存一行*/
  }
  fclose(fp);					      /*关闭图像文件*/
  free(sc);					      /*释放存储单元*/
}

int loadPCXhead(char *file)                            /*读PCX文件头*/
{
  struct PCX_HEAD PCX_head;
  FILE *fp;
  int  flag=0;

  fp=fopen(file,"rb");                                /*打开图像文件*/
  if (fp!=NULL) {
    flag=1;
    fread(&PCX_head,128,1,fp);                         /*读PCX文件头*/
    BitCount     = PCX_head.BitPerPixel;
    Planes       = PCX_head.NumOfPlanes;
    BytesPerLine = PCX_head.BytesPerLine;
    Width        = PCX_head.X2-PCX_head.X1+1;
    Height       = PCX_head.Y2-PCX_head.Y1+1;
    Compression  = PCX_head.Encode;
    OffBits      = 128;
    if (BitCount<=8) ColorNum=1<<(BitCount*Planes);
    if (BitCount==1) {
      fseek(fp,16,SEEK_SET);
      fread(T_pal,1,48,fp);                     /*读入16色调色板数据*/
    }
    if ((Planes==1)&&(BitCount==8)) {
      fseek(fp,-768,SEEK_END);
      fread(T_pal,1,768,fp);                   /*读入256色调色板数据*/
    }
    fclose(fp);                                       /*关闭图像文件*/
  }
  return(flag);
}

void loadPCXline(BYTE *p,int wid,FILE *fp)
{                                          /*PCX图像文件行程编码一行*/
  int n=0,i,c;
 
  memset(p,0,wid);
  do{
    c=fgetc(fp);                                      /*读入一个字符*/
    if((c&0xc0)==0xc0) {                  /*高两位为1时,该字节为行程*/
      i=c&0x3f;                                 /*屏蔽掉高两位的行程*/
      c=fgetc(fp);                                        /*取颜色号*/
      while(i--)  p[n++]=c;                           /*恢复象素数据*/
    }
    else  p[n++]=c;                                   /*单个象素数据*/
  }
  while(n<wid);                                  /*一行未满,继续解码*/
 }

void loadPCXimage(char *file,int px,int py,int Dx,int Dy,int sx,int sy)
{                                   /*256色或真彩色模式下显示PCX图像*/
  FILE *fp;
  int  i,j,k,dw,dh,bpl;
  BYTE *sc,*buf,*buf1;

  i=loadPCXhead(file);                                 /*读PCX文件头*/
  if (i==0)  return;

  dw=min(Width -px,Dx);    dw=min(dw,swide-sx);       /*确定视口尺寸*/
  dh=min(Height-py,Dy);    dh=min(dh,sdeep-sy);
  if (T_bitpp<=8) 
     set_allpalette(T_pal);       /*设置VGA调色板寄存器*/
  else { 
     px *= T_bpp;
     if (BitCount*Planes<=8)
        set_Vpalette((BYTE *)T_pal,(union COLOR_V *)bgr);   
  }
  sc =malloc(3*Width);                                /*申请存储单元*/
  buf=malloc(4*Width);
  buf1=buf;         

  fp=fopen(file,"rb");                                /*打开图像文件*/
  fseek(fp,128,SEEK_SET);                         /*指向橡数数据位置*/
  bpl=BytesPerLine * Planes;                      /*计算每行总字节数*/
  for(i=0;i<Height;i++) {
    if (!feof(fp)) {
      loadPCXline(sc,bpl,fp);                             /*解码一行*/

      if (Planes==1) {              /*单位平面图像,为二值和256色彩色*/
	     if (BitCount==1)  Trans_1to8(sc,BytesPerLine);
	     buf1=sc;
      }
      else if (Planes==4) {                           /*16色彩色图像*/
	     Trans_4pto8(sc,buf,BytesPerLine);
	  }
      else if (Planes==3) {                             /*真彩色图像*/
	     for (j=0,k=0;j<BytesPerLine;j++,k+=T_bpp) {
	        buf[k+2]=sc[j];
	        buf[k+1]=sc[BytesPerLine+j];
	        buf[k]  =sc[2*BytesPerLine+j];
		 }
	  }
      if ((Planes*BitCount<=8)&&(T_bitpp>=24))
         trans_8toColor(buf1,Width);
    
      if ((i>=py)&&(i<py+dh))                          /*显示一行图像*/
	     put_image(sx,i+sy-py,dw,1,&buf1[px]);
	}
    else break;
  }
  fclose(fp);                                         /*关闭图像文件*/
  free(sc);                                           /*释放存储单元*/
  free(buf);
}

int savePCXhead(char *file)                            /*写PCX文件头*/
{
  struct PCX_HEAD PCX_head;
  BYTE *p;
  int  i,flag=0;
  FILE *fp;

  p=(BYTE *) &PCX_head;
  for (i=0;i<128;i++) (*p++) = 0;
  if (BitCount==1)  BytesPerLine = (Width+7)/8;     /*计算每行字节数*/
  else              BytesPerLine = Width;
  BytesPerLine = (BytesPerLine+1)/2*2;              /*计算每行字节数*/
  fp=fopen(file,"w+b");                               /*建立图像文件*/
  if (fp!=NULL) {                               /*形成图像文件头数据*/
    flag=1;
    PCX_head.Header       = 10;
    PCX_head.Version      = 5;
    PCX_head.Encode       = 1;
    PCX_head.BitPerPixel  = (BYTE) BitCount;
    PCX_head.X2           = Width-1;
    PCX_head.Y2           = Height-1;
    PCX_head.NumOfPlanes  = (BYTE) Planes;
    PCX_head.BytesPerLine = BytesPerLine;
    fwrite(&PCX_head,128,1,fp);                       /*写图像文件头*/
    if (BitCount==1) {
      get_allpalette(T_pal);
      fseek(fp,0x10,SEEK_SET);
      fwrite(T_pal,48,1,fp);                      /*写16色调色板数据*/
    }
    fclose(fp);
  }                                                   /*关闭图像文件*/
  return(flag);
}

void savePCXline(BYTE *buf,int pwide,FILE *fp)
{                                          /*PCX图像文件行程编码一行*/
  int   j,k,gray,n=0;
  BYTE  buf1[1000];

  for (j=0;j<pwide;j++) {
    gray=buf[j];              k=1;                      /*取一个数据*/
    j++;
    while((j<pwide)&&(gray==buf[j])) {                    /*检测行程*/
      k++;                    j++;
    }
    j--;
    while(k>63) {                             /*大于63的行程分成几段*/
      buf1[n++]=0xff;         buf1[n++]=gray;
      k-=63;
    }
    if (k==1)                                         /*单点行程处理*/
      if (gray<0xc0)          buf1[n++]=gray;     /*小于0XC0用单字节*/
      else {                                         /*否则,用双字节*/
	    buf1[n++]=0xc1;       buf1[n++]=gray;
      }
    else { buf1[n++]=0xc0+k;  buf1[n++]=gray;   }         /*得行程码*/
  }
  fwrite(buf1,1,n,fp);
}

void savePCXimage(char *file,int x, int y,int Dx, int Dy,int bits)
{                                                    /*存PCX图像文件*/
  int i,j,k,bpl,P_flag=12;
  BYTE *sc,*buf,*buf1;
  FILE *fp;

  BitCount = bits;
  Width=min(Dx,swide-x);   Height=min(Dy,sdeep-y);    /*确定图像窗口*/
  i=savePCXhead(file);                                 /*写PCX文件头*/
  if (i==0)  return;

  bpl=BytesPerLine*Planes;                        /*计算每行总字节数*/
  sc =malloc(4*Width);                                /*申请存储单元*/
  buf=malloc(3*Width);

  buf1=buf;
  fp=fopen(file,"r+b");                               /*打开图像文件*/
  fseek(fp,0x80,SEEK_SET);                        /*指向像素存放位置*/
  for(i=0;i<Height;i++) {
    get_image(x,i+y,Width,1,sc);                    /*取一行图像数据*/

    if (Planes==1) {
      buf1=sc;
      if (BitCount==1)  Trans_8to1(sc,Width);
    }
    else if (Planes==4) {
      Trans_8to4p(sc,buf,BytesPerLine);
    }
    else if (Planes==3) {
      for (j=0,k=0;j<BytesPerLine;j++,k+=T_bpp) {
	    buf[j]=sc[k+2];       buf[j+BytesPerLine]=sc[k+1];
	    buf[j+2*BytesPerLine]=sc[k];
      }
    }

    savePCXline(buf1,bpl,fp);                       /*数据作行程编码*/
  }
  if ((Planes==1)&&(BitCount==8)) {
    get_allpalette(T_pal);
    fseek(fp,0,SEEK_CUR);
    fwrite(&P_flag,1,1,fp);
    fwrite(T_pal,768,1,fp);                      /*写256色调色板数据*/
  }
  fclose(fp);                                         /*关闭图像文件*/
  free(sc);                                           /*释放存储单元*/
  free(buf);
}

int   index,rement;
char  rem;
char  *CTfirst,*CTlast,strbuf[256];
int   *CTnext,*CTlink,CLEAR,EOI,len,max,next;

int  LoadCode(int len,FILE *fp)         /*解码时从数据流中读一个代码*/
{
  int value,mask,start,used;

  start=value=0;
  while(len > 0) {
    if (rement == 0) {
      while(index == 0) index=getc(fp);
      index--;
      rem=(unsigned char) getc(fp);
      rement=8;
    }
    if (len > rement)  used = rement;
    else               used = len;
    rement -= used;
    mask = (0xff>>(8-used));
    mask &= rem;       rem  >>= used;
    mask <<= start;    start += used;
    value |= mask;     len   -= used;
  }
  return(value);
}

void InitStringTable(int size,int flag)             /*初始化字符串表*/
{
  int i;

  len=size+1;        max=1<<len;
  CLEAR=1<<size;     EOI=CLEAR+1;
  next=CLEAR+2;
  for (i=0;i<CLEAR;i++) {
    CTfirst[i]=i;    CTlast[i]=i;
    if (flag==1)     CTlink[i]=-1;
    else {
      CTlink[i]=-2;  CTnext[i]=-1;
    }
  }
  for (i=CLEAR;i<4096;i++) {
    CTlink[i]=-2;
    if (flag==2) CTnext[i]=-1;
  }
}

void AddStringToTable(int S,int K)              /*增加一个自由码表项*/
{
  CTlast[next] =CTfirst[K];
  CTlink[next] =S;
  CTfirst[next]=CTfirst[S];
  if (++next == max) {
    if (len < 12) {
      len++;	         max<<=1;
    }
  }
}

int  StringFromCode(int i,int K,BYTE *buff)         /*由代码查字符串*/
{
  while(K != -1) {
    buff[i++]=CTlast[K];   K=CTlink[K];
  }
  return(i);
}

int  WriteString(int xx,int i,BYTE *buff,BYTE *buffer)
						/*输出解压缩的字符串*/
{
  for (i--;i>=0;i--)  buffer[xx++]=buff[i];
  return(xx);
}

void unpack_LZWline(BYTE *buffer,int width,FILE *fp,int flag)
{                                                      /*LZW解码一行*/
  static int  size,K,S,xx;
  int  i;
  BYTE buff[1024];

  if (flag > 0) {                      /* flag > 0,LZW工作单元初始化*/
    CTfirst=(char*) malloc(4096);
	CTlast =(char*) malloc(4096);
	CTnext =(int *) malloc(4096*sizeof(int));
	CTlink =(int *) malloc(4096*sizeof(int));
    rement=0;        rem=0;             	    /*工作单元初始化*/
    index=0;
    S=-1;            xx=0;
    fread(&size,1,1,fp);                   		/*读处理尺寸*/
    InitStringTable(size,1);
  }
  else if (flag==0) {                                                 /*LZW解压缩*/
    if (xx >= width) {
      if (xx>width)
	for (i=0;i<xx-width;i++) buffer[i]=buffer[i+width];
      xx-=width;
    }
    while((K=LoadCode(len,fp)) != EOI) {          /*判断文件是否结束*/
      if (K == CLEAR) {                                 /*清除码处理*/
	    InitStringTable(size,1);                        /*码表初始化*/
	    S=-1;                                             /*前缀码空*/
      }
      else {
	    if (S != -1) {                                  /*前缀码非空*/
	      if (CTlink[K] == -2)                          /*代码非索引*/
	        AddStringToTable(S,S);                      /*增加新表项*/
	      else  AddStringToTable(S,K);
		}
	    i =StringFromCode(0,K,buff);                /*由代码查字符串*/
	    xx=WriteString(xx,i,buff,buffer);       /*输出解压缩的字符串*/
	    S=K;                                              /*置前缀码*/
	    if (xx >= width)  break;                        /*满一行退出*/
      }
    }
  }
  else {
    free(CTfirst);
	free(CTlast);
	free(CTnext);
	free(CTlink);
  }
}

void WriteCode(int code,int len,FILE *fp)    
{                                       /*编码时向数据流中写一个代码*/
  int  aln,used,cl;

  cl=len;                     aln=rement+len;
  while(aln >= 8) {
    if (rement > 0) {
      used=8-rement;
      strbuf[index++]=rem|(code<<rement);
      code=code >> used;
      cl -= used;             rement=0;
    }
    else {
      strbuf[index++]=code;
      cl -= 8;                code=code >> 8;
    }
    aln-=8;
    if (index == 256) {
      strbuf[0]=(char) 0xff;
      fwrite(strbuf,index,1,fp);
      index=1;
    }
  }
  if (rement == 0) {
    rem=code;                 rement=cl;
  }
  else {
    rem|=(code << rement);    rement+=cl;
  }
}

int IsInTable(int S,BYTE K)                   /*查字典并增加一个表项*/
{
  int  code;

  if (S==-1)  code=K;                               /*前缀码为空处理*/
  else if (CTlink[S]==-2) {                       /*前缀码不在字典中*/
    CTlink[S]=next;      CTlast[next++]=K;          /*字典中增加表项*/
    code=-1;
  }
  else {                                            /*前缀码在字典中*/
    code=CTlink[S];
    while((CTnext[code]!=-1) && (CTlast[code]!=K))          /*查字典*/
      code=CTnext[code];
    if (CTlast[code] != K) {                      /*新字节不在词汇中*/
      CTnext[code]=next;    CTlast[next++]=K;       /*字典中增加表项*/
      code=-1;
    }
  }
  return(code);
}

void pack_LZWline(FILE *fp,int width,BYTE *buffer,int flag)
{						       /*LZW压缩一行*/
  static int  size,S;
  BYTE   K,*ptr;
  int    j,SK;

  if (flag > 0) {                      /* flag > 0,LZW工作单元初始化*/
    CTfirst=(char*) malloc(4096);
	CTlast =(char*) malloc(4096);
	CTnext =(int *) malloc(4096*sizeof(int));
	CTlink =(int *) malloc(4096*sizeof(int));
	rement=0;        rem=0;
    index=1;
    S=-1;            size=flag;
    fwrite(&size,1,1,fp);
    InitStringTable(size,2);
    WriteCode(CLEAR,len,fp);                              /*发清除码*/
  }
  else if (flag==0) {                                      /*LZW压缩*/
    for (j=0,ptr=buffer;j<width;j++) {
      K=*ptr++;                                         /*读入一字节*/
      SK=IsInTable(S,K);                      /*查字典并增加一个表项*/
      if (SK != -1)  S=SK;                   /*前缀码为空码,置前缀码*/
      else {
	    WriteCode(S,len,fp);                              /*输出代码*/
	    S=K;                                    /*新输入字符做前缀码*/
      }
      if (next > max) {                             /*自由码增一处理*/
	    if (len == 12) {
	      WriteCode(S,len,fp);
	      WriteCode(CLEAR,len,fp);                        /*发清除码*/
	      InitStringTable(size,2);
	      S=-1;
        }
	    else {
	      len++;	      max=max<<1;
	      if (len == 12)      max--;
		}
      }
    }
  }
  else {                                     /*结束处理,释放工作单元*/
    WriteCode(S,len,fp);                            /*发最后一个代码*/
    WriteCode(EOI,len,fp);                              /*发结束代码*/
    if (rement != 0)  strbuf[index++]=rem;              /*剩余位处理*/
    if (index != 1) {
      strbuf[0]=(unsigned char) (index-1);
      fwrite(strbuf,index,1,fp);
    }
    fputc(0,fp);
    fputc(';',fp);                                      /*发结束标志*/
    free(CTfirst);
	free(CTlast);
	free(CTnext);
	free(CTlink);
  }
}

int loadGIFhead(char *file)                            /*读GIF文件头*/
{
  struct GIF_GLOBAL gifglobal;
  struct GIF_LOCAL  giflocal;
  FILE *fp;
  int  flag=0;

  fp=fopen(file,"rb");                                /*打开图像文件*/
  if (fp!=NULL) {
    flag=1;
    fseek(fp,0L,SEEK_SET);
    fread(&gifglobal,13,1,fp);                      /*读入全局信息表*/
    Width =gifglobal.screen_width;
    Height=gifglobal.screen_depth;
    BitCount =(gifglobal.global_flag & 0x7) + 1;
    ColorNum = 1 << BitCount;                           /*计算色彩数*/
    fread(T_pal,3,ColorNum,fp);                         /*读入调色板数据*/
    fread(&giflocal,10,1,fp);                           /*读入局部信息表*/
    fclose(fp);                                       /*关闭图像文件*/
	      /*  以下取出图像参数  */
    ColorMap = 13 ;
    OffBits  = 13+3*ColorNum+10;
    Planes       = 1;
    Compression  = 1;
    BytesPerLine = Width*BitCount/8;
    ImageType    = 0;
    Orientation  = 0;
  }
  return(flag);
}

void loadGIFimage(char *file,int px,int py,int Dx,int Dy,int sx,int sy)
{
  BYTE *buffer,*sc,*buf1;
  int  i,dw,dh;
  FILE *fp;

  i=loadGIFhead(file);                                 /*读GIF文件头*/
  if (i==0)  return;
  if (T_bitpp<=8) 
     set_allpalette(T_pal);       /*设置VGA调色板寄存器*/
  else { 
     px *= T_bpp;
     if (BitCount<=8)
        set_Vpalette((BYTE *)T_pal,(union COLOR_V *)bgr);   
  }

  dw=min(Width -px,Dx);        dw=min(dw,swide-sx);   /*确定视口尺寸*/
  dh=min(Height-py,Dy);        dh=min(dh,sdeep-sy);
  buffer=(BYTE*) malloc(4*Width);                                /*申请存储单元*/
  buf1=buffer;
  sc=(BYTE*) malloc(4*Width);                                /*申请存储单元*/

  fp=fopen(file,"rb");                                /*打开图像文件*/
  fseek(fp,OffBits,SEEK_SET);                         /*设置取数指针*/
  unpack_LZWline(buffer,Width,fp,1);                /*工作单元初始化*/
  for (i=0;i<Height;i++) {
    unpack_LZWline(buffer,Width,fp,0);                    /*解码一行*/
    if (T_bitpp>=24) {  
      memcpy(sc,buffer,dw);
      trans_8toColor(sc,Width);
	  buf1=sc;
	}
    if ((i>py)&&(i<py+dh))                            /*显示窗口控制*/
      put_image(sx,i+sy-py,dw,1,&buf1[px]);                /*显示一行*/
  }
  unpack_LZWline(buffer,Width,fp,-1);                 /*释放工作单元*/
  fclose(fp);                                         /*关闭图像文件*/

  free(sc);
  free(buffer);                                       /*释放存储单元*/
}

int saveGIFhead(char *file)                    	       /*写GIF文件头*/
{
  struct GIF_GLOBAL gifglobal = {
	   'G','I','F','8','7','a',0,0,0,0,0 };
  struct GIF_LOCAL  giflocal;
  FILE *fp;
  int  flag=0;
  BYTE *p;  

  gifglobal.screen_width = Width;                   /*生成信息头数据*/
  gifglobal.screen_depth = Height;
  gifglobal.global_flag = (BitCount-1) | 0xf0;

  get_allpalette(T_pal);                            /*取出调色板数据*/

  p=(BYTE*) &giflocal;
  p[0]=',';
  p[1]=p[2]=p[3]=p[4]=p[9]=0;
  p[5]=(BYTE) (Width  & 0xff);		
  p[6]=(BYTE) (Width >> 8);
  p[7]=(BYTE) (Height & 0xff);		
  p[8]=(BYTE) (Height >> 8);

  fp=fopen(file,"w+b");                               /*建立图像文件*/
  if (fp!=NULL) {
    flag=1;
    fseek(fp,0L,SEEK_SET);
    fwrite(&gifglobal,13,1,fp);        /*写全局信息头*/
	ColorNum = 1 << BitCount;                             /*计算色彩数*/
    fwrite(T_pal,3,ColorNum,fp); /*写调色板数据*/
    fwrite(&giflocal,10,1,fp);          /*写局部信息头*/
    fclose(fp);                                       /*关闭图像文件*/
    OffBits=13+3*ColorNum+10;
  }                                                 /*计算像数据位置*/
  return(flag);
}

void saveGIFimage(char *file,int x, int y,int Dx, int Dy,int bits)    
{                                                       /*写图像文件*/
  BYTE *buffer,ch=';';
  int  j;
  FILE *fp;

  BitCount = bits;
  Width=min(Dx,swide-x);   Height=min(Dy,sdeep-y);
  j=saveGIFhead(file);                                 /*写GIF文件头*/
  if (j==0)  return;

  buffer=malloc(1024);                                /*申请存储单元*/
  fp=fopen(file,"r+b");                               /*打开图像文件*/
  fseek(fp,OffBits,SEEK_SET);                         /*设置存取指针*/
  pack_LZWline(fp,Width,buffer,8);                  /*工作单元初始化*/
  for (j=0;j<Dy;j++) {
    get_image(x,y+j,Width,1,buffer);                /*取一行图像数据*/
    pack_LZWline(fp,Width,buffer,0);                      /*编码一行*/
  }
  pack_LZWline(fp,Width,buffer,-1);                   /*释放工作单元*/
  fwrite(&ch,1,1,fp);
  fclose(fp);                                         /*关闭图像文件*/
  free(buffer);                                       /*释放存储单元*/
}

int loadTGAhead(char *file) 			       /*读TGA文件头*/
{
  struct TGA_HEAD tgahead;
  int  i,j,flag=0;
  FILE *fp;
  BYTE *p;
 
  fp=fopen(file,"rb");   		              /*打开图像文件*/
  if (fp != NULL) {
    flag=1;
    fseek(fp,0L,SEEK_SET);
    fread(&tgahead,18,1,fp);      		        /*读入图像文件头*/
						                      /*以下取出图像参数*/
    p=(BYTE*) &tgahead;
    Width        = p[12]+256*p[13];      // tgahead.Width;
    Height       = p[14]+256*p[15];      // tgahead.Height;
    BitCount     = p[16];                // tgahead.Bits;
    BytesPerLine = Width * BitCount / 8;
    Compression  = p[2] & 0x8;           // tgahead.ImageType;
    ImageType    = p[2] & 0x3;
    Planes=1;
    ColorNum     = p[5]+256*p[6];
    OffBits=18;
    if (ImageType!=2) OffBits += 3*ColorNum;

    p=(BYTE *) T_pal;
    if (ImageType==1) {                    	        /*文件中有调色板*/
      fread(p,1,768,fp);                            /*读入调色板数据*/
      for (i=0;i<768;i+=3) {            	          /*红蓝分量交换*/
	    j=p[i];      p[i]=p[i+2];
	    p[i+2]=j;
      }
	}
	else if (ImageType==3) {                    /*黑白图像补上调色板*/
      for (i=0;i<256;i++) {
	     for (j=0;j<3;j++)  p[3*i+j]=i;
      }
      ColorNum = 256;
    }
    fclose(fp);                            	      /*关闭图像文件*/
  }
  return(flag);
}

void loadTGAimage(char *file,int px,int py,int Dx,int Dy,int sx,int sy)
{                                               /*读入并显示彩色图像*/
  int  i,dw,dh;
  FILE *fp;
  BYTE *sc;

  i=loadTGAhead(file);                                 /*读TGA文件头*/
  if (i==0)  return;

  dw=min(Width -px,Dx);     dw=min(dw,swide-sx);      /*确定视口尺寸*/
  dh=min(Height-py,Dy);     dh=min(dh,sdeep-sy);

  if (T_bitpp<=8) 
     set_allpalette(T_pal);       /*设置VGA调色板寄存器*/
  else { 
     px *= T_bpp;
     if (BitCount<=8)
        set_Vpalette((BYTE *)T_pal,(union COLOR_V *)bgr);   
  }
  sc=malloc(4*Width);                                 /*申请存储单元*/

  fp=fopen(file,"rb");                  	      /*打开图像文件*/
  fseek(fp,OffBits,SEEK_SET);              	      /*设置取数指针*/
  for (i=Height-1;i>=0;i--) {
    fread(sc,BytesPerLine,1,fp);                	/*读一行图像数据*/
    if (T_bitpp==32)  Trans_24to32(sc,3*Width);      /*32Bit显示模式*/

    if ((BitCount<=8)&&(T_bitpp>=24))
       trans_8toColor(sc,Width);

    if ((i>=py)&&(i<py+dh))
      put_image(sx,i+sy-py,dw,1,&sc[px]);            	  /*显示一行*/
  }
  fclose(fp);                                         /*关闭图像文件*/
  free(sc);                                           /*释放存储单元*/
}

int saveTGAhead(char *file)                            /*写TGA文件头*/
{
  struct TGA_HEAD tgahead;
  int  i,j,flag=0;                           	     /*TGA缺省文件头*/
  FILE *fp;
  BYTE *p;

  p=(BYTE*) &tgahead;
  memset(p,0,18);
  p[12]=Width & 0xff;	p[13]=Width>>8;  //  tgahead.Width
  p[14]=Height & 0xff;	p[15]=Height>>8; //  tgahead.Height
  p[16]=(BYTE) BitCount;                 //  tgahead.Bits
  OffBits=18;
  if (ImageType==1) {
    p[1]=p[2]=(BYTE)1;                   //  tgahead.ColorType
    p[5]=(BYTE)0;	p[6]=(BYTE)1;        //  tgahead.ColorLength
    p[7]=(BYTE)24;                       //  tgahead.ColorBit
	p[16]=(BYTE)8;
	OffBits += 768;;
  }
  else if (ImageType==2) {
    p[2]=(BYTE)2;	p[16]=(BYTE)24;
  }
  else {
    p[2]=(BYTE)3;	p[16]=(BYTE)8;
  }

  fp=fopen(file,"w+b");                               /*打开图像文件*/
  if (fp != NULL) {
    flag=1;
    fseek(fp,0L,SEEK_SET);
    fwrite(&tgahead,18,1,fp);      		        /*读入图像文件头*/
    if (ImageType==1) {
      get_allpalette(T_pal);         		      /*取调色板数据*/
      p=(BYTE *) T_pal;
      for (i=0;i<768;i+=3) {                 	      /*色调数据转换*/
	     j=p[i];      p[i]=p[i+2];
	     p[i+2]=j;
      }
      fwrite(T_pal,1,768,fp);         		      /*写调色板数据*/
    }
    fclose(fp);                            	      /*关闭图像文件*/
  }
  return(flag);
}

void saveTGAimage(char *file,int x,int y,int Dx,int Dy,int bits)
{
  int  i,n;
  FILE *fp;
  BYTE *sc;

  BitCount = bits;
  Width=min(Dx,swide-x);   Height=min(Dy,sdeep-y);
  i=saveTGAhead(file);                                 /*写TGA文件头*/
  if (i==0)  return;

  sc=malloc(4*Width);                                 /*申请存储单元*/

  n=1;
  if (T_bitpp>16)  n=3;
  fp=fopen(file,"r+b");                               /*打开图像文件*/
  fseek(fp,OffBits,SEEK_SET);                         /*设置存数指针*/
  for (i=Dy-1;i>-1;i--) {
    get_image(x,y+i,Width,1,sc);                    /*读一行图像数据*/
    if (T_bitpp==32)  Trans_32to24(sc,Width);
					  /*32Bit数据转变为24Bit数据*/
    fwrite(sc,n,Width,fp);                    	  /*图像数据存入文件*/
  }
  fclose(fp);                                 	      /*关闭图像文件*/
  free(sc);                                   	      /*释放存储单元*/
}

int  LoadFileHead(char *file)
{
   int  m,n;

   strupr(file);
   n=strlen(file);
   if (strcmp(&file[n-3],"BMP")==0)	
	   m=loadBMPhead(file);
   else if (strcmp(&file[n-3],"PCX")==0)
	   m=loadPCXhead(file); 	
   else if (strcmp(&file[n-3],"GIF")==0)
	   m=loadGIFhead(file); 	
   else if (strcmp(&file[n-3],"TGA")==0)
	   m=loadTGAhead(file); 	
   else if (strcmp(&file[n-3],"TIF")==0)
	   m=loadTIFhead(file); 	
   return m;
}

void LoadFileImage(char *file,int px,int py,int Dx,int Dy,int sx,int sy)
{
   int  n;

   strupr(file);
   n=strlen(file);
   if (strcmp(&file[n-3],"BMP")==0)	
	   loadBMPimage(file,px,py,Dx,Dy,sx,sy);
   else if (strcmp(&file[n-3],"PCX")==0)
	   loadPCXimage(file,px,py,Dx,Dy,sx,sy); 	
   else if (strcmp(&file[n-3],"GIF")==0)
	   loadGIFimage(file,px,py,Dx,Dy,sx,sy); 	
   else if (strcmp(&file[n-3],"TGA")==0)
	   loadTGAimage(file,px,py,Dx,Dy,sx,sy); 	
   else if (strcmp(&file[n-3],"TIF")==0)
	   loadTIFimage(file,px,py,Dx,Dy,sx,sy); 	
}

int  SaveFileHead(char *file)
{
   int  m,n;

   strupr(file);
   n=strlen(file);
   if (strcmp(&file[n-3],"BMP")==0)	
	   m=saveBMPhead(file);
   else if (strcmp(&file[n-3],"PCX")==0)
	   m=savePCXhead(file); 	
   else if (strcmp(&file[n-3],"GIF")==0)
	   m=saveGIFhead(file); 	
   else if (strcmp(&file[n-3],"TGA")==0)
	   m=saveTGAhead(file); 	
   else if (strcmp(&file[n-3],"TIF")==0)
	   m=saveTIFhead(file); 	
   return m;
}

void SaveFileImage(char *file,int x,int y,int Dx,int Dy,int bits)
{
   int  n;

   strupr(file);
   n=strlen(file);
   if (strcmp(&file[n-3],"BMP")==0)	
	   saveBMPimage(file,x,y,Dx,Dy,bits);
   else if (strcmp(&file[n-3],"PCX")==0)
	   savePCXimage(file,x,y,Dx,Dy,bits); 	
   else if (strcmp(&file[n-3],"GIF")==0)
	   saveGIFimage(file,x,y,Dx,Dy,bits); 	
   else if (strcmp(&file[n-3],"TGA")==0)
	   saveTGAimage(file,x,y,Dx,Dy,bits); 	
   else if (strcmp(&file[n-3],"TIF")==0)
	   saveTIFimage(file,x,y,Dx,Dy,bits); 	
}

