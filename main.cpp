#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {

    VideoCapture capture("./Traffic_Light.mp4");

    if (!capture.isOpened()) {
        printf("Can't open the video");
        return 0;
    }

    //opencv 이미지 변수
    Mat frame_light;

    while (1) {
        capture >> frame_light;
        if (frame_light.empty()) break;

        Mat gray_light;
        cvtColor(frame_light, gray_light, CV_BGR2GRAY);
        Mat blur_light;
        GaussianBlur(gray_light, blur_light, Size(0, 0), 1.0);

        // 원 검출
        vector<Vec3f> circles;
        HoughCircles(blur_light, circles, CV_HOUGH_GRADIENT, 1, 50, 120, 50, 50, 90);

        for (size_t i = 0; i < circles.size(); i++) {
            Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            circle(frame, center, radius, Scalar(0, 0, 255), 3, 8, 0);

            int x1 = int(cvRound(circles[i][0] - radius));
            int y1 = int(cvRound(circles[i][1] - radius));
            Rect rect(x1, y1, 2 * radius, 2 * radius);
            Mat crop_light = frame(rect);
            //imshow("crop", crop);

            int cw = rect.size().width;
            int ch = rect.size().height;

            // 신호등 영역 ROI mask 영상
            Mat mask_light(cw, ch, CV_8UC1, Scalar::all(0));
            Point crop_center(int(cw / 2), int(ch / 2));
            circle(mask_light, crop_center, radius, Scalar::all(255) , -1, 8, 0);

            //imshow("mask", mask);

            // 색 인식
            Mat hsv_light;
            cvtColor(crop_light, hsv_light, CV_BGR2HSV);
            vector<Mat> channels;
            split(hsv_light, channels);
            channels[0] += 30;
            merge(channels, hsv_light);

            float mean_hue_light = mean(hsv_light, mask_light)[0];

            printf("%f \n", mean_hue_light);
            
            string color = "none";
            if (mean_hue_light > 0 && mean_hue_light < 60) {
                color = "red";
            }
            else if (mean_hue_light > 70 && mean_hue_light < 150) {
                color = "green";
            }
            putText(frame_light, color, center, CV_FONT_HERSHEY_SIMPLEX, 0.75, Scalar::all(255));

            imshow("frame", frame_light);
        }

        if (waitKey(20) == 27) break;
    }
    
    capture.release();
    destroyAllWindows();

	return 0;
}
