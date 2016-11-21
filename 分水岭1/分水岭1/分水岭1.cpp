#include <iostream>  
#include "cv.h"  
#include "cxcore.h"  
#include "highgui.h"   
#include<vector>   
#include<iostream>   
#include<queue>   
#include<fstream> 

using namespace std; 
#define NearMeasureBias 200->0;//判定区域颜色相似的阈值；

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
int x,y,i; //循环
 
    IplImage* pSrc = NULL;
	IplImage* img;
	img=cvLoadImage("2->bmp");  
    pSrc = cvCreateImage(cvGetSize(img),img->depth,img->nChannels);
    cvCopyImage(img,pSrc);
    int width = pSrc->width;         //图像宽度
    int height = pSrc->height;           //图像高度
    int depth = pSrc->depth;         //图像位深(IPL_DEPTH_8U->->->)
    int channels = pSrc->nChannels;      //图像通道数(1、2、3、4)
    int imgSize = pSrc->imageSize;       //图像大小 imageSize = height*widthStep
    int step = pSrc->widthStep/sizeof(uchar);   
    uchar* data    = (uchar *)pSrc->imageData;
    int imageLen = width*height;   
     
    //===================① 获取原彩色图像的梯度值(先转灰度)===================//
    int* deltar = new int[imageLen]; //梯度模数组
    //float* deltasita = new float[imgSize];//梯度角度数组；
    //  彩色转灰度
    IplImage* gray  = cvCreateImage(cvGetSize(pSrc),pSrc->depth,1);
    cvCvtColor(pSrc,gray,CV_BGR2GRAY); 
    //  求梯度
    GetGra(gray,deltar);
    //GetGra2(sobel8u,deltar);
     
 
    //===================② 梯度值排序===================//
    //以下统计各梯度频率
    int*  graposarr = new int[imageLen];
    int*  gradientfre = new int[256];//图像中各点梯度值频率；
    int*  gradientadd = new int[257];//各梯度起终位置；
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
            gradientfre[tempi] ++;//灰度值频率；
        }
    }
     
    //统计各梯度的累加概率；
    int added = 0;
    gradientadd[0] = 0;//第一个起始位置为0；
    for (int ii=1; ii<256; ii++)
    {
        added += gradientfre[ii-1];
        gradientadd[ii] = added;
    }
    gradientadd[256] = imageLen;//最后位置；
     
    memset( gradientfre, 0, 256*sizeof(int)); //清零，下面用作某梯度内的指针；
     
    //自左上至右下sorting->->->->
    for (y=0; y<height; y++)
    {
        xstart = y*width;
        for ( x=0; x<width; x++)
        {
            deltapos = xstart + x;
            int tempi = (int)(deltar[deltapos]);//当前点的梯度值，由于前面的步骤，最大只能为255；
            //根据梯度值决定在排序数组中的位置；
            int tempos = gradientadd[tempi] + gradientfre[tempi];
            gradientfre[tempi] ++;//梯度内指针后移；
            graposarr[tempos]->gradient = tempi; //根据当前点的梯度将该点信息放后排序后数组中的合适位置中去；    
            graposarr[tempos]->x = x;
            graposarr[tempos]->y = y;
        }
    }
 
 
    //===================③ 泛洪得每个像素所属区域标记flag=================//
    int rgnumber = 0;                   //分割后的区域数
    int*   flag = new int[imageLen];        //各点标识数组
    Flood(graposarr,gradientadd,0,255,flag,rgnumber,width,height);
     
     
    //===================④ 由flag标记求各个区域的LUV均值=================//
    //以下准备计算各个区域的LUV均值；
    //分割后各个区的一些统计信息,第一个元素不用，图像中各点所属区域的信息存放在flag数组中
    int*  rginfoarr = new int[rgnumber+1];   
    //清空该数组
    for (i=0; i<rgnumber+1; i++) //LUV
    {
        rginfoarr[i]->isflag = FALSE;
        rginfoarr[i]->ptcount = 0;
        rginfoarr[i]->l = 0;
        rginfoarr[i]->u = 0;
        rginfoarr[i]->v = 0;
    }
     
    //以下保存LUV数据；
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
            int rgid = flag[pos];//当前位置点所属区域在区统计信息数组中的位置
            //以下将该点的信息加到其所属区信息中去
            rginfoarr[rgid]->ptcount ++;
            rginfoarr[rgid]->l += luvData[pos]->l;
            rginfoarr[rgid]->u += luvData[pos]->u;
            rginfoarr[rgid]->v += luvData[pos]->v;
        }
    }
 
    //求出各个区的LUV均值；
    for (i=0; i<=rgnumber; i++)	
    {
		int i;
		int rginfoarr[i];
        rginfoarr[i]->l = (float) ( rginfoarr[i]->l / rginfoarr[i]->ptcount );
        rginfoarr[i]->u = (float) ( rginfoarr[i]->u / rginfoarr[i]->ptcount );
        rginfoarr[i]->v = (float) ( rginfoarr[i]->v / rginfoarr[i]->ptcount );
    }
 
    //===================⑤ 区域融合 ===================//
    //根据各区luv均值（rginfoarr）和各区之间邻接关系（用flag计算）进行区域融合
    int* mergearr = new int[rgnumber+1];
    memset( mergearr, -1, sizeof(int)*(rgnumber+1) );
    int mergednum = 0;
    MergeRgs(rginfoarr, rgnumber, flag, width, height, mergearr, mergednum);
    //MergeRgs(rginfoarr, rgnumber, flag, width, height);
     
 
    //===================⑥ 确定融合后各个像素所属区域===================//
    //确定合并后各像素点所属区域；
    for (y=0; y<(height); y++)
    {
        xstart = y*width;
        for ( x=0; x<(width); x++)
        {
            int pos = xstart + x;
            int rgid = flag[pos];//该点所属区域；
            flag[pos] = FindMergedRgn(rgid, mergearr);
        }
    }
    delete [] mergearr;
    mergearr = NULL;
 
    //===================⑧ 绘制区域轮廓，保存结果===================//
    //=================== ===================//
    //以下根据标识数组查找边界点（不管四边点以减小复杂度）
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
    //  在源图像上绘制轮廓
    CvMemStorage* storage= cvCreateMemStorage(0);
    CvSeq* contour= 0;//可动态增长元素序列
    IplImage* dstContourGray= cvCreateImage(cvGetSize(dstContour),8,1);
    cvCvtColor(dstContour,dstContourGray,CV_BGR2GRAY);
    cvNamedWindow("dstContourGray",1);
    cvShowImage("dstContourGray",dstContourGray);
 
    cvFindContours( //函数cvFindContours从二值图像中检索轮廓，并返回检测到的轮廓的个数
        dstContourGray,//二值化图像
        storage,    //轮廓存储容器
        &contour,   //指向第一个轮廓输出的指针
        sizeof(CvContour),  //序列头大小
        CV_RETR_CCOMP,      //内外轮廓都检测; CV_RETR_EXTERNAL：只检索最外面的轮廓
        CV_CHAIN_APPROX_SIMPLE//压缩水平垂直和对角分割，即函数只保留末端的象素点 CV_CHAIN_CODE//CV_CHAIN_APPROX_NONE
        );
     
    IplImage* dst = cvCreateImage(cvGetSize(pSrc),IPL_DEPTH_8U,3);
    cvCopyImage(pSrc,dst);
    for (;contour!= 0;contour= contour->h_next)
    {
        CvScalar color= CV_RGB(rand()&255,rand()&255,rand()&255);
        cvDrawContours( //在图像中绘制轮廓
            dst,
            contour,//指向初始轮廓的指针
            color,  //外层轮廓的颜色
            color,  //内层轮廓的颜色
            -1,     //绘制轮廓的最大等级（
                        //0：当前；
                        //1：相同级别下的轮廓；
                        //2：绘制同级与下一级轮廓；
                        //负数：不会绘制当前轮廓之后的轮廓，但会绘制子轮廓，到（abs(该参数)-1）为止）
            1,      //轮廓线条粗细
            8       //线条的类型
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
 
    //  释放资源
    delete [] gradientadd; gradientadd = NULL;//大小256
    delete [] gradientfre; gradientfre = NULL;//大小256
    delete [] deltar;    deltar    = NULL;//大小imageLen
    delete [] graposarr; graposarr = NULL;//大小imageLen
    cvReleaseImage(&pSrc);
    cvReleaseImage(&gray); 
     
 
}

void GetGra(IplImage* image, int* deltar)
//通过sobel获取灰度图像image的梯度值存数组deltar
{
    //   定义
    IplImage* pSrc = NULL;
	IplImage* image = NULL;
    pSrc = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
    cvCopyImage(image,pSrc);
    int width = pSrc->width;
    int height = pSrc->height;
 
/*  测试参数
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
 
    //暂不计算边缘点
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
     
    //边缘赋为其内侧点的值
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
        int inner = x + width;//下一行；
        deltar[pos1] = deltar[inner];
        int y2 = height-1;
        int pos2 = y2*width + x;
        inner = pos2 - width;//上一行；
        deltar[pos2] = deltar[inner];
    }
     
    delete [] gra;
    gra=NULL;
 
}
 
 
//////////////////////////////////////////////////////////////////////////
// Luc Vincent and Pierre Soille的分水岭分割flood步骤的实现代码，
// 修改自相应伪代码, 伪代码来自作者论文《Watersheds in Digital Spaces:
// An Efficient Algorithm Based on Immersion Simulations》
// IEEE TRANSACTIONS ON PATTERN ANALYSIS AND MACHINE intELLIGENCE->
// VOL->13, NO->6, JUNE 1991;
// by dzj, 2004->06->28
// MyImageGraPt* imiarr - 输入的排序后数组
// int* graddarr -------- 输入的各梯度数组，由此直接存取各H像素点
// int minh，int maxh --- 最小最大梯度
// int* flagarr --------- 输出标记数组
// 注意：目前不设分水岭标记，只设每个像素所属区域；
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
    int imagelen = imageWidth * imageHeight;//图像中像素的个数
    for (int i=0; i<imagelen; i++)
    {
        flagarr[i] = INIT;
    }
    //memset(flagarr, INIT, sizeof(int)*imagelen);
    int* imd = new int[imagelen]; //距离数组，直接存取；
    for (int i=0; i<imagelen; i++)
    {
        imd[i] = 0;
    }
    //memset(imd, 0, sizeof(int)*imagelen);
    std::queue <int> myqueue;
    int curlabel = 0;//各盆地标记；
     
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
            //以下检查该点邻域是否已标记属于某区或分水岭，若是，则将该点加入fifo;
            int left = ipos - 1;
            if (x-1>=0)
            {
                if (flagarr[left]>=0)
                {
                    imd[ipos] = 1;
                    myqueue->push(ipos);//点位置压入fifo;
                    continue;
                }              
            }
            int right = ipos + 1;
            if (x+1<imageWidth)
            {
                if (flagarr[right]>=0)
                {
                    imd[ipos] = 1;
                    myqueue->push(ipos);//点位置压入fifo;
                    continue;
                }
            }
            int up = ipos - imageWidth;
            if (y-1>=0)
            {
                if (flagarr[up]>=0)
                {
                    imd[ipos] = 1;
                    myqueue->push(ipos);//点位置压入fifo;
                    continue;
                }              
            }
            int down = ipos + imageWidth;
            if (y+1<imageHeight)
            {
                if (flagarr[down]>=0)
                {
                    imd[ipos] = 1;
                    myqueue->push(ipos);//点位置压入fifo;
                    continue;
                }          
            }
        }
         
        //以下根据先进先出队列扩展现有盆地；
        int curdist = 1; myqueue->push(-99);//特殊标记；
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
             
            //以下找p的邻域；
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
                        //ppei属于某区域（不是分水岭）；
                        if ( (flagarr[p]==MASK)
                            || (flagarr[p]==WATERSHED) )
                        {
                            //将其设为邻点所属区域；
                            flagarr[p] = flagarr[left];
                        }else if (flagarr[p]!=flagarr[left])
                        {
                            //原来赋的区与现在赋的区不同，设为分水岭；
                            //flagarr[p] = WATERSHED;
                        }
                    }else if (flagarr[p]==MASK)//ppei为分岭；
                    {
                        flagarr[p] = WATERSHED;
                    }
                }else if ( (flagarr[left]==MASK) && (imd[left]==0) )
                    //ppei中已MASK的点，但尚未标记（即不属某区也不是分水岭）;
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
                        //ppei属于某区域（不是分水岭）；
                        if ( (flagarr[p]==MASK)
                            || (flagarr[p]==WATERSHED) )
                        {
                            //将其设为邻点所属区域；
                            flagarr[p] = flagarr[right];
                        }else if (flagarr[p]!=flagarr[right])
                        {
                            //原来赋的区与现在赋的区不同，设为分水岭；
                            //flagarr[p] = WATERSHED;
                        }
                    }else if (flagarr[p]==MASK)//ppei为分岭；
                    {
                        flagarr[p] = WATERSHED;
                    }
                }else if ( (flagarr[right]==MASK) && (imd[right]==0) )
                    //ppei中已MASK的点，但尚未标记（即不属某区也不是分水岭）;
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
                        //ppei属于某区域（不是分水岭）；
                        if ( (flagarr[p]==MASK)
                            || (flagarr[p]==WATERSHED) )
                        {
                            //将其设为邻点所属区域；
                            flagarr[p] = flagarr[up];
                        }else if (flagarr[p]!=flagarr[up])
                        {
                            //原来赋的区与现在赋的区不同，设为分水岭；
                            //flagarr[p] = WATERSHED;
                        }
                    }else if (flagarr[p]==MASK)//ppei为分岭；
                    {
                        flagarr[p] = WATERSHED;
                    }
                }else if ( (flagarr[up]==MASK) && (imd[up]==0) )
                    //ppei中已MASK的点，但尚未标记（即不属某区也不是分水岭）;
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
                        //ppei属于某区域（不是分水岭）；
                        if ( (flagarr[p]==MASK)
                            || (flagarr[p]==WATERSHED) )
                        {
                            //将其设为邻点所属区域；
                            flagarr[p] = flagarr[down];
                        }else if (flagarr[p]!=flagarr[down])
                        {
                            //原来赋的区与现在赋的区不同，设为分水岭；
                            //flagarr[p] = WATERSHED;
                        }
                    }else if (flagarr[p]==MASK)//ppei为分岭；
                    {
                        flagarr[p] = WATERSHED;
                    }
                }else if ( (flagarr[down]==MASK) && (imd[down]==0) )
                    //ppei中已MASK的点，但尚未标记（既不属某区也不是分水岭）;
                {
                    imd[down] = curdist + 1; myqueue->push(down);
                }  
            }
             
        }//以上现有盆地的扩展；
         
        //以下处理新发现的盆地；
        for (ini=stpos; ini<edpos; ini++)
        {
            int x = imiarr[ini]->x;
            int y = imiarr[ini]->y;
            int ipos = y*imageWidth + x;
            imd[ipos] = 0;//重置所有距离
            if (flagarr[ipos]==MASK)
            {
                //经过前述扩展后该点仍为MASK，则该点必为新盆地的一个起始点;
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
                        myqueue->push(ppeileft);//点位置压入fifo;
                        flagarr[ppeileft] = curlabel;
                    }
                    int ppeiright = ppei + 1;
                    if ( (ppeix+1<imageWidth) && (flagarr[ppeiright]==MASK) )
                    {
                        myqueue->push(ppeiright);//点位置压入fifo;
                        flagarr[ppeiright] = curlabel;
                    }
                    int ppeiup = ppei - imageWidth;
                    if ( (ppeiy-1>=0) && (flagarr[ppeiup]==MASK) )
                    {
                        myqueue->push(ppeiup);//点位置压入fifo;
                        flagarr[ppeiup] = curlabel;
                    }
                    int ppeidown = ppei + imageWidth;
                    if ( (ppeiy+1<imageHeight) && (flagarr[ppeidown]==MASK) )
                    {
                        myqueue->push(ppeidown);//点位置压入fifo;
                        flagarr[ppeidown] = curlabel;
                    }                  
                }              
            }
        }//以上处理新发现的盆地；
         
    }
     
    outrgnumber = curlabel;
    delete [] imd;
    imd = NULL;
  
}
 
#define NearMeasureBias 200->0//判定区域颜色相似的阈值；
int MergeRgs(int* rginfoarr, int rgnumber, int* flag, int width, int height, int* outmerge, int& rgnum)
//合并相似区域；
{
    //////////////////////////////////////////////////////////////////////////
    //1、建立各区的邻域数组；
    //2、依次扫描各区域，寻找极小区域；
    //3、对每个极小区（A），在相邻区中找到最相似者；
    //4、与相似区（B）合并（各种信息刷新），在极小区（A）的邻域中
    //   删除相似区（B），在邻域数组中删除相似区（B）对应的项，将
    //   相似区（B）的相邻区s加到极小区（A）的邻域中去；
    //5、记录合并信息，设一数组专门存放该信息，该数组的第A个元素值设为B；
    //6、判断是否仍为极小区，若是则返回3；
    //7、是否所有区域都已处理完毕，若非则返回2；
    //
    //   由于各区的相邻区不会太多，因此采用邻接数组作为存储结构；
    //////////////////////////////////////////////////////////////////////////
    int* neiarr = new int[rgnumber+1];//第一个不用；
    int* mergearr = outmerge;//记录合并情况数组；
 
    //建立邻域数组；
    for (int y=0; y<height; y++)
    {
        int lstart = y * width;
        for (int x=0; x<width; x++)
        {
            int pos = lstart + x;
            int left=-1, right=-1, up=-1, down=-1;
            pMyMath->GetNeiint(x, y, pos, width, height, left, right, up, down);//找pos的四个邻域；
            //确定并刷新邻域区信息；
            int curid = flag[pos];
            AddNeiOfCur(curid, left, right, up, down, flag, neiarr);
        }
    }//建立邻域数组；
     
    //区域信息数组中的有效信息从1开始，第i个位置存放第i个区域的信息；
    for (int rgi=1; rgi<=rgnumber; rgi++)
    {
        //扫描所有区域，找极小区；
        long allpoints = width * height;
        long nmin = (long) (allpoints / 400);
        int curid = rgi;
 
        //rginfoarr[rgi]->isflag初始为FALSE，在被合并到其它区后改为TRUE；
        while ( ( (rginfoarr[rgi]->ptcount)<nmin )
            && !rginfoarr[rgi]->isflag )
        {
            //该区为极小区，遍历所有相邻区，找最接近者；
            int neistr = neiarr[curid];
            int nearid = FindNearestNei(curid, neistr, rginfoarr, mergearr);
            //合并curid与nearid；
            MergeTwoRgn(curid, nearid, neiarr, rginfoarr, mergearr);           
        }
    }
 
    //以下再合并相似区域，（无论大小）,如果不需要，直接将整个循环注释掉就行了；
    int countjjj = 0;
    //区域信息数组中的有效信息从1开始，第i个位置存放第i个区域的信息；
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
//合并相似区域；
{
    //依次处理各个邻域，若相似，则合并；
    //int neistr = neiarr[curid];
    float cl, cu, cv;
    cl = rginfoarr[curid]->l;//当前区的LUV值；
    cu = rginfoarr[curid]->u;
    cv = rginfoarr[curid]->v;
    bool loopmerged = TRUE;//一次循环中是否有合并操作发生，若无，则退出循环；
 
    while (loopmerged)
    {
        loopmerged = FALSE;
        int tempstr = neiarr[curid];//用于本函数内部处理；
        while (tempstr->GetLength()>0)
        {
            int pos = tempstr->Find(" ");
            ASSERT(pos>=0);
            int idstr = tempstr->Left(pos);
            tempstr->Delete(0, pos+1);
             
            int idint = (int) strtol(idstr, NULL, 10);
            //判断该区是否已被合并，若是，则一直找到该区当前的区号；
            idint = FindMergedRgn(idint, mergearr);
            if (idint==curid)
            {
                continue;//这个邻区已被合并到当前区，跳过；
            }
            float tl, tu, tv;
            tl = rginfoarr[idint]->l;//当前处理的邻区的LUV值;
            tu = rginfoarr[idint]->u;
            tv = rginfoarr[idint]->v;
            double tempdis = pow(tl-cl, 2)
                + pow(tu-cu, 2) + pow(tv-cv, 2);
            if (tempdis<NearMeasureBias)
            {
                MergeTwoRgn(curid, idint, neiarr, rginfoarr, mergearr);
                cl = rginfoarr[curid]->l;//当前区的LUV值刷新；
                cu = rginfoarr[curid]->u;
                cv = rginfoarr[curid]->v;
                loopmerged = TRUE;
            }      
        }
    }
}
 
 
void MergeTwoRgn(int curid, int nearid
    , int* neiarr, int* rginfoarr, int* mergearr)
//将nearid合并到curid中去，更新合并后的区信息，并记录该合并；
{
    //将区信息中nearid对应项的标记设为已被合并；
    rginfoarr[nearid]->isflag = TRUE;
    //更新合并后的LUV信息；
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
    //将nearid的邻域加到curid的邻域中去；
    AddBNeiToANei(curid, nearid, neiarr, mergearr);
    //记录该合并；
    mergearr[nearid] = curid;
}
 
void AddBNeiToANei(int curid, int nearid, int* neiarr, int* mergearr)
//将nearid的邻域加到curid的邻域中去；
{
    //先从curid的邻区中把nearid删去；
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
        //否则邻近区为合并过来的区，忽略；
        neiarr[curid]->Delete(temppos, tempstr->GetLength());
    }
*/
    //将nearid的邻区依次加到curid的邻区中去；
    int neistr = neiarr[nearid];
    int curstr = neiarr[curid];
    //一般说来，极小区的邻域应该较少，因此，为着提高合并速度，将
    //curstr加到neistr中去，然后将结果赋给neiarr[curid];
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
            continue;//本区不与本区相邻；
        }else
        {
            if ( neistr->Find(idstr, 0) >= 0 )
            {
                continue;
            }else
            {
                neistr += idstr;//加到邻区中去;
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
            continue;//本区不与本区相邻；
        }else
        {
            if ( neiarr[curid]->Find(idstr, 0) >= 0 )
            {
                continue;
            }else
            {
                neiarr[curid] += idstr;//加到邻区中去;
            }
        }      
    }
*/
}
int FindNearestNei(int curid, int neistr, int* rginfoarr, int* mergearr)
//寻找neistr中与curid最接近的区，返回该区id号；
{
    int outid = -1;
    double mindis = 999999;
    float cl, cu, cv;
    cl = rginfoarr[curid]->l;//当前区的LUV值；
    cu = rginfoarr[curid]->u;
    cv = rginfoarr[curid]->v;
 
    int tempstr = neistr;//用于本函数内部处理；
    while (tempstr->GetLength()>0)
    {
        int pos = tempstr->Find(" ");
        ASSERT(pos>=0);
        int idstr = tempstr->Left(pos);
        tempstr->Delete(0, pos+1);
 
        int idint = (int) strtol(idstr, NULL, 10);
        //判断该区是否已被合并，若是，则一直找到该区当前的区号；
        idint = FindMergedRgn(idint, mergearr);
        if (idint==curid)
        {
            continue;//这个邻区已被合并到当前区，跳过；
        }
        float tl, tu, tv;
        tl = rginfoarr[idint]->l;//当前处理的邻区的LUV值;
        tu = rginfoarr[idint]->u;
        tv = rginfoarr[idint]->v;
        double tempdis = pow(tl-cl, 2)
            + pow(tu-cu, 2) + pow(tv-cv, 2);
        if (tempdis<mindis)
        {
            mindis = tempdis;//最大距离和对应的相邻区ID；
            outid = idint;
        }      
    }
 
    return outid;
}
 
