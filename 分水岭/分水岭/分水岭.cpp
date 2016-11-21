#include <opencv2/imgproc/imgproc.hpp>   
#include <opencv2/objdetect/objdetect.hpp>   
#include <opencv2/highgui/highgui.hpp>   
#include<vector>   
#include<iostream>   
#include<queue>   
#include<fstream>
using namespace cv; //下面的所有cv相关类型不用加上前缀了
Mat marker_mask;  
Mat g_markers;  
Mat img0, img, img_gray,wshed;  
Point_<int> prev_pt(-1,-1);  
using std::vector;  
using std::queue;  
static void my_watershed(Mat img,Mat& markers,int comp_count);  
static void mouse_event(int event,int x, int y,int flags, void*)  
{  
    if(img.rows==0)  
        return;  
     if(event==CV_EVENT_LBUTTONUP||!(flags&CV_EVENT_FLAG_LBUTTON))  
         prev_pt=Point_<int>(-1,-1);  
     else if(event==CV_EVENT_LBUTTONDOWN)  
         prev_pt=Point2i(x,y);  
     else if(event==CV_EVENT_MOUSEMOVE&&(flags&CV_EVENT_FLAG_LBUTTON))  
     {  
         Point2i pt(x,y);  
         if(prev_pt.x<0)  
             prev_pt=pt;  
         line(marker_mask,prev_pt,pt,Scalar(255,255,255),1,8,0);  
         line(img,prev_pt,pt,Scalar(255,255,255),1,8,0);  
         prev_pt=pt;  
         imshow("image",img);  
     }  
}  
int main()  
{  
    img0=cvLoadImage("2.bmp");  
    img=img0.clone();  
    CvRNG rng = cvRNG(-1);   
    img_gray=img0.clone();  
    wshed=img0.clone();  
    marker_mask=Mat(Size(img0.cols,img0.rows),8,1);  
    g_markers=Mat(Size(img0.cols,img0.rows),CV_32S,1);  
    cvtColor(img,marker_mask,CV_BGR2GRAY);  
    cvtColor(marker_mask,img_gray,CV_GRAY2BGR);  
    for(int i=0;i<marker_mask.rows;i++)  
        for(int j=0;j<marker_mask.cols;j++)  
            marker_mask.at<unsigned char>(i,j)=0;  
    for(int i=0;i<g_markers.rows;i++)  
        for(int j=0;j<g_markers.cols;j++)  
            g_markers.at<int>(i,j)=0;  
    imshow("image",img);  
    imshow("watershed transform",wshed);  
    setMouseCallback("image",mouse_event,0);  
    for(;;)  
    {  
        int c=waitKey(0);  
        if((char)c==27)  
            break;  
        if((char)c=='r')  
        {  
            for(int i=0;i<marker_mask.rows;i++)  
                for(int j=0;j<marker_mask.cols;j++)  
                    marker_mask.at<unsigned char>(i,j)=0;  
            img0.copyTo(img);  
            imshow("image",img);  
        }  
        if((char)c=='w'||(char)c==' ')  
        {  
            vector<vector<Point>> contours;  
            CvMat* color_tab=0;  
            int comp_count=0;  
            findContours(marker_mask,contours,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE,Point(0,0));  
            for(int i=0;i<g_markers.rows;i++)  
                for(int j=0;j<g_markers.cols;j++)  
                        g_markers.at<int>(i,j)=0;  
            vector<vector<Point> >::iterator iter=contours.begin();  
            for(int i=0;i<(int)contours.size();i++)  
            {  
                drawContours(g_markers,contours,i,Scalar::all(comp_count+1),  
                    1,8,vector<cv::Vec4i>());  
                comp_count++;  
            }  
          
            if(comp_count==0)  
                continue;  
            color_tab=cvCreateMat(1,comp_count,CV_8UC3);  
            for(int i=0;i<comp_count;i++)  
            {  
                uchar* ptr=color_tab->data.ptr+i*3;  
                ptr[0]=(uchar)(cvRandInt(&rng)%180+50);  
                ptr[1]=(uchar)(cvRandInt(&rng)%180+50);  
                ptr[2]=(uchar)(cvRandInt(&rng)%180+50);  
            }  
            Mat temp=g_markers.clone();  
  
            double t=(double)cvGetTickCount();  
            //my_watershed(img0,g_markers,comp_count);   
            watershed(img0,g_markers);  
            t=(double)cvGetTickCount()-t;  
            std::cout<<"exec time= "<<t/(cvGetTickFrequency()*1000.)<<std::endl;  
        for(int i=0;i<g_markers.rows;i++)  
            for(int j=0;j<g_markers.cols;j++)  
            {  
                int idx=g_markers.at<int>(i,j);  
                uchar* dst=&wshed.at<uchar>(i,j*3);  
                if(idx==-1)  
                    dst[0]=dst[1]=dst[2]=(uchar)255;  
                else if(idx<=0||idx>comp_count)  
                    dst[0]=dst[1]=dst[2]=(uchar)8;  
                else{  
                    uchar* ptr=color_tab->data.ptr+(idx-1)*3;  
                    dst[0]=ptr[0];dst[1]=ptr[1];dst[2]=ptr[2];  
                }  
            }  
            addWeighted(wshed,0.5,img_gray,0.5,0,wshed);  
            imshow("watershed transform",wshed);  
            cvReleaseMat(&color_tab);  
        }  
    }  
    return 0;  
}  
static void my_watershed(Mat img0,Mat& markers,int comp_count)  
{  
    Mat gray=Mat(Size(img0.rows,img0.cols),8,1);  
    cvtColor(img0,gray,CV_BGR2GRAY);  
    Mat imge=Mat(Size(img0.rows,img0.cols),8,1);  
    Sobel(gray,imge,CV_8U,1,1);  
    vector<queue<Point2i>*>Labeleddata;//图像中各连通区域的点   
    queue<Point2i>* pque;//某连通区域已包含的点   
    queue<Point2i> quetem; //用于提取某连通区域中输入种子点中的初始种子点   
    vector<int*> SeedCounts;  
    int* Array;  
    Point2i temp;  
    int row=imge.rows,col=imge.cols;  
    Mat marker_saved=markers.clone();  
    bool up,down,right,left,uplef,uprig,downlef,downrig;  
    int m,n;  
    for(int i=0;i<comp_count;i++)  
    {  
        Array=new int[256];  
        SeedCounts.push_back(Array);//统计某waterlevel的各个连通区域中种子点的个数   
        pque=new queue<Point2i>[256];   
        Labeleddata.push_back(pque);//存储该连通区域中种子生长所得的点         
    }  
    for(int i=0;i<row;i++)  
        for(int j=0;j<col;j++)  
        {  
            if(markers.at<int>(i,j)>0)  
            {  
                temp.x=i;  
                temp.y=j;  
                quetem.push(temp);  
                int num=markers.at<int>(i,j);  
                markers.at<int>(i,j)=-1;//该点已处理,其他种子点生长时将绕过该点   
                while(!quetem.empty())  
                {  
                    up=down=right=left=uplef=uprig=downlef=downrig=false;  
                    temp=quetem.front(); //提取出一个点，在该点的八连通区域内寻找可生长点   
                    m=temp.x;  
                    n=temp.y;  
                    quetem.pop();  
  
                    if(m-1>=0)//若上方可生长则添加为新种子   
                    {  
                        if(markers.at<int>(m-1,n)==num)  
                        {  
                            temp.x=m-1;  
                            temp.y=n;  
                            quetem.push(temp);  
                            markers.at<int>(m-1,n)=-1;  
                        }  
                        else{  
                            up=true;  
                        }  
                    }  
                    if(m-1>=0&&n-1>=0)  
                    {  
                        if(markers.at<int>(m-1,n-1)==num)  
                        {  
                            temp.x=m-1;  
                            temp.y=n-1;  
                            quetem.push(temp);  
                            markers.at<int>(m-1,n-1)=-1;  
                        }  
                        else{  
                            uplef=true;  
                        }  
                    }  
                    if(m+1<=row-1)  
                    {  
                        if(markers.at<int>(m+1,n)==num)  
                        {  
                            temp.x=m+1;  
                            temp.y=n;  
                            quetem.push(temp);  
                            markers.at<int>(m+1,n)=-1;  
                        }  
                        else{  
                            down=true;  
                        }  
                    }  
                    if(m+1<=row-1&&n+1<=col-1)  
                    {  
                        if(markers.at<int>(m+1,n+1)==num)  
                        {  
                            temp.x=m+1;  
                            temp.y=n+1;  
                            quetem.push(temp);  
                            markers.at<int>(m+1,n+1)=-1;  
                        }  
                        else{  
                            downrig=true;  
                        }  
                    }  
                    if(n+1<=col-1)  
                    {  
                        if(markers.at<int>(m,n+1)==num)  
                        {  
                            temp.x=m;  
                            temp.y=n+1;  
                            quetem.push(temp);  
                            markers.at<int>(m,n+1)=-1;  
                        }  
                        else{  
                            right=true;  
                        }  
                    }  
                    if(m-1>=0&&n+1<=col-1)  
                    {  
                        if(markers.at<int>(m-1,n+1)==num)  
                        {  
                            temp.x=m-1;  
                            temp.y=n+1;  
                            quetem.push(temp);  
                            markers.at<int>(m-1,n+1)=-1;  
                        }  
                        else{  
                            uprig=true;  
                        }  
                    }  
                    if(n-1>=0)  
                    {  
                        if(markers.at<int>(m,n-1)==num)  
                        {  
                            temp.x=m;  
                            temp.y=n-1;  
                            quetem.push(temp);  
                            markers.at<int>(m,n-1)=-1;  
                        }  
                        else{  
                            left=true;  
                        }  
                    }  
                    if(m+1<=row-1&&n-1>=0)  
                    {  
                        if(markers.at<int>(m+1,n-1)==num)  
                        {  
                            temp.x=m+1;  
                            temp.y=n-1;  
                            quetem.push(temp);  
                            markers.at<int>(m+1,n-1)=-1;  
                        }  
                        else{  
                            downlef=true;  
                        }  
                    }  
                    //八连通区域中有未标记点，则该点属于初始种子点   
                    if(up||down||right||left||uplef||downlef||uprig||downrig)  
                    {  
                        temp.x=m;  
                        temp.y=n;  
                        Labeleddata[comp_count-1][imge.at<uchar>(m,n)].push(temp);  
                        SeedCounts[comp_count-1][imge.at<uchar>(m,n)]++;  
                    }  
                }  
            }  
        }  
        bool active;  
        int waterlevel;  
        for(waterlevel=0;waterlevel<180;waterlevel++)  
        {  
            active=true;  
            while(active) //当1-count_com个连通区域都无可生长点时结束循环   
            {  
                active=false;  
                for(int i=0;i<comp_count;i++)//将区域i中将waterlevel梯度以下的点用于区域增长   
                {  
                    if(!Labeleddata[i][waterlevel].empty())//区域增长，经过多次迭代，直至该区域，该waterlevel无可生长点。   
                    {  
                        active=true;      
                        while(SeedCounts[i][waterlevel]>0) //SeedCount中保留了前一轮生长后各区域，各waterlevel的种子点个数，本轮生长结束后，将根据Labeleddata中的元素个数更新   
                        {  
                            SeedCounts[i][waterlevel]--;  
                            temp=Labeleddata[i][waterlevel].front();  
                            Labeleddata[i][waterlevel].pop();  
                            m=temp.x;  
                            n=temp.y;  
                            int num=marker_saved.at<int>(m,n);  
                            if(m-1>=0)  
                            {  
                                if(!marker_saved.at<int>(m-1,n))//上方点未处理过   
                                {  
                                    temp.x=m-1;  
                                    temp.y=n;  
                                    marker_saved.at<int>(m-1,n)=num;  
                                    if(imge.at<uchar>(m-1,n)<=waterlevel)  
                                        Labeleddata[i][waterlevel].push(temp);  
                                    else{  
                                        Labeleddata[i][imge.at<uchar>(m-1,n)].push(temp); //本次生长不处理，可能在waterlevel变化到某值时再用于生长   
                                        SeedCounts[i][imge.at<uchar>(m-1,n)]++;  
                                    }  
                                }  
                            }  
                            if(m+1<=row-1)  
                            {  
                                if(!marker_saved.at<int>(m+1,n))  
                                {  
                                    temp.x=m+1;  
                                    temp.y=n;  
                                    marker_saved.at<int>(m+1,n)=num;  
                                    if(imge.at<uchar>(m+1,n)<=waterlevel)  
                                        Labeleddata[i][waterlevel].push(temp);  
                                    else{  
                                        Labeleddata[i][imge.at<uchar>(m+1,n)].push(temp);  
                                        SeedCounts[i][imge.at<uchar>(m+1,n)]++;  
                                    }  
                                }  
                            }  
                            if(n+1<=col-1)  
                            {  
                                if(!marker_saved.at<int>(m,n+1))  
                                {  
                                    temp.x=m;  
                                    temp.y=n+1;  
                                    marker_saved.at<int>(m,n+1)=num;  
                                    if(imge.at<uchar>(m,n+1)<=waterlevel)  
                                        Labeleddata[i][waterlevel].push(temp);  
                                    else{  
                                        Labeleddata[i][imge.at<uchar>(m,n+1)].push(temp);  
                                        SeedCounts[i][imge.at<uchar>(m,n+1)]++;  
                                    }  
                                }  
                            }  
                            if(n-1>=0)  
                            {  
                                if(!marker_saved.at<int>(m,n-1))  
                                {  
                                    temp.x=m;  
                                    temp.y=n-1;  
                                    marker_saved.at<int>(m,n-1)=num;  
                                    if(imge.at<uchar>(m,n-1)<=waterlevel)  
                                        Labeleddata[i][waterlevel].push(temp);  
                                    else  
                                    {  
                                        Labeleddata[i][imge.at<uchar>(m,n-1)].push(temp);  
                                        SeedCounts[i][imge.at<uchar>(m,n-1)]++;  
                                    }  
                                }  
                            }  
                        }  
                            SeedCounts[i][waterlevel]=Labeleddata[i][waterlevel].size();  
                        }  
                          
                    }  
            }  
        }  
        markers=marker_saved.clone();  
}  