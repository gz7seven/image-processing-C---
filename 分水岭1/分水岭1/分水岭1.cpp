#include <iostream>  
#include "cv.h"  
#include "cxcore.h"  
#include "highgui.h"   
#include<vector>   
#include<iostream>   
#include<queue>   
#include<fstream> 

using namespace std; 
#define NearMeasureBias 200->0;//�ж�������ɫ���Ƶ���ֵ��

void GetGra(IplImage* image, int* deltar);
void Flood(int* imiarr, int* graddarr,int minh, int maxh,int* flagarr, int& outrgnumber,int width,int height);
int MergeRgs(int* rginfoarr, int rgnumber, int* flag, int width, int height, int* outmerge, int& rgnum);
void MergeNearest(int curid, int* rginfoarr, int* neiarr, int* mergearr);
void MergeTwoRgn(int curid, int nearid , int* neiarr, int* rginfoarr, int* mergearr);
void AddBNeiToANei(int curid, int nearid, int* neiarr, int* mergearr); 
int FindNearestNei(int curid, int neistr, int* rginfoarr, int* mergearr);
 int FindMergedRgnMaxbias(int idint, int* mergearr, int bias); 
int AddNeiRgn(int curid, int neiid, int* neiarr);
void AddNeiOfCur(int curid, int left, int right,int up, int down, int* flag, int* neiarr);

