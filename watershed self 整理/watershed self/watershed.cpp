#include"stdafx.h"

/////////////////////////////////////////
//IplImage* image-- 输入图片
//INT* deltar ----  输出的梯度数组
/////////////////////////////////////////
void GetGra(IplImage* image, INT* deltar)
//通过sobel获取灰度图像image的梯度值存数组deltar
{
    IplImage* pSrc = NULL;
    pSrc = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
    cvCopyImage(image,pSrc);
    int width = pSrc->width;
    int height = pSrc->height;
 
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
    FLOAT dGray;
    FLOAT dGradOne;
    FLOAT dGradTwo;
    int x,y;
    int* gra = new int[height*width];
    int gr;
    for (y = 0;y < height;y++)
    {
        for (x = 0;x < width;x++)
        {
            int image_pos=width*y+x;
            gr = ((uchar *)(pSrc->imageData))[y*pSrc->widthStep+x];
            gra[image_pos]=gr;
        }
    }
    //暂不计算边缘点
    for (y = 1;y < height-1;y++)
    {
        for (x = 1;x < width-1;x++)
        {
            dGray = 0;
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
            for (int yy = 0;yy < 3;yy++)
            {
                for (int xx = 0;xx < 3;xx++)
                {
                    dGradOne += nTmp[yy][xx]*nWeight[0][yy][xx];//X轴方向
                    dGradTwo += nTmp[yy][xx]*nWeight[1][yy][xx];//Y轴方向
                }
            }
            dGray = dGradOne*dGradOne+dGradTwo*dGradTwo;
            dGray = sqrtf(dGray);
            deltar[y*width+x] = (int)dGray;
        }
    }    
    //边缘赋为其内侧点的值
    for (y=0; y<height; y++)
    {
        INT x1 = 0;
        INT pos1 = y*width + x1;
        deltar[pos1] = deltar[pos1+1]; 
        INT x2 = width-1;
        INT pos2 = y*width + x2;
        deltar[pos2] = deltar[pos2-1];
    }
    for ( x = 0; x < width; x++)
    {
        INT y1 = 0;
        INT pos1 = x;
        INT inner = x + width;//下一行；
        deltar[pos1] = deltar[inner];
        INT y2 = height-1;
        INT pos2 = y2*width + x;
        inner = pos2 - width;//上一行；
        deltar[pos2] = deltar[inner];
    }     
    delete [] gra;
    gra=NULL;
}

////////////////////////////////////////////////////////
//INT* deltar----------------------------梯度数组
//INT width,INT height:
//MyImageGraPtWatershed*  graposarr------各梯度频率
//INT* gradientadd-----------------------各梯度起终位置；
////////////////////////////////////////////////////////
void GradSort(INT* deltar,INT width,INT height,MyImageGraPtWatershed*  graposarr,INT* gradientadd)
{
	int imageLen = width*height;
    INT*  gradientfre = new INT[256];         //图像中各点梯度值频率；
	memset( gradientfre, 0, 256*sizeof(INT));
	LONG xstart, imagepos, deltapos;
    xstart = imagepos = deltapos = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            xstart = y*width;
            deltapos = xstart + x;
            if (deltar[deltapos] > 255)
            {
                deltar[deltapos] = 255;
            }
            INT tempi = (INT)(deltar[deltapos]);
            gradientfre[tempi] ++;            //灰度值频率；
        }
    }
     
    //统计各梯度的累加概率；
    INT added = 0;
    gradientadd[0] = 0;                        //第一个起始位置为0；
    for (INT ii = 1; ii < 256; ii++)
    {
        added += gradientfre[ii-1];
        gradientadd[ii] = added;
    }
    gradientadd[256] = imageLen;               //最后位置；
     
    memset( gradientfre, 0, 256*sizeof(INT));  //清零，下面用作某梯度内的指针；
     
    //自左上至右下sorting....
    for (int y = 0; y < height; y++)
    {
        xstart = y*width;
        for (int x = 0; x < width; x++)
        {
            deltapos = xstart + x;
            INT tempi = (INT)(deltar[deltapos]);//当前点的梯度值，由于前面的步骤，最大只能为255；
            //根据梯度值决定在排序数组中的位置；
            INT tempos = gradientadd[tempi] + gradientfre[tempi];
            gradientfre[tempi] ++;              //梯度内指针后移；
            graposarr[tempos].gradient = tempi; //根据当前点的梯度将该点信息放后排序后数组中的合适位置中去；    
            graposarr[tempos].x = x;
            graposarr[tempos].y = y;
        }
    }

}
 
