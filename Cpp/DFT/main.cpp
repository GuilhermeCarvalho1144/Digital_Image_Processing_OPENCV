/******************************************************************************
                                 DFT Example

This program is a code example in how to take the Discrete Fourier Transform
(DFT) of an image. This kind of processing is used to take the frequency domain
component of an image

This code is just a simple example in how to use the DFT from a RGB image and
then calculate the inverse DFT to put obtain the original image

 By: Guilherme Carvalho
******************************************************************************/

//****************************************************************************
//Libraries used
#include <QCoreApplication>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <stdint.h>

//****************************************************************************
//Using namespcaes to make the code cleaner
using namespace std;
using namespace cv;

/******************************************************************************
This function computes the DFT of an image pass by reference and return the DFT
matrix (Imaginary and real parts)
******************************************************************************/

Mat calculateDFT(Mat& src){
    Mat dftComplex[2] = {src, Mat::zeros(src.size(), CV_32FC1)};//channel 1 dftImg channel 2 zeros
    Mat dftReady;
    merge(dftComplex, 2, dftReady);
    Mat dftOutput;
    dft(dftReady, dftOutput, DFT_COMPLEX_OUTPUT);
    return dftOutput;
}

/******************************************************************************
This function recenter the DFT image, so we are able to analysis it. This data
have no correlation with the real image and SHOULD NOT be use in any kind of
processing
******************************************************************************/

void centerDFT(Mat& src){
    int midX = src.cols /2;
    int midY = src.rows /2;

    //creating the quadrants
    Mat q1 (src, Rect(0, 0, midX, midY));
    Mat q2 (src, Rect(midX, 0, midX, midY));
    Mat q3 (src, Rect(0, midY, midX, midY));
    Mat q4 (src, Rect(midX, midY, midX, midY));

    Mat temp;

    //swaping
    q1.copyTo(temp);
    q4.copyTo(q1);
    temp.copyTo(q4);

    q2.copyTo(temp);
    q3.copyTo(q2);
    temp.copyTo(q3);


}

/******************************************************************************
This function computes the inverse DFT of an image pass by reference and return
the DFT matrix (Only the real part)
******************************************************************************/

Mat invertDFT(Mat& src){
    Mat inverse;
    dft(src, inverse, DFT_INVERSE | DFT_REAL_OUTPUT | DFT_SCALE);
    return inverse;
}

/******************************************************************************
The main function
******************************************************************************/
int main(void){
    //defining the Mat objects
    Mat imgOriginal = imread("allcolors.png", CV_LOAD_IMAGE_COLOR);
    Mat dftImgOriginal;
    Mat dftImg[3];
    Mat dftOutput[3];
    Mat dftInverse[3];
   //spliting the channels
   split(imgOriginal, dftImg);
   //taking the dft for each channel
   for(int i = 0; i < 3; i++){
       dftImg[i].convertTo(dftImg[i], CV_32FC1, 1.0/255.0);// Coverting the image uint8 to float
       dftOutput[i] = calculateDFT(dftImg[i]);
       // to show the DFT we need to slipt the channels
       Mat dftSplit [2] = {Mat::zeros(dftOutput[i].size(), CV_32FC1), Mat::zeros(dftOutput[i].size(), CV_32FC1)};
       split(dftOutput[i], dftSplit);
       //Taking the magnitude
       Mat dftMagnitude;
       magnitude(dftSplit[0], dftSplit[1], dftMagnitude);
       //to normalize we need to add 1 anda take the log because the range is too big
       dftMagnitude += Scalar::all(1);
       log(dftMagnitude, dftMagnitude);
       normalize(dftMagnitude, dftMagnitude, 0, 1, CV_MINMAX);
       imshow("No center DFT", dftMagnitude);
       centerDFT(dftMagnitude);
       imshow("Center DFT", dftMagnitude);//DO NOT USE THIS DATA FOR PROCESSING
       dftInverse[i] = invertDFT(dftOutput[i]);
       imshow("Inverse DFT",dftInverse[i]);
       waitKey(0);
   }
   //merging the results
   merge(dftInverse, 3, dftImgOriginal);
   if(dftImgOriginal.empty()) cout<<"imagem vazia";
   else imshow("Inverse DFT from all the channels merged", dftImgOriginal);
   waitKey(0);
}
