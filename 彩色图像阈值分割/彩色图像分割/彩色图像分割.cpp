#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

/// ȫ�ֱ������弰��ֵ

int threshold_value = 0;
int threshold_type = 3;;
int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;

Mat src, src_gray, dst;
char* window_name = "Threshold Demo";

char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
char* trackbar_value = "Value";

/// �Զ��庯������
void Threshold_Demo( int, void* );

/**
 * @������
 */
int main( int argc, char** argv )
{
  /// ��ȡһ��ͼƬ�����ı�ͼƬ�������ɫ���ͣ��ö�ȡ��ʽΪDOS����ģʽ��
  src = imread( "yangpin.jpg" );

  /// ��ͼƬת���ɻҶ�ͼƬ
  cvtColor( src, src_gray, CV_RGB2GRAY );

  /// ����һ��������ʾͼƬ
  namedWindow( window_name, CV_WINDOW_AUTOSIZE );

  /// ������������������ֵ
  createTrackbar( trackbar_type,
                  window_name, &threshold_type,
                  max_type, Threshold_Demo );

  createTrackbar( trackbar_value,
                  window_name, &threshold_value,
                  max_value, Threshold_Demo );

  /// ��ʼ���Զ������ֵ����
  Threshold_Demo( 0, 0 );

  /// �ȴ��û������������ESC�����˳��ȴ����̡�
  while(true)
  {
    int c;
    c = waitKey( 20 );
    if( (char)c == 27 )
      { break; }
   }

}


/**
 * @�Զ������ֵ����
 */
void Threshold_Demo( int, void* )
{
  /* 0: ��������ֵ
     1: ����������ֵ
     2: �ض���ֵ
     3: 0��ֵ
     4: ��0��ֵ
   */

  threshold( src_gray, dst, threshold_value, max_BINARY_value,threshold_type );

  imshow( window_name, dst );
}