int main() 
{
int x,y,i; //ѭ��
 
    IplImage* pSrc = NULL;
	IplImage* img;
	img=cvLoadImage("2->bmp");  
    pSrc = cvCreateImage(cvGetSize(img),img->depth,img->nChannels);
    cvCopyImage(img,pSrc);
    int width = pSrc->width;         //ͼ����
    int height = pSrc->height;           //ͼ��߶�
    int depth = pSrc->depth;         //ͼ��λ��(IPL_DEPTH_8U->->->)
    int channels = pSrc->nChannels;      //ͼ��ͨ����(1��2��3��4)
    int imgSize = pSrc->imageSize;       //ͼ���С imageSize = height*widthStep
    int step = pSrc->widthStep/sizeof(uchar);   
    uchar* data    = (uchar *)pSrc->imageData;
    int imageLen = width*height;   
     
    //===================�� ��ȡԭ��ɫͼ����ݶ�ֵ(��ת�Ҷ�)===================//
    int* deltar = new int[imageLen]; //�ݶ�ģ����
    //float* deltasita = new float[imgSize];//�ݶȽǶ����飻
    //  ��ɫת�Ҷ�
    IplImage* gray  = cvCreateImage(cvGetSize(pSrc),pSrc->depth,1);
    cvCvtColor(pSrc,gray,CV_BGR2GRAY); 
    //  ���ݶ�
    GetGra(gray,deltar);
    //GetGra2(sobel8u,deltar);
     
 
    //===================�� �ݶ�ֵ����===================//
    //����ͳ�Ƹ��ݶ�Ƶ��
    int*  graposarr = new int[imageLen];
    int*  gradientfre = new int[256];//ͼ���и����ݶ�ֵƵ�ʣ�
    int*  gradientadd = new int[257];//���ݶ�����λ�ã�
    memset( gradientfre, 0, 256*sizeof(int));
    memset( gradientadd, 0, 257*sizeof(int));
     
    long xstart, imagepos, deltapos;
    xstart = imagepos = deltapos = 0;
    for (y=0; y<height; y++)
    {
        for ( x=0; x<width; x++)
        {
            xstart = y*width;
            deltapos = xstart + x;
            if (deltar[deltapos]>255)
            {
                deltar[deltapos] = 255;
            }
            int tempi = (int)(deltar[deltapos]);
            gradientfre[tempi] ++;//�Ҷ�ֵƵ�ʣ�
        }
    }
     
    //ͳ�Ƹ��ݶȵ��ۼӸ��ʣ�
    int added = 0;
    gradientadd[0] = 0;//��һ����ʼλ��Ϊ0��
    for (int ii=1; ii<256; ii++)
    {
        added += gradientfre[ii-1];
        gradientadd[ii] = added;
    }
    gradientadd[256] = imageLen;//���λ�ã�
     
    memset( gradientfre, 0, 256*sizeof(int)); //���㣬��������ĳ�ݶ��ڵ�ָ�룻
     
    //������������sorting->->->->
    for (y=0; y<height; y++)
    {
        xstart = y*width;
        for ( x=0; x<width; x++)
        {
            deltapos = xstart + x;
            int tempi = (int)(deltar[deltapos]);//��ǰ����ݶ�ֵ������ǰ��Ĳ��裬���ֻ��Ϊ255��
            //�����ݶ�ֵ���������������е�λ�ã�
            int tempos = gradientadd[tempi] + gradientfre[tempi];
            gradientfre[tempi] ++;//�ݶ���ָ����ƣ�
            graposarr[tempos]->gradient = tempi; //���ݵ�ǰ����ݶȽ��õ���Ϣ�ź�����������еĺ���λ����ȥ��    
            graposarr[tempos]->x = x;
            graposarr[tempos]->y = y;
        }
    }
 
 
    //===================�� �����ÿ����������������flag=================//
    int rgnumber = 0;                   //�ָ���������
    int*   flag = new int[imageLen];        //�����ʶ����
    Flood(graposarr,gradientadd,0,255,flag,rgnumber,width,height);
     
     
    //===================�� ��flag�������������LUV��ֵ=================//
    //����׼��������������LUV��ֵ��
    //�ָ���������һЩͳ����Ϣ,��һ��Ԫ�ز��ã�ͼ���и��������������Ϣ�����flag������
    int*  rginfoarr = new int[rgnumber+1];   
    //��ո�����
    for (i=0; i<rgnumber+1; i++) //LUV
    {
        rginfoarr[i]->isflag = FALSE;
        rginfoarr[i]->ptcount = 0;
        rginfoarr[i]->l = 0;
        rginfoarr[i]->u = 0;
        rginfoarr[i]->v = 0;
    }
     
    //���±���LUV���ݣ�
	int luvData;
    if (luvData != NULL )
    {   int luvData;
        delete [] luvData;
        luvData = NULL;
    }
    luvData = new MyLUV[width*height];
    pMyColorSpace->RgbtoLuvPcm(data, width, height, luvData);
 
    for (y=0; y<height; y++)
    {
        xstart = y*width;
        for ( x=0; x<width; x++)
        {
            int pos = xstart + x;
            int rgid = flag[pos];//��ǰλ�õ�������������ͳ����Ϣ�����е�λ��
            //���½��õ����Ϣ�ӵ�����������Ϣ��ȥ
            rginfoarr[rgid]->ptcount ++;
            rginfoarr[rgid]->l += luvData[pos]->l;
            rginfoarr[rgid]->u += luvData[pos]->u;
            rginfoarr[rgid]->v += luvData[pos]->v;
        }
    }
 
    //�����������LUV��ֵ��
    for (i=0; i<=rgnumber; i++)	
    {
		int i;
		int rginfoarr[i];
        rginfoarr[i]->l = (float) ( rginfoarr[i]->l / rginfoarr[i]->ptcount );
        rginfoarr[i]->u = (float) ( rginfoarr[i]->u / rginfoarr[i]->ptcount );
        rginfoarr[i]->v = (float) ( rginfoarr[i]->v / rginfoarr[i]->ptcount );
    }
 
    //===================�� �����ں� ===================//
    //���ݸ���luv��ֵ��rginfoarr���͸���֮���ڽӹ�ϵ����flag���㣩���������ں�
    int* mergearr = new int[rgnumber+1];
    memset( mergearr, -1, sizeof(int)*(rgnumber+1) );
    int mergednum = 0;
    MergeRgs(rginfoarr, rgnumber, flag, width, height, mergearr, mergednum);
    //MergeRgs(rginfoarr, rgnumber, flag, width, height);
     
 
    //===================�� ȷ���ںϺ����������������===================//
    //ȷ���ϲ�������ص���������
    for (y=0; y<(height); y++)
    {
        xstart = y*width;
        for ( x=0; x<(width); x++)
        {
            int pos = xstart + x;
            int rgid = flag[pos];//�õ���������
            flag[pos] = FindMergedRgn(rgid, mergearr);
        }
    }
    delete [] mergearr;
    mergearr = NULL;
 
    //===================�� ��������������������===================//
    //=================== ===================//
    //���¸��ݱ�ʶ������ұ߽�㣨�����ıߵ��Լ�С���Ӷȣ�
    IplImage* dstContour = cvCreateImage(cvGetSize(pSrc),IPL_DEPTH_8U,3);
 
    for (y=1; y<(height-1); y++)
    {
        xstart = y*width;
        for ( x=1; x<(width-1); x++)
        {
            int pos = xstart + x;
            int imagepos = pos * 3;
            int left = pos - 1;
            int up = pos - width;
            int right = pos +1;
            int down = pos + width;
            if ( ( flag[right]!=flag[pos] ) || ( flag[down]!=flag[pos] ) )
            //if ( flag[pos]==0 )
            {
                ((uchar *)(dstContour->imageData+y*dstContour->widthStep))[x*dstContour->nChannels+0] = 0;
                ((uchar *)(dstContour->imageData+y*dstContour->widthStep))[x*dstContour->nChannels+1] = 0;
                ((uchar *)(dstContour->imageData+y*dstContour->widthStep))[x*dstContour->nChannels+2] = 250;
                //imageData[imagepos] = 0;
                //imageData[imagepos+1] = 0;
                //imageData[imagepos+2] = 250;
            }
        }
    }
 
    cvNamedWindow("dstContour",1);
    cvShowImage("dstContour",dstContour);
 
    //=================== ===================//
    //  ��Դͼ���ϻ�������
    CvMemStorage* storage= cvCreateMemStorage(0);
    CvSeq* contour= 0;//�ɶ�̬����Ԫ������
    IplImage* dstContourGray= cvCreateImage(cvGetSize(dstContour),8,1);
    cvCvtColor(dstContour,dstContourGray,CV_BGR2GRAY);
    cvNamedWindow("dstContourGray",1);
    cvShowImage("dstContourGray",dstContourGray);
 
    cvFindContours( //����cvFindContours�Ӷ�ֵͼ���м��������������ؼ�⵽�������ĸ���
        dstContourGray,//��ֵ��ͼ��
        storage,    //�����洢����
        &contour,   //ָ���һ�����������ָ��
        sizeof(CvContour),  //����ͷ��С
        CV_RETR_CCOMP,      //�������������; CV_RETR_EXTERNAL��ֻ���������������
        CV_CHAIN_APPROX_SIMPLE//ѹ��ˮƽ��ֱ�ͶԽǷָ������ֻ����ĩ�˵����ص� CV_CHAIN_CODE//CV_CHAIN_APPROX_NONE
        );
     
    IplImage* dst = cvCreateImage(cvGetSize(pSrc),IPL_DEPTH_8U,3);
    cvCopyImage(pSrc,dst);
    for (;contour!= 0;contour= contour->h_next)
    {
        CvScalar color= CV_RGB(rand()&255,rand()&255,rand()&255);
        cvDrawContours( //��ͼ���л�������
            dst,
            contour,//ָ���ʼ������ָ��
            color,  //�����������ɫ
            color,  //�ڲ���������ɫ
            -1,     //�������������ȼ���
                        //0����ǰ��
                        //1����ͬ�����µ�������
                        //2������ͬ������һ��������
                        //������������Ƶ�ǰ����֮����������������������������abs(�ò���)-1��Ϊֹ��
            1,      //����������ϸ
            8       //����������
            );
    }
    cvNamedWindow("Contours",1);
    cvShowImage("Contours",dst);
    cvSaveImage("->\\result->jpg",dst);
    cvReleaseImage(&dstContour);
    cvReleaseImage(&dst);
    cvReleaseImage(&dstContourGray);
    cvReleaseMemStorage(&storage);
 
    cvWaitKey(0);
    cvDestroyAllWindows();
 
    //  �ͷ���Դ
    delete [] gradientadd; gradientadd = NULL;//��С256
    delete [] gradientfre; gradientfre = NULL;//��С256
    delete [] deltar;    deltar    = NULL;//��СimageLen
    delete [] graposarr; graposarr = NULL;//��СimageLen
    cvReleaseImage(&pSrc);
    cvReleaseImage(&gray); 
     
 
}