//////////////////////////////////////////////////////////////////////////
// Luc Vincent and Pierre Soille的分水岭分割flood步骤的实现代码，
// MyImageGraPt* imiarr - 输入的排序后数组
// INT* graddarr -------- 输入的各梯度数组，由此直接存取各H像素点
// INT minh，INT maxh --- 最小最大梯度
// INT* flagarr --------- 输出标记数组
// 注意：目前不设分水岭标记，只设每个像素所属区域；
//////////////////////////////////////////////////////////////////////////
void Flood(MyImageGraPtWatershed* imiarr, INT* graddarr,
                         INT minh, INT maxh,
                         INT* flagarr, INT& outrgnumber,
                         INT width,INT height)
{
    int imageWidth = width;
    int imageHeight = height;
 
    const INT INIT = -2;                       //initial value of lable image
    const INT MASK = -1;                       //initial value at each level
    const INT WATERSHED = 0;                   //label of the watershed pixels
    INT h = 0;
    INT imagelen = imageWidth * imageHeight;   //图像中像素的个数
    for (INT i = 0; i < imagelen; i++)
    {
        flagarr[i] = INIT;
    }
    INT* imd = new INT[imagelen];              //距离数组，直接存取；
    for (int i = 0; i < imagelen; i++)
    {
        imd[i] = 0;
    }
    std::queue <int> myqueue;
    INT curlabel = 0;                          //各盆地标记；
     
    for (h = minh; h <= maxh; h++)
    {
        INT stpos = graddarr[h];
        INT edpos = graddarr[h+1];
        for (INT ini=stpos; ini<edpos; ini++)
        {
            INT x = imiarr[ini].x;
            INT y = imiarr[ini].y;
            INT ipos = y*imageWidth + x;
            flagarr[ipos] = MASK;
            //以下检查该点邻域是否已标记属于某区或分水岭，若是，则将该点加入fifo;
            INT left = ipos - 1;
            if (x-1 >= 0)
            {
                if (flagarr[left] >= 0)
                {
                    imd[ipos] = 1;
                    myqueue.push(ipos);          //点位置压入fifo;
                    continue;
                }              
            }
            INT right = ipos + 1;
            if (x+1 < imageWidth)
            {
                if (flagarr[right] >= 0)
                {
                    imd[ipos] = 1;
                    myqueue.push(ipos);           //点位置压入fifo;
                    continue;
                }
            }
            INT up = ipos - imageWidth;
            if (y-1 >= 0)
            {
                if (flagarr[up] >= 0)
                {
                    imd[ipos] = 1;
                    myqueue.push(ipos);            //点位置压入fifo;
                    continue;
                }              
            }
            INT down = ipos + imageWidth;
            if (y+1 < imageHeight)
            {
                if (flagarr[down] >= 0)
                {
                    imd[ipos] = 1;
                    myqueue.push(ipos);             //点位置压入fifo;
                    continue;
                }          
            }
        }       
        //以下根据先进先出队列扩展现有盆地；
        INT curdist = 1; myqueue.push(-99);         //特殊标记；
        while (TRUE)
        {
            INT p = myqueue.front();
            myqueue.pop();
            if (p == -99)
            {
                if ( myqueue.empty() )
                {
                    break;
                }else
                {
                    myqueue.push(-99);
                    curdist = curdist + 1;
                    p = myqueue.front();
                    myqueue.pop();
                }
            }          
            //以下找p的邻域；
            INT y = (INT) (p/imageWidth);
            INT x = p - y*imageWidth;
            INT left = p - 1;
            if  (x-1 >= 0)
            {
                if ( ( (imd[left] < curdist) && flagarr[left] > 0)
                    || (flagarr[left] == 0) )
                {
                    if ( flagarr[left] > 0 )
                    {
                        //ppei属于某区域（不是分水岭）；
                        if ( (flagarr[p] == MASK)
                            || (flagarr[p] == WATERSHED) )
                        {
                            //将其设为邻点所属区域；
                            flagarr[p] = flagarr[left];
                        }else if (flagarr[p] != flagarr[left])
                        {
                            //原来赋的区与现在赋的区不同，设为分水岭；
                            //flagarr[p] = WATERSHED;
                        }
                    }else if (flagarr[p] == MASK)          //ppei为分岭；
                    {
                        flagarr[p] = WATERSHED;
                    }
                }else if ( (flagarr[left] == MASK) && (imd[left] == 0) )
                //ppei中已MASK的点，但尚未标记（即不属某区也不是分水岭）;
                {
                    imd[left] = curdist + 1; myqueue.push(left);
                }
            }
             
            INT right = p + 1;
            if (x+1 < imageWidth)
            {
                if ( ( (imd[right] < curdist) &&  flagarr[right] > 0)
                    || (flagarr[right] == 0) )
                {
                    if ( flagarr[right] > 0 )
                    {
                        //ppei属于某区域（不是分水岭）；
                        if ( (flagarr[p] == MASK)
                            || (flagarr[p] == WATERSHED) )
                        {
                            //将其设为邻点所属区域；
                            flagarr[p] = flagarr[right];
                        }else if (flagarr[p] != flagarr[right])
                        {
                            //原来赋的区与现在赋的区不同，设为分水岭；
                            //flagarr[p] = WATERSHED;
                        }
                    }else if (flagarr[p] == MASK)//ppei为分岭；
                    {
                        flagarr[p] = WATERSHED;
                    }
                }else if ( (flagarr[right] == MASK) && (imd[right] == 0) )
                    //ppei中已MASK的点，但尚未标记（即不属某区也不是分水岭）;
                {
                    imd[right] = curdist + 1; myqueue.push(right);
                }
            }
             
            INT up = p - imageWidth;
            if (y-1>=0)
            {
                if ( ( (imd[up] < curdist) &&  flagarr[up] > 0)
                    || (flagarr[up] == 0) )
                {
                    if ( flagarr[up] > 0 )
                    {
                        //ppei属于某区域（不是分水岭）；
                        if ( (flagarr[p] == MASK)
                            || (flagarr[p] == WATERSHED) )
                        {
                            //将其设为邻点所属区域；
                            flagarr[p] = flagarr[up];
                        }else if (flagarr[p] != flagarr[up])
                        {
                            //原来赋的区与现在赋的区不同，设为分水岭；
                            //flagarr[p] = WATERSHED;
                        }
                    }else if (flagarr[p] == MASK)//ppei为分岭；
                    {
                        flagarr[p] = WATERSHED;
                    }
                }else if ( (flagarr[up] == MASK) && (imd[up] == 0) )
                    //ppei中已MASK的点，但尚未标记（即不属某区也不是分水岭）;
                {
                    imd[up] = curdist + 1; myqueue.push(up);
                }
            }
             
            INT down = p + imageWidth;
            if (y+1<imageHeight)
            {
                if ( ( (imd[down] < curdist) &&  flagarr[down] > 0)
                    || (flagarr[down] == 0) )
                {
                    if ( flagarr[down] > 0 )
                    {
                        //ppei属于某区域（不是分水岭）；
                        if ( (flagarr[p] == MASK)
                            || (flagarr[p] == WATERSHED) )
                        {
                            //将其设为邻点所属区域；
                            flagarr[p] = flagarr[down];
                        }else if (flagarr[p] != flagarr[down])
                        {
                            //原来赋的区与现在赋的区不同，设为分水岭；
                            //flagarr[p] = WATERSHED;
                        }
                    }else if (flagarr[p] == MASK)//ppei为分岭；
                    {
                        flagarr[p] = WATERSHED;
                    }
                }else if ( (flagarr[down] == MASK) && (imd[down] == 0) )
                    //ppei中已MASK的点，但尚未标记（既不属某区也不是分水岭）;
                {
                    imd[down] = curdist + 1; myqueue.push(down);
                }  
            }
             
        }//以上现有盆地的扩展；
         
        //以下处理新发现的盆地；
        for (int ini = stpos; ini < edpos; ini++)
        {
            INT x = imiarr[ini].x;
            INT y = imiarr[ini].y;
            INT ipos = y*imageWidth + x;
            imd[ipos] = 0;//重置所有距离
            if (flagarr[ipos] == MASK)
            {
                //经过前述扩展后该点仍为MASK，则该点必为新盆地的一个起始点;
                curlabel = curlabel + 1;
                myqueue.push(ipos);
                flagarr[ipos] = curlabel;
                 
                while ( myqueue.empty() == FALSE )
                {
                    INT ppei = myqueue.front();
                    myqueue.pop();
                    INT ppeiy = (INT) (ppei/imageWidth);
                    INT ppeix = ppei - ppeiy*imageWidth;
                     
                    INT ppeileft = ppei - 1;
                    if ( (ppeix-1>=0) && (flagarr[ppeileft] == MASK) )
                    {
                        myqueue.push(ppeileft);               //点位置压入fifo;
                        flagarr[ppeileft] = curlabel;
                    }
                    INT ppeiright = ppei + 1;
                    if ( (ppeix+1 < imageWidth) && (flagarr[ppeiright] == MASK) )
                    {
                        myqueue.push(ppeiright);               //点位置压入fifo;
                        flagarr[ppeiright] = curlabel;
                    }
                    INT ppeiup = ppei - imageWidth;
                    if ( (ppeiy-1 >= 0) && (flagarr[ppeiup] == MASK) )
                    {
                        myqueue.push(ppeiup);                   //点位置压入fifo;
                        flagarr[ppeiup] = curlabel;
                    }
                    INT ppeidown = ppei + imageWidth;
                    if ( (ppeiy+1 < imageHeight) && (flagarr[ppeidown] == MASK) )
                    {
                        myqueue.push(ppeidown);                 //点位置压入fifo;
                        flagarr[ppeidown] = curlabel;
                    }                  
                }              
            }
        }
		//以上处理新发现的盆地； 
	}    
    outrgnumber = curlabel;
    delete [] imd;
    imd = NULL;
}
 
INT FindMergedRgn(INT idint, INT* mergearr)
//找到idint最终所合并到的区号；
{
    INT outid = idint;
    while ( mergearr[outid] > 0 )
    {
        outid = mergearr[outid];
    }
    return outid;
}