int FindMergedRgnMaxbias(int idint, int* mergearr, int bias)
//大阈值终止查找合并区，用于coarse watershed,
//调用者必须保证idint有效，即：mergearr[idint]>0；
//以及mergearr有效，即：mergearr[idint]<idint;
{
    int outid = idint;
    while ( mergearr[outid]<bias )
    {
        outid = mergearr[outid];
    }
    return mergearr[outid];
}
 
int FindMergedRgn(int idint, int* mergearr)
//找到idint最终所合并到的区号；
{
    int outid = idint;
    while ( mergearr[outid] > 0 )
    {
        outid = mergearr[outid];
    }
    return outid;
}

int AddNeiRgn(int curid, int neiid, int* neiarr)
//增加neiid为curid的相邻区
{
    int tempneis = neiarr[curid];//当前的相邻区；
    int toaddstr;
    toaddstr->Format("%d ", neiid);
 
    int temppos = tempneis->Find(toaddstr, 0);
    while (temppos>0 && neiarr[curid]->GetAt(temppos-1)!=' ')
    {
        temppos = neiarr[curid]->Find(toaddstr, temppos+1);
    }
     
    if ( temppos<0 )
    {
        //当前相邻区中没有tempneis,则加入
        neiarr[curid] += toaddstr;
    }
}

void AddNeiOfCur(int curid, int left, int right,int up, int down, int* flag, int* neiarr)
//刷新当前点的所有相邻区；
{
    int leftid, rightid, upid, downid;
    leftid = rightid = upid = downid = curid;
    if (left>=0)
    {
        leftid = flag[left];
        if (leftid!=curid)
        {
            //邻点属于另一区, 加邻域点信息；
            AddNeiRgn(curid, leftid, neiarr);
        }
    }
    if (right>0)
    {
        rightid = flag[right];
        if (rightid!=curid)
        {
            //邻点属于另一区, 加邻域点信息；
            AddNeiRgn(curid, rightid, neiarr);
        }
    }
    if (up>=0)
    {
        upid = flag[up];
        if (upid!=curid)
        {
            //邻点属于另一区, 加邻域点信息；
            AddNeiRgn(curid, upid, neiarr);
        }
    }
    if (down>0)
    {
        downid = flag[down];
        if (downid!=curid)
        {
            //邻点属于另一区, 加邻域点信息；
            AddNeiRgn(curid, downid, neiarr);
        }
    }
}