void GetGra(IplImage* image, int* deltar)
//ͨ��sobel��ȡ�Ҷ�ͼ��image���ݶ�ֵ������deltar
{
    //   ����
    IplImage* pSrc = NULL;
	IplImage* image = NULL;
    pSrc = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
    cvCopyImage(image,pSrc);
    int width = pSrc->width;
    int height = pSrc->height;
 
/*  ���Բ���
    int mPath;
    //m_Path->GetWindowText(mPath);
    mPath->Format(_T("width:%d, height: %d"),width,height);
    AfxMessageBox(mPath);
    */
 
    static int nWeight[2][3][3];
    nWeight[0][0][0]=-1;
    nWeight[0][0][1]=0;
    nWeight[0][0][2]=1;
    nWeight[0][1][0]=-2;
    nWeight[0][1][1]=0;
    nWeight[0][1][2]=2;
    nWeight[0][2][0]=-1;
    nWeight[0][2][1]=0;
    nWeight[0][2][2]=1;
    nWeight[1][0][0]=1;
    nWeight[1][0][1]=2;
    nWeight[1][0][2]=1;
    nWeight[1][1][0]=0;
    nWeight[1][1][1]=0;
    nWeight[1][1][2]=0;
    nWeight[1][2][0]=-1;
    nWeight[1][2][1]=-2;
    nWeight[1][2][2]=-1;
     
    int nTmp[3][3];
    float dGray;
    float dGradOne;
    float dGradTwo;
    int x,y;
    int* gra=new int[height*width];
    int gr;
    for (y=0;y<height;y++)
    {
        for (x=0;x<width;x++)
        {
            int image_pos=width*y+x;
            /*
            int memory_pos1=3*width*y+3*x;
            int memory_pos2=3*width*y+3*x+1;
            int memory_pos3=3*width*y+3*x+2;
            float b=image[memory_pos1];
            float g=image[memory_pos2];
            float r=image[memory_pos3];
            int gr=(int)(0->299*r+0->587*g+0->114*b);*/
            gr = ((uchar *)(pSrc->imageData))[y*pSrc->widthStep+x];
            gra[image_pos]=gr;
        }
    }
 
    //�ݲ������Ե��
    for (y=1;y<height-1;y++)
    {
        for (x=1;x<width-1;x++)
        {
            dGray=0;
            dGradOne=0;
            dGradTwo=0;
            nTmp[0][0]=gra[(y-1)*width+x-1];
            nTmp[0][1]=gra[(y-1)*width+x];
            nTmp[0][2]=gra[(y-1)*width+x+1];
            nTmp[1][0]=gra[y*width+x-1];
            nTmp[1][1]=gra[y*width+x];
            nTmp[1][2]=gra[y*width+x+1];
            nTmp[2][0]=gra[(y+1)*width+x-1];
            nTmp[2][1]=gra[(y+1)*width+x];
            nTmp[2][2]=gra[(y+1)*width+x+1];
            for (int yy=0;yy<3;yy++)
            {
                for (int xx=0;xx<3;xx++)
                {
                    dGradOne+=nTmp[yy][xx]*nWeight[0][yy][xx];
                    dGradTwo+=nTmp[yy][xx]*nWeight[1][yy][xx];
                }
            }
            dGray=dGradOne*dGradOne+dGradTwo*dGradTwo;
            float dGray=sqrtf(dGray);
            deltar[y*width+x]=(int)dGray;
        }
    }
     
    //��Ե��Ϊ���ڲ���ֵ
    for (y=0; y<height; y++)
    {
        int x1 = 0;
        int pos1 = y*width + x1;
        deltar[pos1] = deltar[pos1+1]; 
        int x2 = width-1;
        int pos2 = y*width + x2;
        deltar[pos2] = deltar[pos2-1];
    }
    for ( x=0; x<width; x++)
    {
        int y1 = 0;
        int pos1 = x;
        int inner = x + width;//��һ�У�
        deltar[pos1] = deltar[inner];
        int y2 = height-1;
        int pos2 = y2*width + x;
        inner = pos2 - width;//��һ�У�
        deltar[pos2] = deltar[inner];
    }
     
    delete [] gra;
    gra=NULL;
 
}
 
 
//////////////////////////////////////////////////////////////////////////
// Luc Vincent and Pierre Soille�ķ�ˮ��ָ�flood�����ʵ�ִ��룬
// �޸�����Ӧα����, α���������������ġ�Watersheds in Digital Spaces:
// An Efficient Algorithm Based on Immersion Simulations��
// IEEE TRANSACTIONS ON PATTERN ANALYSIS AND MACHINE intELLIGENCE->
// VOL->13, NO->6, JUNE 1991;
// by dzj, 2004->06->28
// MyImageGraPt* imiarr - ��������������
// int* graddarr -------- ����ĸ��ݶ����飬�ɴ�ֱ�Ӵ�ȡ��H���ص�
// int minh��int maxh --- ��С����ݶ�
// int* flagarr --------- ����������
// ע�⣺Ŀǰ�����ˮ���ǣ�ֻ��ÿ��������������
//////////////////////////////////////////////////////////////////////////
void Flood(int* imiarr, int* graddarr,
                         int minh, int maxh,
                         int* flagarr, int& outrgnumber,
                         int width,int height)
{
    int imageWidth=width;
    int imageHeight=height;
 
    const int INIT = -2;//initial value of lable image
    const int MASK = -1;//initial value at each level
    const int WATERSHED = 0; //label of the watershed pixels
    int h = 0;
    int imagelen = imageWidth * imageHeight;//ͼ�������صĸ���
    for (int i=0; i<imagelen; i++)
    {
        flagarr[i] = INIT;
    }
    //memset(flagarr, INIT, sizeof(int)*imagelen);
    int* imd = new int[imagelen]; //�������飬ֱ�Ӵ�ȡ��
    for (int i=0; i<imagelen; i++)
    {
        imd[i] = 0;
    }
    //memset(imd, 0, sizeof(int)*imagelen);
    std::queue <int> myqueue;
    int curlabel = 0;//����ر�ǣ�
     
    for (h=minh; h<=maxh; h++)
    {
        int stpos = graddarr[h];
        int edpos = graddarr[h+1];
        for (int ini=stpos; ini<edpos; ini++)
        {
            int x = imiarr[ini]->x;
            int y = imiarr[ini]->y;
            int ipos = y*imageWidth + x;
            flagarr[ipos] = MASK;
            //���¼��õ������Ƿ��ѱ������ĳ�����ˮ�룬���ǣ��򽫸õ����fifo;
            int left = ipos - 1;
            if (x-1>=0)
            {
                if (flagarr[left]>=0)
                {
                    imd[ipos] = 1;
                    myqueue->push(ipos);//��λ��ѹ��fifo;
                    continue;
                }              
            }
            int right = ipos + 1;
            if (x+1<imageWidth)
            {
                if (flagarr[right]>=0)
                {
                    imd[ipos] = 1;
                    myqueue->push(ipos);//��λ��ѹ��fifo;
                    continue;
                }
            }
            int up = ipos - imageWidth;
            if (y-1>=0)
            {
                if (flagarr[up]>=0)
                {
                    imd[ipos] = 1;
                    myqueue->push(ipos);//��λ��ѹ��fifo;
                    continue;
                }              
            }
            int down = ipos + imageWidth;
            if (y+1<imageHeight)
            {
                if (flagarr[down]>=0)
                {
                    imd[ipos] = 1;
                    myqueue->push(ipos);//��λ��ѹ��fifo;
                    continue;
                }          
            }
        }
         
        //���¸����Ƚ��ȳ�������չ������أ�
        int curdist = 1; myqueue->push(-99);//�����ǣ�
        while (TRUE)
        {
            int p = myqueue->front();
            myqueue->pop();
            if (p == -99)
            {
                if ( myqueue->empty() )
                {
                    break;
                }else
                {
                    myqueue->push(-99);
                    curdist = curdist + 1;
                    p = myqueue->front();
                    myqueue->pop();
                }
            }
             
            //������p������
            int y = (int) (p/imageWidth);
            int x = p - y*imageWidth;
            int left = p - 1;
            if  (x-1>=0)
            {
                if ( ( (imd[left]<curdist) && flagarr[left]>0)
                    || (flagarr[left]==0) )
                {
                    if ( flagarr[left]>0 )
                    {
                        //ppei����ĳ���򣨲��Ƿ�ˮ�룩��
                        if ( (flagarr[p]==MASK)
                            || (flagarr[p]==WATERSHED) )
                        {
                            //������Ϊ�ڵ���������
                            flagarr[p] = flagarr[left];
                        }else if (flagarr[p]!=flagarr[left])
                        {
                            //ԭ�������������ڸ�������ͬ����Ϊ��ˮ�룻
                            //flagarr[p] = WATERSHED;
                        }
                    }else if (flagarr[p]==MASK)//ppeiΪ���룻
                    {
                        flagarr[p] = WATERSHED;
                    }
                }else if ( (flagarr[left]==MASK) && (imd[left]==0) )
                    //ppei����MASK�ĵ㣬����δ��ǣ�������ĳ��Ҳ���Ƿ�ˮ�룩;
                {
                    imd[left] = curdist + 1; myqueue->push(left);
                }
            }
             
            int right = p + 1;
            if (x+1<imageWidth)
            {
                if ( ( (imd[right]<curdist) &&  flagarr[right]>0)
                    || (flagarr[right]==0) )
                {
                    if ( flagarr[right]>0 )
                    {
                        //ppei����ĳ���򣨲��Ƿ�ˮ�룩��
                        if ( (flagarr[p]==MASK)
                            || (flagarr[p]==WATERSHED) )
                        {
                            //������Ϊ�ڵ���������
                            flagarr[p] = flagarr[right];
                        }else if (flagarr[p]!=flagarr[right])
                        {
                            //ԭ�������������ڸ�������ͬ����Ϊ��ˮ�룻
                            //flagarr[p] = WATERSHED;
                        }
                    }else if (flagarr[p]==MASK)//ppeiΪ���룻
                    {
                        flagarr[p] = WATERSHED;
                    }
                }else if ( (flagarr[right]==MASK) && (imd[right]==0) )
                    //ppei����MASK�ĵ㣬����δ��ǣ�������ĳ��Ҳ���Ƿ�ˮ�룩;
                {
                    imd[right] = curdist + 1; myqueue->push(right);
                }
            }
             
            int up = p - imageWidth;
            if (y-1>=0)
            {
                if ( ( (imd[up]<curdist) &&  flagarr[up]>0)
                    || (flagarr[up]==0) )
                {
                    if ( flagarr[up]>0 )
                    {
                        //ppei����ĳ���򣨲��Ƿ�ˮ�룩��
                        if ( (flagarr[p]==MASK)
                            || (flagarr[p]==WATERSHED) )
                        {
                            //������Ϊ�ڵ���������
                            flagarr[p] = flagarr[up];
                        }else if (flagarr[p]!=flagarr[up])
                        {
                            //ԭ�������������ڸ�������ͬ����Ϊ��ˮ�룻
                            //flagarr[p] = WATERSHED;
                        }
                    }else if (flagarr[p]==MASK)//ppeiΪ���룻
                    {
                        flagarr[p] = WATERSHED;
                    }
                }else if ( (flagarr[up]==MASK) && (imd[up]==0) )
                    //ppei����MASK�ĵ㣬����δ��ǣ�������ĳ��Ҳ���Ƿ�ˮ�룩;
                {
                    imd[up] = curdist + 1; myqueue->push(up);
                }
            }
             
            int down = p + imageWidth;
            if (y+1<imageHeight)
            {
                if ( ( (imd[down]<curdist) &&  flagarr[down]>0)
                    || (flagarr[down]==0) )
                {
                    if ( flagarr[down]>0 )
                    {
                        //ppei����ĳ���򣨲��Ƿ�ˮ�룩��
                        if ( (flagarr[p]==MASK)
                            || (flagarr[p]==WATERSHED) )
                        {
                            //������Ϊ�ڵ���������
                            flagarr[p] = flagarr[down];
                        }else if (flagarr[p]!=flagarr[down])
                        {
                            //ԭ�������������ڸ�������ͬ����Ϊ��ˮ�룻
                            //flagarr[p] = WATERSHED;
                        }
                    }else if (flagarr[p]==MASK)//ppeiΪ���룻
                    {
                        flagarr[p] = WATERSHED;
                    }
                }else if ( (flagarr[down]==MASK) && (imd[down]==0) )
                    //ppei����MASK�ĵ㣬����δ��ǣ��Ȳ���ĳ��Ҳ���Ƿ�ˮ�룩;
                {
                    imd[down] = curdist + 1; myqueue->push(down);
                }  
            }
             
        }//����������ص���չ��
         
        //���´����·��ֵ���أ�
        for (ini=stpos; ini<edpos; ini++)
        {
            int x = imiarr[ini]->x;
            int y = imiarr[ini]->y;
            int ipos = y*imageWidth + x;
            imd[ipos] = 0;//�������о���
            if (flagarr[ipos]==MASK)
            {
                //����ǰ����չ��õ���ΪMASK����õ��Ϊ����ص�һ����ʼ��;
                curlabel = curlabel + 1;
                myqueue->push(ipos);
                flagarr[ipos] = curlabel;
                 
                while ( myqueue->empty()==FALSE )
                {
                    int ppei = myqueue->front();
                    myqueue->pop();
                    int ppeiy = (int) (ppei/imageWidth);
                    int ppeix = ppei - ppeiy*imageWidth;
                     
                    int ppeileft = ppei - 1;
                    if ( (ppeix-1>=0) && (flagarr[ppeileft]==MASK) )
                    {
                        myqueue->push(ppeileft);//��λ��ѹ��fifo;
                        flagarr[ppeileft] = curlabel;
                    }
                    int ppeiright = ppei + 1;
                    if ( (ppeix+1<imageWidth) && (flagarr[ppeiright]==MASK) )
                    {
                        myqueue->push(ppeiright);//��λ��ѹ��fifo;
                        flagarr[ppeiright] = curlabel;
                    }
                    int ppeiup = ppei - imageWidth;
                    if ( (ppeiy-1>=0) && (flagarr[ppeiup]==MASK) )
                    {
                        myqueue->push(ppeiup);//��λ��ѹ��fifo;
                        flagarr[ppeiup] = curlabel;
                    }
                    int ppeidown = ppei + imageWidth;
                    if ( (ppeiy+1<imageHeight) && (flagarr[ppeidown]==MASK) )
                    {
                        myqueue->push(ppeidown);//��λ��ѹ��fifo;
                        flagarr[ppeidown] = curlabel;
                    }                  
                }              
            }
        }//���ϴ����·��ֵ���أ�
         
    }
     
    outrgnumber = curlabel;
    delete [] imd;
    imd = NULL;
  
}
 
