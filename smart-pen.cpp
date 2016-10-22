//include libraries//
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

#include<tesseract\baseapi.h>
#include<leptonica\allheaders.h>

#include<iostream>
#include <fstream>

//smart pen code//
using namespace cv;
using namespace std;

 int main( int argc, char** argv )
 {
    VideoCapture cap(0); //capture the video from webcam


    if ( !cap.isOpened() )  // if not success, exit program
    {
         cout << "Cannot open the web cam" << endl;
         return -1;
    }

    namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	char info[100];
int i=0;

  int iLowH = 22;
 int iHighH = 38;

  int iLowS = 92; 
 int iHighS = 203;

  int iLowV = 75;
 int iHighV = 255;

 //create trackbar for rotation
  //const char* pzRotatingWindowName = "Rotated Video";
    namedWindow( "Rotated Video", CV_WINDOW_AUTOSIZE );

    int iAngle =360;
    createTrackbar("Angle", "Control", &iAngle, 360);




 


  //Create trackbars in "Control" window
 createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
 createTrackbar("HighH", "Control", &iHighH, 179);

  createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
 createTrackbar("HighS", "Control", &iHighS, 255);

  createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
 createTrackbar("HighV", "Control", &iHighV, 255);

  int iLastX = -1; 
 int iLastY = -1;

  //Capture a temporary image from the camera
 Mat imgTmp;
 cap.read(imgTmp); 

  //Create a black image with the size as the camera output
 Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );;
 
 
    while (true)
    {
        Mat imgOriginal;

        bool bSuccess = cap.read(imgOriginal); // read a new frame from video



         if (!bSuccess) //if not success, break loop
        {
             cout << "Cannot read a frame from video stream" << endl;
             break;
        }

    Mat imgHSV;

   cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
 
  Mat imgThresholded;

   inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
      
  //morphological opening (removes small objects from the foreground)
  erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)) );
  dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

   //morphological closing (removes small holes from the foreground)
  dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)) ); 
  erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

   //Calculate the moments of the thresholded image
  Moments oMoments = moments(imgThresholded);

   double dM01 = oMoments.m01;
  double dM10 = oMoments.m10;
  double dArea = oMoments.m00;

   // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
  if (dArea > 10000)
  {
   //calculate the position of the ball
   int posX = dM10 / dArea;
   int posY = dM01 / dArea;        
        
   if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
   {
    //Draw a red line from the previous point to the current point
    line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(255,255,255), 2);
   }

    iLastX = posX;
   iLastY = posY;
   
  }


  namedWindow("HSV",CV_WINDOW_AUTOSIZE);
  imshow("Original", imgOriginal); 
   imshow("Thresholded Image", imgThresholded); //show the thresholded image
  imgOriginal = imgLines;
  Mat imgRotated;
  namedWindow("Rotated",CV_WINDOW_AUTOSIZE);
  

  flip(imgOriginal,imgRotated,0);
   //imshow("Rotated", imgRotated);
  

   
 // //show the original image

  //rotation
  //get the affine transformation matrix
 Mat matRotation = getRotationMatrix2D( Point(imgRotated.cols / 2, imgRotated.rows / 2), (iAngle - 180), 1 );
 
 // Rotate the image
 Mat matRotatedFrame;
 warpAffine(imgRotated, matRotatedFrame, matRotation, imgRotated.size() );

 imshow("Rotated Video", matRotatedFrame );

 
  

  
  char c = waitKey(1);
        if(c==50)
		{ iLastX=-1;
		iLastY=-1;
		}

		
        if (c == 100) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
       {
            
          vector<int> compression_params; //vector that stores the compression parameters of the image

     compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); //specify the compression technique

     compression_params.push_back(98); //specify the compression quality



     bool bSuccess = imwrite("C:\NISHKA.JPG", matRotatedFrame , compression_params); //write the image to file





     if ( !bSuccess )

    {

         cout << "ERROR : Failed to save the image" << endl;

         //system("pause"); //wait for a key press

    }
	 imgOriginal.setTo(Scalar(0,0,0));
		}
		if(c==97)
			{

	 //TESSERACT STARTS//
	 char *outText;
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init(NULL, "eng")) {
        cerr << "could not initialised tessearct" <<endl;
        exit(1);
    }

    // Open input image with leptonica library
    Pix *image = pixRead("C:\NISHKA.JPG");
    api->SetImage(image);
    // Get OCR result

    outText = api->GetUTF8Text();
	 cout<<"OCR output"<<endl<< outText;
	 info[i]= *outText;
	 i++;
   
    // Destroy used object and release memory
    api->End();
    //delete [] outText;
    //pixDestroy(&image);





    //tesseract ends//





	}
    if(c==115)
		{ cout<<endl<<"SPACE\n\n";
	      info[i]=' ';
		  i++;
	}


	if(c==27)
	{ cout << "esc key is pressed by user" << endl;
	
	ofstream myfile;
    myfile.open ("example.txt",ios::app|ios::out);

	for(int j=0;j<i;j++)
	{
	
    myfile << info[j];
	}   

            break; 
       }

    }

   return 0;
}








