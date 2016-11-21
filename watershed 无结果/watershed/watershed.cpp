#include"cv.h"
#include"highgui.h"
#include"iostream"
#include <windows.h>

#define SIZE 10000000
using namespace cv; //���������cv������Ͳ��ü���ǰ׺��
//����һ������
typedef struct
{
        int q[SIZE];
        int front;
        int rear;
}queue;

//image_in:����ͼ������ָ��
//image_out:���ͼ������ָ��
//image_d:����ͼ������ָ��
//image_mask:���ͼ��ָ��

//ͼ��ָ��еķ�ˮ���㷨
void Watershed(uchar *image_in, int xsize, int ysize,uchar* image_d ,uchar*image_mask,uchar* image_out)
{
    const BYTE MASK = -2;
    const BYTE WSHED = 0;
    int Current_Dist = -1;
    int Current_Label = 0;
    queue* QU  = new queue[1] ;
    

    int i,j,k,l,m,n,p1,p2,x,y;
    int temp = 254;

    for(i = 0;i<SIZE;i++)
        QU->q[i] = 0;
    QU->front = 0;
    QU->rear = 0;

    for(i = 0;i<xsize;i++)
        for(j = 0;j<ysize;j++)
        {
            *(image_out+j*xsize+i) = *(image_in+j*xsize+i);
        }
         //�ñ�ǳ�ʼֵΪ0
    for(k = 0;k<xsize;k++)
        for(l = 0;l<ysize;l++)
        {
            *(image_mask+l*xsize+k)=0;
        }


    //�þ���ͼ��ĳ�ʼֵΪ0
    for(i = 0;i<xsize;i++)
        for(j = 0;j<ysize;j++)
        {
            *(image_d+j*xsize+i) = 0;
        }

    //����ǰ����û�취����ֻ�������ֲ���ȫ�����ˣ�
    for(k = 0;k<xsize;k++)
        for(l = 0;l<ysize;l++)
        {
            if(*(image_out+j*xsize+i) == 255)
                *(image_out+j*xsize+i) = 254;
        }

    //�������ص�ֵ��������ͼ�������������
    for(k = 0;k<xsize;k++)
        for(l = 0;l<ysize;l++)
        {
            
            for(i = 0;i<xsize;i++)
                for(j = 0;j<ysize;j++)
                {
                    if(*(image_out+j*xsize+i)<=temp  )
                    {
                        *(image_mask+l*xsize+k) = j*xsize+i;
                        temp = *(image_out+j*xsize+i);                        
                    }
                }
            *(image_out+*(image_mask+l*xsize+k)) = 255;
            temp = 254;
        }
     

    //�����ͼ��ĳ�ʼֵΪ-1
    for(i = 0;i<xsize;i++)
        for(j = 0;j<ysize;j++)
        {
            *(image_out+j*xsize+i) = -1;
        }
    
    int    h = 0;
    for(k = 0;k<xsize;k++)
        for(l = 0;l<ysize;l++)
        {

            if(*(image_in+*(image_mask+l*xsize+k)) == h)
            {
     label1:    *(image_out+*(image_mask+l*xsize+k)) = MASK;
                for(m = 1;m<xsize-1;m++)
                    for(n = 1;n<ysize-1;n++)
                    {
                        if(*(image_out+(n-1)*xsize+(m-1)) == MASK
                            ||*(image_out+(n-1)*xsize+m) == MASK
                            ||*(image_out+(n-1)*xsize+(m+1)) == MASK
                            ||*(image_out+n*xsize+(m-1)) == MASK
                            ||*(image_out+n*xsize+(m+1)) == MASK
                            ||*(image_out+(n+1)*xsize+(m-1)) == MASK
                            ||*(image_out+(n+1)*xsize+m) == MASK
                            ||*(image_out+(n+1)*xsize+(m+1))==MASK)
                        {
                            //����ͼ��imd(p) = 1
                            *(image_d+n*xsize+m) = 1;
                            
                            //����p�������
                            {
                                QU->rear = (QU->rear+1)%SIZE;
                                QU->q[QU->rear] = *(image_mask+n*xsize+m);
                            }
                        }
                    }
                //��ǰ��������Ϊ1
                Current_Dist = 1;

                //�����������������
                const int VPIXEL = -100;
                {
                    QU->rear = (QU->rear+1)%SIZE;
                    QU->q[QU->rear] = VPIXEL;
                }

                for(;;)
                {
                    //�����е�һ��������и�p
                    {
                        QU->front = (QU->front+1)%SIZE;
                        *(image_mask+l*xsize+k) = QU->q[QU->front];
                    }

                    if(*(image_mask+l*xsize+k) == VPIXEL)
                    {

                      //������пգ����˳�ѭ��
                      if(QU->front == QU->rear)
                        break;

                      //���򣬽��������ؽ����У���ǰ�����1,��ȡ�������е�һ����
                      else
                      {
                        {
                          QU->rear = (QU->rear+1)%SIZE;
                          QU->q[QU->rear] = VPIXEL;
                        }
                        Current_Dist++;
                        {
                          QU->front = (QU->front+1)%SIZE;
                          p1 = QU->q[QU->front];
                        }
                      }
                    }
                    else 
                    {
                        //�Ըõ�İ�����������

                        //���ϵ�
                        if(*(image_d+*(image_mask+l*xsize+k)-xsize-1)<Current_Dist && (*(image_out+*(image_mask+l*xsize+k)-xsize-1)>0 || *(image_out+*(image_mask+l*xsize+k)-xsize-1) == WSHED))
                        {
                            if(*(image_out+*(image_mask+l*xsize+k)-xsize-1) >0)
                            {
                                if(*(image_out+*(image_mask+l*xsize+k)) == MASK || *(image_out+*(image_mask+l*xsize+k)) == WSHED)
                                    *(image_out+*(image_mask+l*xsize+k)) = *(image_out+*(image_mask+l*xsize+k)-xsize-1);
                                else
                                {
                                    if(*(image_out+*(image_mask+l*xsize+k)) != *(image_out+*(image_mask+l*xsize+k)-xsize-1))
                                        *(image_out+*(image_mask+l*xsize+k)) = WSHED;
                                }
                            }
                            else
                            {
                                if(*(image_out+*(image_mask+l*xsize+k)) == -1)

                                {
                                    *(image_out+*(image_mask+l*xsize+k)) = WSHED;
                                }
                            }
                        }
                        else
                        {
                            if(*(image_out+*(image_mask+l*xsize+k)-xsize-1) == MASK && *(image_d+*(image_mask+l*xsize+k)-xsize-1) == 0)
                            {
                                *(image_d+*(image_mask+l*xsize+k)-xsize-1) = Current_Dist+1;
                                {
                                   QU->rear = (QU->rear+1)%SIZE;
                                   QU->q[QU->rear] = *(image_mask+l*xsize+k)-xsize-1;
                                }
                            }
                        }


                        //���е�
                        if(*(image_d+*(image_mask+l*xsize+k)-xsize)<Current_Dist && (*(image_out+*(image_mask+l*xsize+k)-xsize)>0 || *(image_out+*(image_mask+l*xsize+k)-xsize) == WSHED))
                        {
                            if(*(image_out+*(image_mask+l*xsize+k)-xsize) >0)
                            {
                                if(*(image_out+*(image_mask+l*xsize+k)) == MASK || *(image_out+*(image_mask+l*xsize+k)) == WSHED)
                                    *(image_out+*(image_mask+l*xsize+k)) = *(image_out+*(image_mask+l*xsize+k)-xsize);
                                else
                                {
                                    if(*(image_out+*(image_mask+l*xsize+k)) != *(image_out+*(image_mask+l*xsize+k)-xsize))
                                        *(image_out+*(image_mask+l*xsize+k)) = WSHED;
                                }
                            }
                            else
                            {
                                if(*(image_out+*(image_mask+l*xsize+k)) == -1)

                                {
                                    *(image_out+*(image_mask+l*xsize+k)) = WSHED;
                                }
                            }
                        }
                        else
                        {
                            if(*(image_out+*(image_mask+l*xsize+k)-xsize) == MASK && *(image_d+*(image_mask+l*xsize+k)-xsize) == 0)
                            {
                                *(image_out+*(image_mask+l*xsize+k)-xsize) = Current_Dist+1;
                                {
                                   QU->rear = (QU->rear+1)%SIZE;
                                   QU->q[QU->rear] = *(image_mask+l*xsize+k)-xsize;
                                }
                            }
                        }

                        //���µ�
                        if(*(image_d+*(image_mask+l*xsize+k)-xsize+1)<Current_Dist && (*(image_out+*(image_mask+l*xsize+k)-xsize+1)>0 || *(image_out+*(image_mask+l*xsize+k)-xsize+1) == WSHED))
                        {
                            if(*(image_out+*(image_mask+l*xsize+k)-xsize+1) >0)
                            {
                                if(*(image_out+*(image_mask+l*xsize+k)) == MASK || *(image_out+*(image_mask+l*xsize+k)) == WSHED)
                                    *(image_out+*(image_mask+l*xsize+k)) = *(image_out+*(image_mask+l*xsize+k)-xsize+1);
                                else
                                {
                                    if(*(image_out+*(image_mask+l*xsize+k)) != *(image_out+*(image_mask+l*xsize+k)-xsize+1))
                                        *(image_out+*(image_mask+l*xsize+k)) = WSHED;
                                }
                            }
                            else
                            {
                                if(*(image_out+*(image_mask+l*xsize+k)) == -1)

                                {
                                    *(image_out+*(image_mask+l*xsize+k)) = WSHED;
                                }
                            }
                        }
                        else
                        {
                            if(*(image_out+*(image_mask+l*xsize+k)-xsize+1) == MASK && *(image_d+*(image_mask+l*xsize+k)-xsize+1) == 0)
                            {
                                *(image_d+*(image_mask+l*xsize+k)-xsize+1) = Current_Dist+1;
                                {
                                   QU->rear = (QU->rear+1)%SIZE;
                                   QU->q[QU->rear] = *(image_mask+l*xsize+k)-xsize+1;
                                }
                            }
                        }

                        //���ϵ�
                        if(*(image_d+*(image_mask+l*xsize+k)-1)<Current_Dist && (*(image_out+*(image_mask+l*xsize+k)-1)>0 || *(image_out+*(image_mask+l*xsize+k)-1) == WSHED))
                        {
                            if(*(image_out+*(image_mask+l*xsize+k)-1) >0)
                            {
                                if(*(image_out+*(image_mask+l*xsize+k)) == MASK || *(image_out+*(image_mask+l*xsize+k)) == WSHED)
                                    *(image_out+*(image_mask+l*xsize+k)) = *(image_out+*(image_mask+l*xsize+k)-1);
                                else
                                {
                                    if(*(image_out+*(image_mask+l*xsize+k)) != *(image_out+*(image_mask+l*xsize+k)-1))
                                        *(image_out+*(image_mask+l*xsize+k)) = WSHED;
                                }
                            }
                            else
                            {
                                if(*(image_out+*(image_mask+l*xsize+k)) == -1)

                                {
                                    *(image_out+*(image_mask+l*xsize+k)) = WSHED;
                                }
                            }
                        }
                        else
                        {
                            if(*(image_out+*(image_mask+l*xsize+k)-1) == MASK && *(image_d+*(image_mask+l*xsize+k)-1) == 0)
                            {
                                *(image_d+*(image_mask+l*xsize+k)-1) = Current_Dist+1;
                                {
                                   QU->rear = (QU->rear+1)%SIZE;
                                   QU->q[QU->rear] = *(image_mask+l*xsize+k)-1;
                                }
                            }
                        }

                        //���µ�
                        if(*(image_d+*(image_mask+l*xsize+k)+1)<Current_Dist && (*(image_out+*(image_mask+l*xsize+k)+1)>0 || *(image_out+*(image_mask+l*xsize+k)+1) == WSHED))
                        {
                            if(*(image_out+*(image_mask+l*xsize+k)+1) >0)
                            {
                                if(*(image_out+*(image_mask+l*xsize+k)) == MASK || *(image_out+*(image_mask+l*xsize+k)) == WSHED)
                                    *(image_out+*(image_mask+l*xsize+k)) = *(image_out+*(image_mask+l*xsize+k)+1);
                                else
                                {
                                    if(*(image_out+*(image_mask+l*xsize+k)) != *(image_out+*(image_mask+l*xsize+k)+1))
                                        *(image_out+*(image_mask+l*xsize+k)) = WSHED;
                                }
                            }
                            else
                            {
                                if(*(image_out+*(image_mask+l*xsize+k)) == -1)

                                {
                                    *(image_out+*(image_mask+l*xsize+k)) = WSHED;
                                }
                            }
                        }
                        else
                        {
                            if(*(image_out+*(image_mask+l*xsize+k)+1) == MASK && *(image_d+*(image_mask+l*xsize+k)+1) == 0)
                            {
                                *(image_d+*(image_mask+l*xsize+k)+1) = Current_Dist+1;
                                {
                                   QU->rear = (QU->rear+1)%SIZE;
                                   QU->q[QU->rear] = *(image_mask+l*xsize+k)+1;
                                }
                            }
                        }

                        //���ϵ�
                        if(*(image_d+*(image_mask+l*xsize+k)+xsize-1)<Current_Dist && (*(image_out+*(image_mask+l*xsize+k)+xsize-1)>0 || *(image_out+*(image_mask+l*xsize+k)+xsize-1) == WSHED))
                        {
                            if(*(image_out+*(image_mask+l*xsize+k)+xsize-1) >0)
                            {
                                if(*(image_out+*(image_mask+l*xsize+k)) == MASK || *(image_out+*(image_mask+l*xsize+k)) == WSHED)
                                    *(image_out+*(image_mask+l*xsize+k)) = *(image_out+*(image_mask+l*xsize+k)+xsize-1);
                                else
                                {
                                    if(*(image_out+*(image_mask+l*xsize+k)) != *(image_out+*(image_mask+l*xsize+k)+xsize-1))
                                        *(image_out+*(image_mask+l*xsize+k)) = WSHED;
                                }
                            }
                            else
                            {
                                if(*(image_out+*(image_mask+l*xsize+k)) == -1)

                                {
                                    *(image_out+*(image_mask+l*xsize+k)) = WSHED;
                                }
                            }
                        }
                        else
                        {
                            if(*(image_out+*(image_mask+l*xsize+k)+xsize-1) == MASK && *(image_d+*(image_mask+l*xsize+k)+xsize-1) == 0)
                            {
                                *(image_d+*(image_mask+l*xsize+k)+xsize-1) = Current_Dist+1;
                                {
                                   QU->rear = (QU->rear+1)%SIZE;
                                   QU->q[QU->rear] = *(image_mask+l*xsize+k)+xsize-1;
                                }
                            }
                        }

                        //���е�
                        if(*(image_d+*(image_mask+l*xsize+k)+xsize)<Current_Dist && (*(image_out+*(image_mask+l*xsize+k)+xsize)>0 || *(image_out+*(image_mask+l*xsize+k)+xsize) == WSHED))
                        {
                            if(*(image_out+*(image_mask+l*xsize+k)+xsize) >0)
                            {
                                if(*(image_out+*(image_mask+l*xsize+k)) == MASK || *(image_out+*(image_mask+l*xsize+k)) == WSHED)
                                    *(image_out+*(image_mask+l*xsize+k)) = *(image_out+*(image_mask+l*xsize+k)+xsize);
                                else
                                {
                                    if(*(image_out+*(image_mask+l*xsize+k)) != *(image_out+*(image_mask+l*xsize+k)+xsize))
                                        *(image_out+*(image_mask+l*xsize+k)) = WSHED;
                                }
                            }
                            else
                            {
                                if(*(image_out+*(image_mask+l*xsize+k)) == -1)

                                {
                                    *(image_out+*(image_mask+l*xsize+k)) = WSHED;
                                }
                            }
                        }
                        else
                        {
                            if(*(image_out+*(image_mask+l*xsize+k)+xsize) == MASK && *(image_d+*(image_mask+l*xsize+k)+xsize) == 0)
                            {
                                *(image_d+*(image_mask+l*xsize+k)+xsize) = Current_Dist+1;
                                {
                                   QU->rear = (QU->rear+1)%SIZE;
                                   QU->q[QU->rear] = *(image_mask+l*xsize+k)+xsize;
                                }
                            }
                        }

                        //���µ�
                        if(*(image_d+*(image_mask+l*xsize+k)+xsize+1)<Current_Dist && (*(image_out+*(image_mask+l*xsize+k)+xsize+1)>0 || *(image_out+*(image_mask+l*xsize+k)+xsize+1) == WSHED))
                        {
                            if(*(image_out+*(image_mask+l*xsize+k)+xsize+1) >0)
                            {
                                if(*(image_out+*(image_mask+l*xsize+k)) == MASK || *(image_out+*(image_mask+l*xsize+k)) == WSHED)
                                    *(image_out+*(image_mask+l*xsize+k)) = *(image_out+*(image_mask+l*xsize+k)+xsize+1);
                                else
                                {
                                    if(*(image_out+*(image_mask+l*xsize+k)) != *(image_out+*(image_mask+l*xsize+k)+xsize+1))
                                        *(image_out+*(image_mask+l*xsize+k)) = WSHED;
                                }
                            }
                            else
                            {
                                if(*(image_out+*(image_mask+l*xsize+k)) == -1)

                                {
                                    *(image_out+*(image_mask+l*xsize+k)) = WSHED;
                                }
                            }
                        }
                        else
                        {
                            if(*(image_out+*(image_mask+l*xsize+k)+xsize+1) == MASK && *(image_d+*(image_mask+l*xsize+k)+xsize+1) == 0)
                            {
                                *(image_d+*(image_mask+l*xsize+k)+xsize+1) = Current_Dist+1;
                                {
                                   QU->rear = (QU->rear+1)%SIZE;
                                   QU->q[QU->rear] = *(image_mask+l*xsize+k)+xsize+1;
                                }
                            }
                        }


                    }
                }

                //���ڵ�ǰ��Ϊh�Ľڵ�p
                for(x = 1;x<xsize-1;x++)
                    for(y = 1;y<ysize-1;y++)
                    {
                        if(*(image_in+*(image_mask+y*xsize+x)) == h)
                        {
                            *(image_d+*(image_mask+y*xsize+x)) = 0;
                            if(*(image_out+*(image_mask+y*xsize+x)) != MASK)
                            {
                                Current_Label++;
                                {
                                   QU->rear = (QU->rear+1)%SIZE;
                                   QU->q[QU->rear] = *(image_mask+y*xsize+x);
                                }
                                *(image_out+*(image_mask+y*xsize+x)) = Current_Label;
                                
                                //������в���
                                if(QU->front != QU->rear)
                                {

                                    //�����е�һ���ڵ��Ƴ���p'
                                    {
                                        QU->front = (QU->front+1)%SIZE;                                
                                         p2  =QU->q[QU->front];
                                    }
                                    int a[8] = {0};
                                    a[0] = -xsize-1;
                                    a[1] = -xsize;
                                    a[2] = -xsize+1;
                                    a[3] = -1;
                                    a[4] = 1;
                                    a[5] = xsize-1;
                                    a[6] = xsize;
                                    a[7] = xsize+1;
                                    for(int z = 0;z<8;z++)
                                    {
                                        //���p''�����ڵ�ǰ��ڵ���δ���
                                        if(*(image_in+p2+a[z]) == h && *(image_out+p2+a[z]) != MASK)
                                        {
                                            {
                                              QU->rear = (QU->rear+1)%SIZE;
                                              QU->q[QU->rear] = p2+a[z];
                                            }
                                            *(image_out+p2+a[z]) = Current_Label;
											
                                        }
                                    }
                                }
                            }
                        }
                    }
            }

            else
            {
                h = *(image_in+*(image_mask+l*xsize+k));
                goto label1;
            }
      }
    delete[] QU;
}

int main(int argc, char**argv )
{
	int xsize,ysize,len;
    //IplImage* pImg;
	//IplImage* img_gray;
	//pImg=cvLoadImage("r_lena.bmp");
	Mat pImg = imread("lena.jpg"); //����Mat����������lena����Ƭ
	//Mat img_gray = cvCreateImage(cvGetSize(pImg),8,1);        // �Ҷ�ͼ��
	//cvCvtColor(pImg,img_gray,CV_BGR2GRAY); 
	xsize=pImg.rows;
	ysize=pImg.cols;
    len=xsize*ysize;

	uchar* image_in;
    uchar* image_d;
	uchar* image_mask;
	uchar* image_out;
	image_in=pImg.data;
	image_d=pImg.data;
	image_mask=pImg.data;
	image_out=pImg.data;
	 
	Watershed(image_in,xsize,ysize,image_d ,image_mask,image_out);

	cvNamedWindow("Image1",1);
	imshow("Image1",pImg); 
	cvWaitKey(0);
	cvDestroyWindow("Image1");


	cvNamedWindow("Image",1);
	cvShowImage("Image",image_out);
	cvWaitKey(0);
	cvDestroyWindow("Image");

}