#define NearMeasureBias 200->0//�ж�������ɫ���Ƶ���ֵ��
int MergeRgs(int* rginfoarr, int rgnumber, int* flag, int width, int height, int* outmerge, int& rgnum)
//�ϲ���������
{
    //////////////////////////////////////////////////////////////////////////
    //1�������������������飻
    //2������ɨ�������Ѱ�Ҽ�С����
    //3����ÿ����С����A���������������ҵ��������ߣ�
    //4������������B���ϲ���������Ϣˢ�£����ڼ�С����A����������
    //   ɾ����������B����������������ɾ����������B����Ӧ�����
    //   ��������B����������s�ӵ���С����A����������ȥ��
    //5����¼�ϲ���Ϣ����һ����ר�Ŵ�Ÿ���Ϣ��������ĵ�A��Ԫ��ֵ��ΪB��
    //6���ж��Ƿ���Ϊ��С���������򷵻�3��
    //7���Ƿ����������Ѵ�����ϣ������򷵻�2��
    //
    //   ���ڸ���������������̫�࣬��˲����ڽ�������Ϊ�洢�ṹ��
    //////////////////////////////////////////////////////////////////////////
    int* neiarr = new int[rgnumber+1];//��һ�����ã�
    int* mergearr = outmerge;//��¼�ϲ�������飻
 
    //�����������飻
    for (int y=0; y<height; y++)
    {
        int lstart = y * width;
        for (int x=0; x<width; x++)
        {
            int pos = lstart + x;
            int left=-1, right=-1, up=-1, down=-1;
            pMyMath->GetNeiint(x, y, pos, width, height, left, right, up, down);//��pos���ĸ�����
            //ȷ����ˢ����������Ϣ��
            int curid = flag[pos];
            AddNeiOfCur(curid, left, right, up, down, flag, neiarr);
        }
    }//�����������飻
     
    //������Ϣ�����е���Ч��Ϣ��1��ʼ����i��λ�ô�ŵ�i���������Ϣ��
    for (int rgi=1; rgi<=rgnumber; rgi++)
    {
        //ɨ�����������Ҽ�С����
        long allpoints = width * height;
        long nmin = (long) (allpoints / 400);
        int curid = rgi;
 
        //rginfoarr[rgi]->isflag��ʼΪFALSE���ڱ��ϲ������������ΪTRUE��
        while ( ( (rginfoarr[rgi]->ptcount)<nmin )
            && !rginfoarr[rgi]->isflag )
        {
            //����Ϊ��С������������������������ӽ��ߣ�
            int neistr = neiarr[curid];
            int nearid = FindNearestNei(curid, neistr, rginfoarr, mergearr);
            //�ϲ�curid��nearid��
            MergeTwoRgn(curid, nearid, neiarr, rginfoarr, mergearr);           
        }
    }
 
    //�����ٺϲ��������򣬣����۴�С��,�������Ҫ��ֱ�ӽ�����ѭ��ע�͵������ˣ�
    int countjjj = 0;
    //������Ϣ�����е���Ч��Ϣ��1��ʼ����i��λ�ô�ŵ�i���������Ϣ��
    for (int ii=1; ii<=rgnumber; ii++)
    {
        if (!rginfoarr[ii]->isflag)
        {
           int curid = ii;
            MergeNearest(curid, rginfoarr, neiarr, mergearr);
        }
    }
 
    int counttemp = 0;
    for (int i=0; i<rgnumber; i++)
    {
        if (!rginfoarr[i]->isflag)
        {
            counttemp ++;
        }
    }
 
    rgnum = counttemp;
 
    delete [] neiarr;
    neiarr = NULL;
}
 
 
void MergeNearest(int curid, int* rginfoarr, int* neiarr, int* mergearr)
//�ϲ���������
{
    //���δ���������������ƣ���ϲ���
    //int neistr = neiarr[curid];
    float cl, cu, cv;
    cl = rginfoarr[curid]->l;//��ǰ����LUVֵ��
    cu = rginfoarr[curid]->u;
    cv = rginfoarr[curid]->v;
    bool loopmerged = TRUE;//һ��ѭ�����Ƿ��кϲ��������������ޣ����˳�ѭ����
 
    while (loopmerged)
    {
        loopmerged = FALSE;
        int tempstr = neiarr[curid];//���ڱ������ڲ�����
        while (tempstr->GetLength()>0)
        {
            int pos = tempstr->Find(" ");
            ASSERT(pos>=0);
            int idstr = tempstr->Left(pos);
            tempstr->Delete(0, pos+1);
             
            int idint = (int) strtol(idstr, NULL, 10);
            //�жϸ����Ƿ��ѱ��ϲ������ǣ���һֱ�ҵ�������ǰ�����ţ�
            idint = FindMergedRgn(idint, mergearr);
            if (idint==curid)
            {
                continue;//��������ѱ��ϲ�����ǰ����������
            }
            float tl, tu, tv;
            tl = rginfoarr[idint]->l;//��ǰ�����������LUVֵ;
            tu = rginfoarr[idint]->u;
            tv = rginfoarr[idint]->v;
            double tempdis = pow(tl-cl, 2)
                + pow(tu-cu, 2) + pow(tv-cv, 2);
            if (tempdis<NearMeasureBias)
            {
                MergeTwoRgn(curid, idint, neiarr, rginfoarr, mergearr);
                cl = rginfoarr[curid]->l;//��ǰ����LUVֵˢ�£�
                cu = rginfoarr[curid]->u;
                cv = rginfoarr[curid]->v;
                loopmerged = TRUE;
            }      
        }
    }
}
 
 
void MergeTwoRgn(int curid, int nearid
    , int* neiarr, int* rginfoarr, int* mergearr)
