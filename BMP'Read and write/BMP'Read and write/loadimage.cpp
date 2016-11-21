/*    LOADBMP_W.C    */
/*     2002.01.16    */

#include "vesa_w.h"
#include "file_w.h"
#include "font_w.h"

int loadBMPhead(char *file)                        /*  ��BMP�ļ�ͷ  */
{
  BITMAPFILEHEADER  bmphead;
  BITMAPINFOHEADER  bmi;
  FILE *fp;
  int  flag=0;
  DWORD dd;
  
  fp=fopen(file,"rb");                            /*  ��ͼ���ļ�  */
  if (fp != NULL) {
    flag=1;
    fseek(fp,0L,SEEK_SET);
    fread(&bmphead,sizeof(BITMAPFILEHEADER),1,fp);/*  ��λͼ�ļ�ͷ  */
    fread(&bmi,sizeof(BITMAPINFOHEADER),1,fp);    /*  ��λͼ��Ϣͷ  */
 					      /*  ����ȡ��ͼ�����  */
    OffBits      = bmphead.bfOffBits;
    Width        = (WORD) bmi.biWidth;
    Height       = (WORD) bmi.biHeight;
    BitCount     = bmi.biBitCount;
    Planes       = bmi.biPlanes;
    Compression  = (WORD) bmi.biCompression;
    BytesPerLine = (Width*BitCount+31)/32*4;    /*  ����ÿ���ֽ���  */
    ColorMap     = sizeof(BITMAPFILEHEADER) + bmi.biSize;
    ColorNum     = 0;

    if (BitCount<=8) {
      dd=bmi.biClrUsed;
	  if ((dd>0)&&(dd<256)) ColorNum = (WORD) dd;
	  else                  ColorNum = 1 << BitCount;
      fseek(fp,ColorMap,SEEK_SET);
      fread((BYTE*) bgr,4,ColorNum,fp);         /*  �����ɫ������  */
      bgr2vga(bgr,T_pal,ColorNum);
    }
    fclose(fp);                                   /*  �ر�ͼ���ļ�  */
  }
  return(flag);
}

void loadBMPimage(char *file,int px,int py,int Dx,int Dy,int sx,int sy)
{                                                      /*��ʾBMPͼ��*/
  int  i,dw,dh;
  BYTE *sc;
  FILE *fp;

  i=loadBMPhead(file);                                 /*��BMP�ļ�ͷ*/
  if (i==0)  return;

  dw=min(Width -px,Dx);    dw=min(dw,swide-sx);       /*ȷ���ӿڳߴ�*/
  dh=min(Height-py,Dy);    dh=min(dh,sdeep-sy);
  if (T_bitpp<=8) set_allpalette(T_pal);       /*����VGA��ɫ��Ĵ���*/
  else            px *= T_bpp;     /*����ÿ�����ֽ���,�������ɫ��ʾ*/
  sc = malloc(4*Width);                               /*����洢��Ԫ*/

  fp=fopen(file,"rb");                                /*��ͼ���ļ�*/
  fseek(fp,OffBits,SEEK_SET);                         /*����ȡ��ָ��*/
  for(i=Height-1;i>=0;i--) {
    fread(sc,1,BytesPerLine,fp);                        /*��һ������*/

    if (BitCount==1)  Trans_1to8(sc,BytesPerLine);      /*�Ƕ�ֵͼ��*/
    else if (BitCount==4)  Trans_4cto8(sc,BytesPerLine);
							/*��16ɫͼ��*/
    else if ((BitCount==24)&&(T_bitpp==32))
       Trans_24to32(sc,BytesPerLine);
			 /*  �� 32λ���ɫ��ʾģʽ������и�ʽת��  */
    if ((BitCount<=8)&&(T_bitpp>=24)) 
       trans_8toColor(sc,Width);

    if ((i>=py)&&(i<py+dh))
      put_image(sx,i+sy-py,dw,1,&sc[px]);             /*��ʾһ��ͼ��*/
  }
  fclose(fp);                                         /*�ر�ͼ���ļ�*/
  free(sc);                                           /*�ͷŴ洢��Ԫ*/
}

int saveBMPhead(char *file)                        /*  дBMP�ļ�ͷ  */
{
  BITMAPFILEHEADER  bmphead = {
	    0x4d42,0,0,0,0  } ;               /*  λͼ�ļ�ͷȱʡֵ  */
  BITMAPINFOHEADER  bmi = {
	   40,0,0,1,0,0,0,0,0,0,0  } ;        /*  λͼ��Ϣͷȱʡֵ  */
  FILE *fp;
  int  flag=0;

  BytesPerLine=(Width*BitCount+31)/32*4;        /*  ����ÿ���ֽ���  */
  ColorMap = 54;
  OffBits  = 54;
  if (BitCount<=8) {
    ColorNum= 1 << BitCount;
    OffBits = 54 + 4*ColorNum;            /*  �����������ݴ��λ��  */
  }

  fp=fopen(file,"w+b");                           /*  ����ͼ���ļ�  */
  if (fp != NULL) {
    flag=1;
    bmi.biWidth        = Width;                   /*  ����ͼ�����  */
    bmi.biHeight       = Height;
    bmi.biBitCount     = BitCount;
    bmphead.bfOffBits = (DWORD) OffBits;

    fwrite(&bmphead,sizeof(BITMAPFILEHEADER),1,fp);
	                                              /*  дλͼ�ļ�ͷ  */
    fwrite(&bmi,sizeof(BITMAPINFOHEADER),1,fp);   /*  дλͼ��Ϣͷ  */
    if (BitCount<=8) {
      get_allpalette(T_pal);                      /*  ȡ��ɫ������  */
      vga2bgr(T_pal,bgr,ColorNum);
      fwrite(bgr,4,ColorNum,fp);                           /*  дɫ������  */
    }
    fclose(fp);                                   /*  �ر�ͼ���ļ�  */
  }
  return(flag);
}

