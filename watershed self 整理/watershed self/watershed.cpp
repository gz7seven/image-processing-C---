#include"stdafx.h"

/////////////////////////////////////////
//IplImage* image-- ����ͼƬ
//INT* deltar ----  ������ݶ�����
/////////////////////////////////////////
void GetGra(IplImage* image, INT* deltar)
//ͨ��sobel��ȡ�Ҷ�ͼ��image���ݶ�ֵ������deltar
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
    //�ݲ������Ե��
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
                    dGradOne += nTmp[yy][xx]*nWeight[0][yy][xx];//X�᷽��
                    dGradTwo += nTmp[yy][xx]*nWeight[1][yy][xx];//Y�᷽��
                }
            }
            dGray = dGradOne*dGradOne+dGradTwo*dGradTwo;
            dGray = sqrtf(dGray);
            deltar[y*width+x] = (int)dGray;
        }
    }    
    //��Ե��Ϊ���ڲ���ֵ
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
        INT inner = x + width;//��һ�У�
        deltar[pos1] = deltar[inner];
        INT y2 = height-1;
        INT pos2 = y2*width + x;
        inner = pos2 - width;//��һ�У�
        deltar[pos2] = deltar[inner];
    }     
    delete [] gra;
    gra=NULL;
}

////////////////////////////////////////////////////////
//INT* deltar----------------------------�ݶ�����
//INT width,INT height:
//MyImageGraPtWatershed*  graposarr------���ݶ�Ƶ��
//INT* gradientadd-----------------------���ݶ�����λ�ã�
////////////////////////////////////////////////////////
void GradSort(INT* deltar,INT width,INT height,MyImageGraPtWatershed*  graposarr,INT* gradientadd)
{
	int imageLen = width*height;
    INT*  gradientfre = new INT[256];         //ͼ���и����ݶ�ֵƵ�ʣ�
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
            gradientfre[tempi] ++;            //�Ҷ�ֵƵ�ʣ�
        }
    }
     
    //ͳ�Ƹ��ݶȵ��ۼӸ��ʣ�
    INT added = 0;
    gradientadd[0] = 0;                        //��һ����ʼλ��Ϊ0��
    for (INT ii = 1; ii < 256; ii++)
    {
        added += gradientfre[ii-1];
        gradientadd[ii] = added;
    }
    gradientadd[256] = imageLen;               //���λ�ã�
     
    memset( gradientfre, 0, 256*sizeof(INT));  //���㣬��������ĳ�ݶ��ڵ�ָ�룻
     
    //������������sorting....
    for (int y = 0; y < height; y++)
    {
        xstart = y*width;
        for (int x = 0; x < width; x++)
        {
            deltapos = xstart + x;
            INT tempi = (INT)(deltar[deltapos]);//��ǰ����ݶ�ֵ������ǰ��Ĳ��裬���ֻ��Ϊ255��
            //�����ݶ�ֵ���������������е�λ�ã�
            INT tempos = gradientadd[tempi] + gradientfre[tempi];
            gradientfre[tempi] ++;              //�ݶ���ָ����ƣ�
            graposarr[tempos].gradient = tempi; //���ݵ�ǰ����ݶȽ��õ���Ϣ�ź�����������еĺ���λ����ȥ��    
            graposarr[tempos].x = x;
            graposarr[tempos].y = y;
        }
    }

}
 