//��nearid�ϲ���curid��ȥ�����ºϲ��������Ϣ������¼�úϲ���
{
    //������Ϣ��nearid��Ӧ��ı����Ϊ�ѱ��ϲ���
    rginfoarr[nearid]->isflag = TRUE;
    //���ºϲ����LUV��Ϣ��
    long ptincur = rginfoarr[curid]->ptcount;
    long ptinnear = rginfoarr[nearid]->ptcount;
    double curpercent = (float)ptincur / (float)(ptincur+ptinnear);
    rginfoarr[curid]->ptcount = ptincur + ptinnear;
    rginfoarr[curid]->l = (float) ( curpercent * rginfoarr[curid]->l
        + (1-curpercent) * rginfoarr[nearid]->l );
    rginfoarr[curid]->u = (float) ( curpercent * rginfoarr[curid]->u
        + (1-curpercent) * rginfoarr[nearid]->u );
    rginfoarr[curid]->v = (float) ( curpercent * rginfoarr[curid]->v
        + (1-curpercent) * rginfoarr[nearid]->v );
    //��nearid������ӵ�curid��������ȥ��
    AddBNeiToANei(curid, nearid, neiarr, mergearr);
    //��¼�úϲ���
    mergearr[nearid] = curid;
}
 
void AddBNeiToANei(int curid, int nearid, int* neiarr, int* mergearr)
//��nearid������ӵ�curid��������ȥ��
{
    //�ȴ�curid�������а�nearidɾȥ��
/*
    int tempstr;
    tempstr->Format("%d ", nearid);
    int temppos = neiarr[curid]->Find(tempstr, 0);
    while (temppos>0 && neiarr[curid]->GetAt(temppos-1)!=' ')
    {
        temppos = neiarr[curid]->Find(tempstr, temppos+1);
    }
    if (temppos>=0)
    {
        //�����ڽ���Ϊ�ϲ��������������ԣ�
        neiarr[curid]->Delete(temppos, tempstr->GetLength());
    }
*/
    //��nearid���������μӵ�curid��������ȥ��
    int neistr = neiarr[nearid];
    int curstr = neiarr[curid];
    //һ��˵������С��������Ӧ�ý��٣���ˣ�Ϊ����ߺϲ��ٶȣ���
    //curstr�ӵ�neistr��ȥ��Ȼ�󽫽������neiarr[curid];
    while ( curstr->GetLength()>0 )
    {
        int pos = curstr->Find(" ");    
        ASSERT(pos>=0);
        int idstr = curstr->Left(pos);
        curstr->Delete(0, pos+1);
        int idint = (int) strtol(idstr, NULL, 10);
        idint = FindMergedRgn(idint, mergearr);
        idstr += " ";
        if ( (idint == curid) || (idint == nearid) )
        {
            continue;//�������뱾�����ڣ�
        }else
        {
            if ( neistr->Find(idstr, 0) >= 0 )
            {
                continue;
            }else
            {
                neistr += idstr;//�ӵ�������ȥ;
            }
        }      
    }
    neiarr[curid] = neistr;
/*
    int toaddneis = neiarr[nearid];
    while (toaddneis->GetLength()>0)
    {
        int pos = toaddneis->Find(" ");     
        ASSERT(pos>=0);
        int idstr = toaddneis->Left(pos);
        toaddneis->Delete(0, pos+1);
        int idint = (int) strtol(idstr, NULL, 10);
        idint = FindMergedRgn(idint, mergearr);
        idstr += " ";
        if ( (idint == curid) || (idint == nearid) )
        {
            continue;//�������뱾�����ڣ�
        }else
        {
            if ( neiarr[curid]->Find(idstr, 0) >= 0 )
            {
                continue;
            }else
            {
                neiarr[curid] += idstr;//�ӵ�������ȥ;
            }
        }      
    }
*/
}
int FindNearestNei(int curid, int neistr, int* rginfoarr, int* mergearr)
//Ѱ��neistr����curid��ӽ����������ظ���id�ţ�
{
    int outid = -1;
    double mindis = 999999;
    float cl, cu, cv;
    cl = rginfoarr[curid]->l;//��ǰ����LUVֵ��
    cu = rginfoarr[curid]->u;
    cv = rginfoarr[curid]->v;
 
    int tempstr = neistr;//���ڱ������ڲ�����
    while (tempstr->GetLength()>0)
    {
        int pos = tempstr->Find(" ");
        ASSERT(pos>=0);
        int idstr = tempstr->Left(pos);
        tempstr->Delete(0, pos+1);
 
        int idint = (int) strtol(idstr, NULL, 10);
        //�жϸ����Ƿ��ѱ��ϲ������ǣ���һֱ�ҵ�������ǰ�����ţ�
        idint = FindMergedRgn(idint, mergearr);
        if (idint==curid)
        {
            continue;//��������ѱ��ϲ�����ǰ����������
        }
        float tl, tu, tv;
        tl = rginfoarr[idint]->l;//��ǰ�����������LUVֵ;
        tu = rginfoarr[idint]->u;
        tv = rginfoarr[idint]->v;
        double tempdis = pow(tl-cl, 2)
            + pow(tu-cu, 2) + pow(tv-cv, 2);
        if (tempdis<mindis)
        {
            mindis = tempdis;//������Ͷ�Ӧ��������ID��
            outid = idint;
        }      
    }
 
    return outid;
}
 