void saveBMPimage(char *file,int x,int y,int Dx,int Dy,int bits)
{                                                /*  ��BMPͼ���ļ�  */
  int  i;
  BYTE *sc;
  FILE *fp;

  BitCount = bits;
  Width=min(Dx,swide-x);   Height=min(Dy,sdeep-y);
						  /*  ȷ��ͼ�񴰿�  */
  i=saveBMPhead(file);                             /*  дBMP�ļ�ͷ  */
  if (i==0)  return;                           /*  �ļ�������,����  */
 
  sc = malloc(4*Width);                           /*  ����洢��Ԫ  */
  fp=fopen(file,"r+b");                           /*  ��ͼ���ļ�  */
  fseek(fp,OffBits,SEEK_SET);                     /*  ���ô���ָ��  */
  for(i=Height-1;i>=0;i--) {
    get_image(x,i+y,Width,1,sc);                /*  ȡһ��ͼ������  */

    if (BitCount==1)      Trans_8to1(sc,Width);
					    /*  ת���ɶ�ֵͼ������  */
    else if (BitCount==4) Trans_8to4c(sc,Width);/*  ת����16ɫͼ��  */
    else if (T_bitpp==32) Trans_32to24(sc,Width);
				        /*  32λ����ת����24λ����  */
    fwrite(sc,1,BytesPerLine,fp);               /*  ��һ��ͼ������  */
  }
  fclose(fp);                                     /*  �ر�ͼ������  */
  free(sc);                                       /*  �ͷŴ洢��Ԫ  */
}

