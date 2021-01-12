#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {

    //VideoCapture capture("./Traffic_Light.mp4");
    VideoCapture capture("./test.mp4");

    if (!capture.isOpened()) {
        printf("Can't open the video");
        return 0;
    }

    //opencv 이미지 변수
    Mat frame;

    while (1) {
        capture >> frame;
        if (frame.empty()) break;

        Mat gray;
        cvtColor(frame, gray, CV_BGR2GRAY);
        Mat blur;
        GaussianBlur(gray, blur, Size(0, 0), 1.0);

        // 원 검출
        vector<Vec3f> circles;
        HoughCircles(blur, circles, CV_HOUGH_GRADIENT, 1, 50, 120, 50, 50, 90);

        for (size_t i = 0; i < circles.size(); i++) {
            Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            circle(frame, center, radius, Scalar(0, 0, 255), 3, 8, 0);

            int x1 = int(cvRound(circles[i][0] - radius));
            int y1 = int(cvRound(circles[i][1] - radius));
            Rect rect(x1, y1, 2 * radius, 2 * radius);
            Mat crop = frame(rect);
            //imshow("crop", crop);

            int cw = rect.size().width;
            int ch = rect.size().height;

            // 신호등 영역 ROI mask 영상
            Mat mask(cw, ch, CV_8UC1, Scalar::all(0));
            Point crop_center(int(cw / 2), int(ch / 2));
            circle(mask, crop_center, radius, Scalar::all(255) , -1, 8, 0);

            //imshow("mask", mask);

            // 색 인식
            Mat hsv;
            cvtColor(crop, hsv, CV_BGR2HSV);
            vector<Mat> channels;
            split(hsv, channels);
            channels[0] += 30;
            merge(channels, hsv);

            float mean_hue = mean(hsv, mask)[0];

            printf("%f \n", mean_hue);
            
            string color = "none";
            if (mean_hue > 0 && mean_hue < 60) {
                color = "red";
            }
            else if (mean_hue > 70 && mean_hue < 150) {
                color = "green";
            }
            putText(frame, color, center, CV_FONT_HERSHEY_SIMPLEX, 0.75, Scalar::all(255));

            imshow("frame", frame);
        }

        if (waitKey(20) == 27) break;
    }
    
    capture.release();
    destroyAllWindows();

	return 0;
}