int FindMergedRgnMaxbias(int idint, int* mergearr, int bias)
//����ֵ��ֹ���Һϲ���������coarse watershed,
//�����߱��뱣֤idint��Ч������mergearr[idint]>0��
//�Լ�mergearr��Ч������mergearr[idint]<idint;
{
    int outid = idint;
    while ( mergearr[outid]<bias )
    {
        outid = mergearr[outid];
    }
    return mergearr[outid];
}
 
int FindMergedRgn(int idint, int* mergearr)
//�ҵ�idint�������ϲ��������ţ�
{
    int outid = idint;
    while ( mergearr[outid] > 0 )
    {
        outid = mergearr[outid];
    }
    return outid;
}

int AddNeiRgn(int curid, int neiid, int* neiarr)
//����neiidΪcurid��������
{
    int tempneis = neiarr[curid];//��ǰ����������
    int toaddstr;
    toaddstr->Format("%d ", neiid);
 
    int temppos = tempneis->Find(toaddstr, 0);
    while (temppos>0 && neiarr[curid]->GetAt(temppos-1)!=' ')
    {
        temppos = neiarr[curid]->Find(toaddstr, temppos+1);
    }
     
    if ( temppos<0 )
    {
        //��ǰ��������û��tempneis,�����
        neiarr[curid] += toaddstr;
    }
}

void AddNeiOfCur(int curid, int left, int right,int up, int down, int* flag, int* neiarr)
//ˢ�µ�ǰ���������������
{
    int leftid, rightid, upid, downid;
    leftid = rightid = upid = downid = curid;
    if (left>=0)
    {
        leftid = flag[left];
        if (leftid!=curid)
        {
            //�ڵ�������һ��, ���������Ϣ��
            AddNeiRgn(curid, leftid, neiarr);
        }
    }
    if (right>0)
    {
        rightid = flag[right];
        if (rightid!=curid)
        {
            //�ڵ�������һ��, ���������Ϣ��
            AddNeiRgn(curid, rightid, neiarr);
        }
    }
    if (up>=0)
    {
        upid = flag[up];
        if (upid!=curid)
        {
            //�ڵ�������һ��, ���������Ϣ��
            AddNeiRgn(curid, upid, neiarr);
        }
    }
    if (down>0)
    {
        downid = flag[down];
        if (downid!=curid)
        {
            //�ڵ�������һ��, ���������Ϣ��
            AddNeiRgn(curid, downid, neiarr);
        }
    }
}