int loadTIFhead(char *file)                    	       /*��TIF�ļ�ͷ*/
{
  struct TIF_IFH head;
  struct TIF_DE dir[25];
  FILE *fp;
  WORD tif_pal[768];
  WORD i,j,k,m,n,t,nn,c1,c2,flag=0;
  BYTE *p;

  fp=fopen(file,"rb");				      /*��ͼ���ļ�*/
  if (fp!=NULL) {
    flag=1;
    ColorMap=0;
    fread(&head,8,1,fp);			      /*��ͼ���ļ�ͷ*/
    fseek(fp,head.FirstIFD,SEEK_SET);
    fread(&nn,2,1,fp);			            /*���ļ�Ŀ¼����*/
    fread(dir,12,nn,fp);			      /*�����ļ�Ŀ¼*/
    for (i=0;i<nn;i++) {           /*���ļ�Ŀ¼��ȡ��TIFͼ���ļ�����*/
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
	  fread(tif_pal,2,n,fp);	         /*����TIF��ɫ������*/
	  p=(BYTE *) T_pal;    n/=3;
	  for (m=0;m<n;m++)   			      /*ɫ������ת��*/
	    for (j=0,k=0;j<3;j++,k+=n)
	      p[3*m+j] = (tif_pal[m+k] >> 8) & 0xff;
          break;
	default :                    break;
      }
      if (ImageType<2) {		    /*���úڰ�ͼ���ɫ������*/
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
    fclose(fp);					      /*�ر�ͼ���ļ�*/
    if (BitCount==1)       BytesPerLine=(Width+7)/8;/*����ÿ���ֽ���*/
    else if (BitCount==4)  BytesPerLine=(Width+1)/2;
    else if (BitCount==8)  BytesPerLine=Width;
    else if (BitCount==24) BytesPerLine=3*Width;
    if (BitCount<=8) ColorNum=1<<BitCount;
	Planes=1;
  }
  return(flag);
}

void loadPackBitsline(BYTE *p,int wid,FILE *fp) /*����λ�������һ��*/
{
  WORD n=0,i,c;
  char ch;

  do{
    ch=fgetc(fp);					  /*ȡһ�ֽ�*/
    if (ch < 0) {				  /*�Ǹ���Ϊ�г̱���*/
      i=-ch+1;					          /*���г�ֵ*/
      c=fgetc(fp);				        /*ȡ������ֵ*/
      while(i--)  p[n++]=(BYTE) c;	/*������ֵ�ظ����ɴ�*/
    }
    else {				          /*����Ϊԭ��������*/
      i= ch+1;						  /*���ֽ���*/
      while(i--)  p[n++]=fgetc(fp);			  /*��������*/
    }
  }
  while(n<wid);				              /*����һ�м���*/
}

void loadTIFimage(char *file,int px,int py,int Dx,int Dy,int sx,int sy)
{						       /*��ʾTIFͼ��*/
  FILE *fp;
  int  i,j,k,m,n,dw,dh;
  BYTE *sc;

  i=loadTIFhead(file);				       /*��TIF�ļ�ͷ*/
  if (i==0)  return;

  dw=min(Width -px,Dx);   dw=min(dw,swide-sx); 	      /*ȷ���ӿڳߴ�*/
  dh=min(Height-py,Dy);   dh=min(dh,sdeep-sy);
  if (T_bitpp<=8) 
     set_allpalette(T_pal);       /*����VGA��ɫ��Ĵ���*/
  else { 
     px *= T_bpp;
     if (BitCount<=8)
        set_Vpalette((BYTE *)T_pal,(union COLOR_V *)bgr);   
  }
  sc=malloc(4*Width);				      /*����洢��Ԫ*/

  fp=fopen(file,"rb");				      /*��ͼ���ļ�*/
  fseek(fp,OffBits,SEEK_SET);
  for (i=0;i<Height;i++) {
    if (Compression==1)
      fread(sc,1,BytesPerLine,fp);		    /*��ѹ�����ݶ���*/
    else if (Compression==32773U)
      loadPackBitsline(sc,BytesPerLine,fp);   	/*����λ�������һ��*/

    if (BitCount==1)      Trans_1to8(sc,BytesPerLine);/*��ʾ��ʽת��*/
    else if (BitCount==4) Trans_4cto8(sc,BytesPerLine);
    else if (BitCount==24) {
      n=3*(Width-1);    k=T_bpp*(Width-1);
      for (j=0;j<Width;j++,n-=3,k-=T_bpp) {
	    m=sc[n+2];
	    if (T_bitpp==32)  sc[k+1]=sc[n+1];
	    sc[k+2]=sc[n];    sc[k]=m;		      /*������������*/
      }
    }
    if ((BitCount<=8)&&(T_bitpp>=24))
       trans_8toColor(sc,Width);
 
    if ((i>=py)&&(i<py+dh))
      put_image(sx,i+sy-py,dw,1,&sc[px]);			  /*��ʾһ��*/
  }
  fclose(fp);					      /*�ر�ͼ���ļ�*/
  free(sc);					      /*�ͷŴ洢��Ԫ*/
}

int saveTIFhead(char *file)			       /*дTIF�ļ�ͷ*/
{
  struct TIF_IFH head;
  struct TIF_DE dir[]={				   /*TIF�ļ�ȱʡ����*/
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

  fp=fopen(file,"w+b"); 			    /*��ͼ���ļ�ͷ*/
  if (fp!=NULL) {
    flag=1;
    memset(head.ByteOrder,'I',2);
    head.Version  = 0x2a;     head.FirstIFD  = 8;
						  /*����ͼ���ļ�����*/
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
    fwrite(&head,8,1,fp);			      /*дͼ���ļ�ͷ*/
    fwrite(&n,2,1,fp);				      /*дͼ��Ŀ¼��*/
    fwrite(dir,12,n,fp);        			/*дͼ��Ŀ¼*/
    fwrite(&ll,4,1,fp);				  /*ָ����һͼ��Ŀ¼*/
    if (BitCount==24) {		      /*�����ɫͼ���ÿ��������λ��*/
      for (j=0,m=8;j<3;j++)
	    fwrite(&m,2,1,fp);
    }
    if (dir[4].ValOff==3) {
      get_allpalette(T_pal);		              /*ȡ��ɫ������*/
      p=(BYTE *) T_pal;
      k=256;
      if (BitCount==4)  k=16;
      for (i=0;i<3;i++) {			      /*ɫ������ת��*/
	    for (j=0;j<k;j++) {
	      n=((int) p[3*j+i]) & 0xff;    m=n | (n<<8);
	      tif_pal[k*i+j]=m;
		}
      }
      if (BitCount==8)
	    fwrite(tif_pal,2,768,fp);
      else if (BitCount==4)                      	   /*дTIF��ɫ������*/
	    fwrite(tif_pal,2,48,fp);
    }
    fclose(fp);					      /*�ر�ͼ���ļ�*/
    if (BitCount==1)       BytesPerLine=(Width+7)/8;  /*����ÿ�ֽ���*/
    else if (BitCount==4)  BytesPerLine=(Width+1)/2;
    else if (BitCount==8)  BytesPerLine=Width;
    else if (BitCount==24) BytesPerLine=3*Width;
  }
  return(flag);
}

void savePackBitsline(BYTE *buf,int pwide,FILE *fp)
{				              /*�ý���λ�������һ��*/
  int   j,k,l,gray,i=0,n=0;
  char  buf1[1000],buf2[200];

  for (j=0;j<pwide;j++) {			          /*����һ��*/
    gray=buf[j];              k=1;	              /*ȡһ��������*/
    j++;
    while((j<pwide)&&(gray==buf[j])) {		      /*��ֵ�ظ�����*/
      k++;           j++;
    }
    j--;
    while(k>128) {				   /*�г̳���128�ض�*/
      buf1[n++]=(BYTE) 0xff;         buf1[n++]=gray;
      k-=128;
    }
    if (k==1) buf2[i++]=gray;		        /*ÿ�ֽ������뻺����*/
    else if ((k==2)&&(i>0)) {		  /*�г�Ϊ2,ͬʱ������������*/
      buf2[i++]=gray;  buf2[i++]=gray;              /*���ݴ��뻺����*/
      k=0;
    }
    if (k>1) {					         /*�г̴���1*/
      if (i>0) {				  /*���������������*/
	    buf1[n++]=i-1;
	    for (l=0;l<i;l++)  buf1[n++]=buf2[l];
	    i=0;
      }
      buf1[n++]=-(k-1);   buf1[n++]=gray;		  /*�г̱���*/
    }
  }
  if (i>0) {				        /*������ʣ���������*/
    buf1[n++]=i-1;
    for (l=0;l<i;l++)  buf1[n++]=buf2[l];
  }
  fwrite(buf1,1,n,fp);					  /*���ݴ���*/
}

void saveTIFimage(char *file,int x,int y,int Dx,int Dy,int bits)
{					             /*дTIFͼ���ļ�*/
  FILE *fp;
  WORD i,j,k,n,t;
  BYTE *sc;

  BitCount = bits;
  Width=min(Dx,swide-x);   Height=min(Dy,sdeep-y);
  i=saveTIFhead(file);				       /*дTIF�ļ�ͷ*/
  if (i==0)  return;

  sc=malloc(4*Width);				      /*����洢��Ԫ*/
  fp=fopen(file,"r+b");				      /*��ͼ���ļ�*/
  fseek(fp,OffBits,SEEK_SET);			      /*���ô���ָ��*/
  for (i=0;i<Height;i++) {
    get_image(x,y+i,Width,1,sc);                  /*����һ��ͼ������*/

    if (BitCount==1)       Trans_8to1(sc,Width);    /*ת���ɶ�ֵͼ��*/
    else if (BitCount==4)  Trans_8to4c(sc,Width);/*ת����16ɫ��ɫͼ��*/
    else if (T_bitpp > 16) {			        /*���ɫͼ��*/
      for (j=0,n=0,k=0;j<Width;j++,n+=3,k+=T_bpp) {
	    t=sc[k];
	    if (T_bitpp==32)  sc[n+1]=sc[k+1];
	    sc[n]=sc[k+2];    sc[n+2]=(BYTE) t;		      /*������������*/
      }
    }

    if (Compression==1)
      fwrite(sc,1,BytesPerLine,fp);	          /*��ѹ��, ֱ�Ӵ���*/
    else if (Compression==32773U)
      savePackBitsline(sc,BytesPerLine,fp);           /*������һ��*/
  }
  fclose(fp);					      /*�ر�ͼ���ļ�*/
  free(sc);					      /*�ͷŴ洢��Ԫ*/
}

int loadPCXhead(char *file)                            /*��PCX�ļ�ͷ*/
{
  struct PCX_HEAD PCX_head;
  FILE *fp;
  int  flag=0;

  fp=fopen(file,"rb");                                /*��ͼ���ļ�*/
  if (fp!=NULL) {
    flag=1;
    fread(&PCX_head,128,1,fp);                         /*��PCX�ļ�ͷ*/
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
      fread(T_pal,1,48,fp);                     /*����16ɫ��ɫ������*/
    }
    if ((Planes==1)&&(BitCount==8)) {
      fseek(fp,-768,SEEK_END);
      fread(T_pal,1,768,fp);                   /*����256ɫ��ɫ������*/
    }
    fclose(fp);                                       /*�ر�ͼ���ļ�*/
  }
  return(flag);
}

void loadPCXline(BYTE *p,int wid,FILE *fp)
{                                          /*PCXͼ���ļ��г̱���һ��*/
  int n=0,i,c;
 
  memset(p,0,wid);
  do{
    c=fgetc(fp);                                      /*����һ���ַ�*/
    if((c&0xc0)==0xc0) {                  /*����λΪ1ʱ,���ֽ�Ϊ�г�*/
      i=c&0x3f;                                 /*���ε�����λ���г�*/
      c=fgetc(fp);                                        /*ȡ��ɫ��*/
      while(i--)  p[n++]=c;                           /*�ָ���������*/
    }
    else  p[n++]=c;                                   /*������������*/
  }
  while(n<wid);                                  /*һ��δ��,��������*/
 }

void loadPCXimage(char *file,int px,int py,int Dx,int Dy,int sx,int sy)
{                                   /*256ɫ�����ɫģʽ����ʾPCXͼ��*/
  FILE *fp;
  int  i,j,k,dw,dh,bpl;
  BYTE *sc,*buf,*buf1;

  i=loadPCXhead(file);                                 /*��PCX�ļ�ͷ*/
  if (i==0)  return;

  dw=min(Width -px,Dx);    dw=min(dw,swide-sx);       /*ȷ���ӿڳߴ�*/
  dh=min(Height-py,Dy);    dh=min(dh,sdeep-sy);
  if (T_bitpp<=8) 
     set_allpalette(T_pal);       /*����VGA��ɫ��Ĵ���*/
  else { 
     px *= T_bpp;
     if (BitCount*Planes<=8)
        set_Vpalette((BYTE *)T_pal,(union COLOR_V *)bgr);   
  }
  sc =malloc(3*Width);                                /*����洢��Ԫ*/
  buf=malloc(4*Width);
  buf1=buf;         

  fp=fopen(file,"rb");                                /*��ͼ���ļ�*/
  fseek(fp,128,SEEK_SET);                         /*ָ����������λ��*/
  bpl=BytesPerLine * Planes;                      /*����ÿ�����ֽ���*/
  for(i=0;i<Height;i++) {
    if (!feof(fp)) {
      loadPCXline(sc,bpl,fp);                             /*����һ��*/

      if (Planes==1) {              /*��λƽ��ͼ��,Ϊ��ֵ��256ɫ��ɫ*/
	     if (BitCount==1)  Trans_1to8(sc,BytesPerLine);
	     buf1=sc;
      }
      else if (Planes==4) {                           /*16ɫ��ɫͼ��*/
	     Trans_4pto8(sc,buf,BytesPerLine);
	  }
      else if (Planes==3) {                             /*���ɫͼ��*/
	     for (j=0,k=0;j<BytesPerLine;j++,k+=T_bpp) {
	        buf[k+2]=sc[j];
	        buf[k+1]=sc[BytesPerLine+j];
	        buf[k]  =sc[2*BytesPerLine+j];
		 }
	  }
      if ((Planes*BitCount<=8)&&(T_bitpp>=24))
         trans_8toColor(buf1,Width);
    
      if ((i>=py)&&(i<py+dh))                          /*��ʾһ��ͼ��*/
	     put_image(sx,i+sy-py,dw,1,&buf1[px]);
	}
    else break;
  }
  fclose(fp);                                         /*�ر�ͼ���ļ�*/
  free(sc);                                           /*�ͷŴ洢��Ԫ*/
  free(buf);
}

int savePCXhead(char *file)                            /*дPCX�ļ�ͷ*/
{
  struct PCX_HEAD PCX_head;
  BYTE *p;
  int  i,flag=0;
  FILE *fp;

  p=(BYTE *) &PCX_head;
  for (i=0;i<128;i++) (*p++) = 0;
  if (BitCount==1)  BytesPerLine = (Width+7)/8;     /*����ÿ���ֽ���*/
  else              BytesPerLine = Width;
  BytesPerLine = (BytesPerLine+1)/2*2;              /*����ÿ���ֽ���*/
  fp=fopen(file,"w+b");                               /*����ͼ���ļ�*/
  if (fp!=NULL) {                               /*�γ�ͼ���ļ�ͷ����*/
    flag=1;
    PCX_head.Header       = 10;
    PCX_head.Version      = 5;
    PCX_head.Encode       = 1;
    PCX_head.BitPerPixel  = (BYTE) BitCount;
    PCX_head.X2           = Width-1;
    PCX_head.Y2           = Height-1;
    PCX_head.NumOfPlanes  = (BYTE) Planes;
    PCX_head.BytesPerLine = BytesPerLine;
    fwrite(&PCX_head,128,1,fp);                       /*дͼ���ļ�ͷ*/
    if (BitCount==1) {
      get_allpalette(T_pal);
      fseek(fp,0x10,SEEK_SET);
      fwrite(T_pal,48,1,fp);                      /*д16ɫ��ɫ������*/
    }
    fclose(fp);
  }                                                   /*�ر�ͼ���ļ�*/
  return(flag);
}

void savePCXline(BYTE *buf,int pwide,FILE *fp)
{                                          /*PCXͼ���ļ��г̱���һ��*/
  int   j,k,gray,n=0;
  BYTE  buf1[1000];

  for (j=0;j<pwide;j++) {
    gray=buf[j];              k=1;                      /*ȡһ������*/
    j++;
    while((j<pwide)&&(gray==buf[j])) {                    /*����г�*/
      k++;                    j++;
    }
    j--;
    while(k>63) {                             /*����63���г̷ֳɼ���*/
      buf1[n++]=0xff;         buf1[n++]=gray;
      k-=63;
    }
    if (k==1)                                         /*�����г̴���*/
      if (gray<0xc0)          buf1[n++]=gray;     /*С��0XC0�õ��ֽ�*/
      else {                                         /*����,��˫�ֽ�*/
	    buf1[n++]=0xc1;       buf1[n++]=gray;
      }
    else { buf1[n++]=0xc0+k;  buf1[n++]=gray;   }         /*���г���*/
  }
  fwrite(buf1,1,n,fp);
}

void savePCXimage(char *file,int x, int y,int Dx, int Dy,int bits)
{                                                    /*��PCXͼ���ļ�*/
  int i,j,k,bpl,P_flag=12;
  BYTE *sc,*buf,*buf1;
  FILE *fp;

  BitCount = bits;
  Width=min(Dx,swide-x);   Height=min(Dy,sdeep-y);    /*ȷ��ͼ�񴰿�*/
  i=savePCXhead(file);                                 /*дPCX�ļ�ͷ*/
  if (i==0)  return;

  bpl=BytesPerLine*Planes;                        /*����ÿ�����ֽ���*/
  sc =malloc(4*Width);                                /*����洢��Ԫ*/
  buf=malloc(3*Width);

  buf1=buf;
  fp=fopen(file,"r+b");                               /*��ͼ���ļ�*/
  fseek(fp,0x80,SEEK_SET);                        /*ָ�����ش��λ��*/
  for(i=0;i<Height;i++) {
    get_image(x,i+y,Width,1,sc);                    /*ȡһ��ͼ������*/

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

    savePCXline(buf1,bpl,fp);                       /*�������г̱���*/
  }
  if ((Planes==1)&&(BitCount==8)) {
    get_allpalette(T_pal);
    fseek(fp,0,SEEK_CUR);
    fwrite(&P_flag,1,1,fp);
    fwrite(T_pal,768,1,fp);                      /*д256ɫ��ɫ������*/
  }
  fclose(fp);                                         /*�ر�ͼ���ļ�*/
  free(sc);                                           /*�ͷŴ洢��Ԫ*/
  free(buf);
}

int   index,rement;
char  rem;
char  *CTfirst,*CTlast,strbuf[256];
int   *CTnext,*CTlink,CLEAR,EOI,len,max,next;

int  LoadCode(int len,FILE *fp)         /*����ʱ���������ж�һ������*/
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

void InitStringTable(int size,int flag)             /*��ʼ���ַ�����*/
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

void AddStringToTable(int S,int K)              /*����һ�����������*/
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

int  StringFromCode(int i,int K,BYTE *buff)         /*�ɴ�����ַ���*/
{
  while(K != -1) {
    buff[i++]=CTlast[K];   K=CTlink[K];
  }
  return(i);
}

int  WriteString(int xx,int i,BYTE *buff,BYTE *buffer)
						/*�����ѹ�����ַ���*/
{
  for (i--;i>=0;i--)  buffer[xx++]=buff[i];
  return(xx);
}

void unpack_LZWline(BYTE *buffer,int width,FILE *fp,int flag)
{                                                      /*LZW����һ��*/
  static int  size,K,S,xx;
  int  i;
  BYTE buff[1024];

  if (flag > 0) {                      /* flag > 0,LZW������Ԫ��ʼ��*/
    CTfirst=(char*) malloc(4096);
	CTlast =(char*) malloc(4096);
	CTnext =(int *) malloc(4096*sizeof(int));
	CTlink =(int *) malloc(4096*sizeof(int));
    rement=0;        rem=0;             	    /*������Ԫ��ʼ��*/
    index=0;
    S=-1;            xx=0;
    fread(&size,1,1,fp);                   		/*������ߴ�*/
    InitStringTable(size,1);
  }
  else if (flag==0) {                                                 /*LZW��ѹ��*/
    if (xx >= width) {
      if (xx>width)
	for (i=0;i<xx-width;i++) buffer[i]=buffer[i+width];
      xx-=width;
    }
    while((K=LoadCode(len,fp)) != EOI) {          /*�ж��ļ��Ƿ����*/
      if (K == CLEAR) {                                 /*����봦��*/
	    InitStringTable(size,1);                        /*����ʼ��*/
	    S=-1;                                             /*ǰ׺���*/
      }
      else {
	    if (S != -1) {                                  /*ǰ׺��ǿ�*/
	      if (CTlink[K] == -2)                          /*���������*/
	        AddStringToTable(S,S);                      /*�����±���*/
	      else  AddStringToTable(S,K);
		}
	    i =StringFromCode(0,K,buff);                /*�ɴ�����ַ���*/
	    xx=WriteString(xx,i,buff,buffer);       /*�����ѹ�����ַ���*/
	    S=K;                                              /*��ǰ׺��*/
	    if (xx >= width)  break;                        /*��һ���˳�*/
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
{                                       /*����ʱ����������дһ������*/
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

int IsInTable(int S,BYTE K)                   /*���ֵ䲢����һ������*/
{
  int  code;

  if (S==-1)  code=K;                               /*ǰ׺��Ϊ�մ���*/
  else if (CTlink[S]==-2) {                       /*ǰ׺�벻���ֵ���*/
    CTlink[S]=next;      CTlast[next++]=K;          /*�ֵ������ӱ���*/
    code=-1;
  }
  else {                                            /*ǰ׺�����ֵ���*/
    code=CTlink[S];
    while((CTnext[code]!=-1) && (CTlast[code]!=K))          /*���ֵ�*/
      code=CTnext[code];
    if (CTlast[code] != K) {                      /*���ֽڲ��ڴʻ���*/
      CTnext[code]=next;    CTlast[next++]=K;       /*�ֵ������ӱ���*/
      code=-1;
    }
  }
  return(code);
}

void pack_LZWline(FILE *fp,int width,BYTE *buffer,int flag)
{						       /*LZWѹ��һ��*/
  static int  size,S;
  BYTE   K,*ptr;
  int    j,SK;

  if (flag > 0) {                      /* flag > 0,LZW������Ԫ��ʼ��*/
    CTfirst=(char*) malloc(4096);
	CTlast =(char*) malloc(4096);
	CTnext =(int *) malloc(4096*sizeof(int));
	CTlink =(int *) malloc(4096*sizeof(int));
	rement=0;        rem=0;
    index=1;
    S=-1;            size=flag;
    fwrite(&size,1,1,fp);
    InitStringTable(size,2);
    WriteCode(CLEAR,len,fp);                              /*�������*/
  }
  else if (flag==0) {                                      /*LZWѹ��*/
    for (j=0,ptr=buffer;j<width;j++) {
      K=*ptr++;                                         /*����һ�ֽ�*/
      SK=IsInTable(S,K);                      /*���ֵ䲢����һ������*/
      if (SK != -1)  S=SK;                   /*ǰ׺��Ϊ����,��ǰ׺��*/
      else {
	    WriteCode(S,len,fp);                              /*�������*/
	    S=K;                                    /*�������ַ���ǰ׺��*/
      }
      if (next > max) {                             /*��������һ����*/
	    if (len == 12) {
	      WriteCode(S,len,fp);
	      WriteCode(CLEAR,len,fp);                        /*�������*/
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
  else {                                     /*��������,�ͷŹ�����Ԫ*/
    WriteCode(S,len,fp);                            /*�����һ������*/
    WriteCode(EOI,len,fp);                              /*����������*/
    if (rement != 0)  strbuf[index++]=rem;              /*ʣ��λ����*/
    if (index != 1) {
      strbuf[0]=(unsigned char) (index-1);
      fwrite(strbuf,index,1,fp);
    }
    fputc(0,fp);
    fputc(';',fp);                                      /*��������־*/
    free(CTfirst);
	free(CTlast);
	free(CTnext);
	free(CTlink);
  }
}

int loadGIFhead(char *file)                            /*��GIF�ļ�ͷ*/
{
  struct GIF_GLOBAL gifglobal;
  struct GIF_LOCAL  giflocal;
  FILE *fp;
  int  flag=0;

  fp=fopen(file,"rb");                                /*��ͼ���ļ�*/
  if (fp!=NULL) {
    flag=1;
    fseek(fp,0L,SEEK_SET);
    fread(&gifglobal,13,1,fp);                      /*����ȫ����Ϣ��*/
    Width =gifglobal.screen_width;
    Height=gifglobal.screen_depth;
    BitCount =(gifglobal.global_flag & 0x7) + 1;
    ColorNum = 1 << BitCount;                           /*����ɫ����*/
    fread(T_pal,3,ColorNum,fp);                         /*�����ɫ������*/
    fread(&giflocal,10,1,fp);                           /*����ֲ���Ϣ��*/
    fclose(fp);                                       /*�ر�ͼ���ļ�*/
	      /*  ����ȡ��ͼ�����  */
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

  i=loadGIFhead(file);                                 /*��GIF�ļ�ͷ*/
  if (i==0)  return;
  if (T_bitpp<=8) 
     set_allpalette(T_pal);       /*����VGA��ɫ��Ĵ���*/
  else { 
     px *= T_bpp;
     if (BitCount<=8)
        set_Vpalette((BYTE *)T_pal,(union COLOR_V *)bgr);   
  }

  dw=min(Width -px,Dx);        dw=min(dw,swide-sx);   /*ȷ���ӿڳߴ�*/
  dh=min(Height-py,Dy);        dh=min(dh,sdeep-sy);
  buffer=(BYTE*) malloc(4*Width);                                /*����洢��Ԫ*/
  buf1=buffer;
  sc=(BYTE*) malloc(4*Width);                                /*����洢��Ԫ*/

  fp=fopen(file,"rb");                                /*��ͼ���ļ�*/
  fseek(fp,OffBits,SEEK_SET);                         /*����ȡ��ָ��*/
  unpack_LZWline(buffer,Width,fp,1);                /*������Ԫ��ʼ��*/
  for (i=0;i<Height;i++) {
    unpack_LZWline(buffer,Width,fp,0);                    /*����һ��*/
    if (T_bitpp>=24) {  
      memcpy(sc,buffer,dw);
      trans_8toColor(sc,Width);
	  buf1=sc;
	}
    if ((i>py)&&(i<py+dh))                            /*��ʾ���ڿ���*/
      put_image(sx,i+sy-py,dw,1,&buf1[px]);                /*��ʾһ��*/
  }
  unpack_LZWline(buffer,Width,fp,-1);                 /*�ͷŹ�����Ԫ*/
  fclose(fp);                                         /*�ر�ͼ���ļ�*/

  free(sc);
  free(buffer);                                       /*�ͷŴ洢��Ԫ*/
}

int saveGIFhead(char *file)                    	       /*дGIF�ļ�ͷ*/
{
  struct GIF_GLOBAL gifglobal = {
	   'G','I','F','8','7','a',0,0,0,0,0 };
  struct GIF_LOCAL  giflocal;
  FILE *fp;
  int  flag=0;
  BYTE *p;  

  gifglobal.screen_width = Width;                   /*������Ϣͷ����*/
  gifglobal.screen_depth = Height;
  gifglobal.global_flag = (BitCount-1) | 0xf0;

  get_allpalette(T_pal);                            /*ȡ����ɫ������*/

  p=(BYTE*) &giflocal;
  p[0]=',';
  p[1]=p[2]=p[3]=p[4]=p[9]=0;
  p[5]=(BYTE) (Width  & 0xff);		
  p[6]=(BYTE) (Width >> 8);
  p[7]=(BYTE) (Height & 0xff);		
  p[8]=(BYTE) (Height >> 8);

  fp=fopen(file,"w+b");                               /*����ͼ���ļ�*/
  if (fp!=NULL) {
    flag=1;
    fseek(fp,0L,SEEK_SET);
    fwrite(&gifglobal,13,1,fp);        /*дȫ����Ϣͷ*/
	ColorNum = 1 << BitCount;                             /*����ɫ����*/
    fwrite(T_pal,3,ColorNum,fp); /*д��ɫ������*/
    fwrite(&giflocal,10,1,fp);          /*д�ֲ���Ϣͷ*/
    fclose(fp);                                       /*�ر�ͼ���ļ�*/
    OffBits=13+3*ColorNum+10;
  }                                                 /*����������λ��*/
  return(flag);
}

void saveGIFimage(char *file,int x, int y,int Dx, int Dy,int bits)    
{                                                       /*дͼ���ļ�*/
  BYTE *buffer,ch=';';
  int  j;
  FILE *fp;

  BitCount = bits;
  Width=min(Dx,swide-x);   Height=min(Dy,sdeep-y);
  j=saveGIFhead(file);                                 /*дGIF�ļ�ͷ*/
  if (j==0)  return;

  buffer=malloc(1024);                                /*����洢��Ԫ*/
  fp=fopen(file,"r+b");                               /*��ͼ���ļ�*/
  fseek(fp,OffBits,SEEK_SET);                         /*���ô�ȡָ��*/
  pack_LZWline(fp,Width,buffer,8);                  /*������Ԫ��ʼ��*/
  for (j=0;j<Dy;j++) {
    get_image(x,y+j,Width,1,buffer);                /*ȡһ��ͼ������*/
    pack_LZWline(fp,Width,buffer,0);                      /*����һ��*/
  }
  pack_LZWline(fp,Width,buffer,-1);                   /*�ͷŹ�����Ԫ*/
  fwrite(&ch,1,1,fp);
  fclose(fp);                                         /*�ر�ͼ���ļ�*/
  free(buffer);                                       /*�ͷŴ洢��Ԫ*/
}

int loadTGAhead(char *file) 			       /*��TGA�ļ�ͷ*/
{
  struct TGA_HEAD tgahead;
  int  i,j,flag=0;
  FILE *fp;
  BYTE *p;
 
  fp=fopen(file,"rb");   		              /*��ͼ���ļ�*/
  if (fp != NULL) {
    flag=1;
    fseek(fp,0L,SEEK_SET);
    fread(&tgahead,18,1,fp);      		        /*����ͼ���ļ�ͷ*/
						                      /*����ȡ��ͼ�����*/
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
    if (ImageType==1) {                    	        /*�ļ����е�ɫ��*/
      fread(p,1,768,fp);                            /*�����ɫ������*/
      for (i=0;i<768;i+=3) {            	          /*������������*/
	    j=p[i];      p[i]=p[i+2];
	    p[i+2]=j;
      }
	}
	else if (ImageType==3) {                    /*�ڰ�ͼ���ϵ�ɫ��*/
      for (i=0;i<256;i++) {
	     for (j=0;j<3;j++)  p[3*i+j]=i;
      }
      ColorNum = 256;
    }
    fclose(fp);                            	      /*�ر�ͼ���ļ�*/
  }
  return(flag);
}

void loadTGAimage(char *file,int px,int py,int Dx,int Dy,int sx,int sy)
{                                               /*���벢��ʾ��ɫͼ��*/
  int  i,dw,dh;
  FILE *fp;
  BYTE *sc;

  i=loadTGAhead(file);                                 /*��TGA�ļ�ͷ*/
  if (i==0)  return;

  dw=min(Width -px,Dx);     dw=min(dw,swide-sx);      /*ȷ���ӿڳߴ�*/
  dh=min(Height-py,Dy);     dh=min(dh,sdeep-sy);

  if (T_bitpp<=8) 
     set_allpalette(T_pal);       /*����VGA��ɫ��Ĵ���*/
  else { 
     px *= T_bpp;
     if (BitCount<=8)
        set_Vpalette((BYTE *)T_pal,(union COLOR_V *)bgr);   
  }
  sc=malloc(4*Width);                                 /*����洢��Ԫ*/

  fp=fopen(file,"rb");                  	      /*��ͼ���ļ�*/
  fseek(fp,OffBits,SEEK_SET);              	      /*����ȡ��ָ��*/
  for (i=Height-1;i>=0;i--) {
    fread(sc,BytesPerLine,1,fp);                	/*��һ��ͼ������*/
    if (T_bitpp==32)  Trans_24to32(sc,3*Width);      /*32Bit��ʾģʽ*/

    if ((BitCount<=8)&&(T_bitpp>=24))
       trans_8toColor(sc,Width);

    if ((i>=py)&&(i<py+dh))
      put_image(sx,i+sy-py,dw,1,&sc[px]);            	  /*��ʾһ��*/
  }
  fclose(fp);                                         /*�ر�ͼ���ļ�*/
  free(sc);                                           /*�ͷŴ洢��Ԫ*/
}

int saveTGAhead(char *file)                            /*дTGA�ļ�ͷ*/
{
  struct TGA_HEAD tgahead;
  int  i,j,flag=0;                           	     /*TGAȱʡ�ļ�ͷ*/
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

  fp=fopen(file,"w+b");                               /*��ͼ���ļ�*/
  if (fp != NULL) {
    flag=1;
    fseek(fp,0L,SEEK_SET);
    fwrite(&tgahead,18,1,fp);      		        /*����ͼ���ļ�ͷ*/
    if (ImageType==1) {
      get_allpalette(T_pal);         		      /*ȡ��ɫ������*/
      p=(BYTE *) T_pal;
      for (i=0;i<768;i+=3) {                 	      /*ɫ������ת��*/
	     j=p[i];      p[i]=p[i+2];
	     p[i+2]=j;
      }
      fwrite(T_pal,1,768,fp);         		      /*д��ɫ������*/
    }
    fclose(fp);                            	      /*�ر�ͼ���ļ�*/
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
  i=saveTGAhead(file);                                 /*дTGA�ļ�ͷ*/
  if (i==0)  return;

  sc=malloc(4*Width);                                 /*����洢��Ԫ*/

  n=1;
  if (T_bitpp>16)  n=3;
  fp=fopen(file,"r+b");                               /*��ͼ���ļ�*/
  fseek(fp,OffBits,SEEK_SET);                         /*���ô���ָ��*/
  for (i=Dy-1;i>-1;i--) {
    get_image(x,y+i,Width,1,sc);                    /*��һ��ͼ������*/
    if (T_bitpp==32)  Trans_32to24(sc,Width);
					  /*32Bit����ת��Ϊ24Bit����*/
    fwrite(sc,n,Width,fp);                    	  /*ͼ�����ݴ����ļ�*/
  }
  fclose(fp);                                 	      /*�ر�ͼ���ļ�*/
  free(sc);                                   	      /*�ͷŴ洢��Ԫ*/
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