//////////////////////////////////////////////////////////////////////////
// Luc Vincent and Pierre Soille�ķ�ˮ��ָ�flood�����ʵ�ִ��룬
// MyImageGraPt* imiarr - ��������������
// INT* graddarr -------- ����ĸ��ݶ����飬�ɴ�ֱ�Ӵ�ȡ��H���ص�
// INT minh��INT maxh --- ��С����ݶ�
// INT* flagarr --------- ����������
// ע�⣺Ŀǰ�����ˮ���ǣ�ֻ��ÿ��������������
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
    INT imagelen = imageWidth * imageHeight;   //ͼ�������صĸ���
    for (INT i = 0; i < imagelen; i++)
    {
        flagarr[i] = INIT;
    }
    INT* imd = new INT[imagelen];              //�������飬ֱ�Ӵ�ȡ��
    for (int i = 0; i < imagelen; i++)
    {
        imd[i] = 0;
    }
    std::queue <int> myqueue;
    INT curlabel = 0;                          //����ر�ǣ�
     
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
            //���¼��õ������Ƿ��ѱ������ĳ�����ˮ�룬���ǣ��򽫸õ����fifo;
            INT left = ipos - 1;
            if (x-1 >= 0)
            {
                if (flagarr[left] >= 0)
                {
                    imd[ipos] = 1;
                    myqueue.push(ipos);          //��λ��ѹ��fifo;
                    continue;
                }              
            }
            INT right = ipos + 1;
            if (x+1 < imageWidth)
            {
                if (flagarr[right] >= 0)
                {
                    imd[ipos] = 1;
                    myqueue.push(ipos);           //��λ��ѹ��fifo;
                    continue;
                }
            }
            INT up = ipos - imageWidth;
            if (y-1 >= 0)
            {
                if (flagarr[up] >= 0)
                {
                    imd[ipos] = 1;
                    myqueue.push(ipos);            //��λ��ѹ��fifo;
                    continue;
                }              
            }
            INT down = ipos + imageWidth;
            if (y+1 < imageHeight)
            {
                if (flagarr[down] >= 0)
                {
                    imd[ipos] = 1;
                    myqueue.push(ipos);             //��λ��ѹ��fifo;
                    continue;
                }          
            }
        }       
        //���¸����Ƚ��ȳ�������չ������أ�
        INT curdist = 1; myqueue.push(-99);         //�����ǣ�
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
            //������p������
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
                        //ppei����ĳ���򣨲��Ƿ�ˮ�룩��
                        if ( (flagarr[p] == MASK)
                            || (flagarr[p] == WATERSHED) )
                        {
                            //������Ϊ�ڵ���������
                            flagarr[p] = flagarr[left];
                        }else if (flagarr[p] != flagarr[left])
                        {
                            //ԭ�������������ڸ�������ͬ����Ϊ��ˮ�룻
                            //flagarr[p] = WATERSHED;
                        }
                    }else if (flagarr[p] == MASK)          //ppeiΪ���룻
                    {
                        flagarr[p] = WATERSHED;
                    }
                }else if ( (flagarr[left] == MASK) && (imd[left] == 0) )
                //ppei����MASK�ĵ㣬����δ��ǣ�������ĳ��Ҳ���Ƿ�ˮ�룩;
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
                        //ppei����ĳ���򣨲��Ƿ�ˮ�룩��
                        if ( (flagarr[p] == MASK)
                            || (flagarr[p] == WATERSHED) )
                        {
                            //������Ϊ�ڵ���������
                            flagarr[p] = flagarr[right];
                        }else if (flagarr[p] != flagarr[right])
                        {
                            //ԭ�������������ڸ�������ͬ����Ϊ��ˮ�룻
                            //flagarr[p] = WATERSHED;
                        }
                    }else if (flagarr[p] == MASK)//ppeiΪ���룻
                    {
                        flagarr[p] = WATERSHED;
                    }
                }else if ( (flagarr[right] == MASK) && (imd[right] == 0) )
                    //ppei����MASK�ĵ㣬����δ��ǣ�������ĳ��Ҳ���Ƿ�ˮ�룩;
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
                        //ppei����ĳ���򣨲��Ƿ�ˮ�룩��
                        if ( (flagarr[p] == MASK)
                            || (flagarr[p] == WATERSHED) )
                        {
                            //������Ϊ�ڵ���������
                            flagarr[p] = flagarr[up];
                        }else if (flagarr[p] != flagarr[up])
                        {
                            //ԭ�������������ڸ�������ͬ����Ϊ��ˮ�룻
                            //flagarr[p] = WATERSHED;
                        }
                    }else if (flagarr[p] == MASK)//ppeiΪ���룻
                    {
                        flagarr[p] = WATERSHED;
                    }
                }else if ( (flagarr[up] == MASK) && (imd[up] == 0) )
                    //ppei����MASK�ĵ㣬����δ��ǣ�������ĳ��Ҳ���Ƿ�ˮ�룩;
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
                        //ppei����ĳ���򣨲��Ƿ�ˮ�룩��
                        if ( (flagarr[p] == MASK)
                            || (flagarr[p] == WATERSHED) )
                        {
                            //������Ϊ�ڵ���������
                            flagarr[p] = flagarr[down];
                        }else if (flagarr[p] != flagarr[down])
                        {
                            //ԭ�������������ڸ�������ͬ����Ϊ��ˮ�룻
                            //flagarr[p] = WATERSHED;
                        }
                    }else if (flagarr[p] == MASK)//ppeiΪ���룻
                    {
                        flagarr[p] = WATERSHED;
                    }
                }else if ( (flagarr[down] == MASK) && (imd[down] == 0) )
                    //ppei����MASK�ĵ㣬����δ��ǣ��Ȳ���ĳ��Ҳ���Ƿ�ˮ�룩;
                {
                    imd[down] = curdist + 1; myqueue.push(down);
                }  
            }
             
        }//����������ص���չ��
         
        //���´����·��ֵ���أ�
        for (int ini = stpos; ini < edpos; ini++)
        {
            INT x = imiarr[ini].x;
            INT y = imiarr[ini].y;
            INT ipos = y*imageWidth + x;
            imd[ipos] = 0;//�������о���
            if (flagarr[ipos] == MASK)
            {
                //����ǰ����չ��õ���ΪMASK����õ��Ϊ����ص�һ����ʼ��;
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
                        myqueue.push(ppeileft);               //��λ��ѹ��fifo;
                        flagarr[ppeileft] = curlabel;
                    }
                    INT ppeiright = ppei + 1;
                    if ( (ppeix+1 < imageWidth) && (flagarr[ppeiright] == MASK) )
                    {
                        myqueue.push(ppeiright);               //��λ��ѹ��fifo;
                        flagarr[ppeiright] = curlabel;
                    }
                    INT ppeiup = ppei - imageWidth;
                    if ( (ppeiy-1 >= 0) && (flagarr[ppeiup] == MASK) )
                    {
                        myqueue.push(ppeiup);                   //��λ��ѹ��fifo;
                        flagarr[ppeiup] = curlabel;
                    }
                    INT ppeidown = ppei + imageWidth;
                    if ( (ppeiy+1 < imageHeight) && (flagarr[ppeidown] == MASK) )
                    {
                        myqueue.push(ppeidown);                 //��λ��ѹ��fifo;
                        flagarr[ppeidown] = curlabel;
                    }                  
                }              
            }
        }
		//���ϴ����·��ֵ���أ� 
	}    
    outrgnumber = curlabel;
    delete [] imd;
    imd = NULL;
}
 
INT FindMergedRgn(INT idint, INT* mergearr)
//�ҵ�idint�������ϲ��������ţ�
{
    INT outid = idint;
    while ( mergearr[outid] > 0 )
    {
        outid = mergearr[outid];
    }
    return outid;
}
