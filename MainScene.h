#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <windows.h>


using namespace cv;
using namespace std;

struct ship
{
    int x;
    int y;
};

class MainScene
{
private:
    ship shipsList[50];
    int shipCount;
    double trshld;
    HWND GameWindow;
    Mat src;

public:
    MainScene(void);
    MainScene(HWND GW);
    void SetWindow(HWND GW);
    void SetTreshold(float trs);
    void ClearList();
    int GetCount(void);
    ship* GetShipsList(void);
    Mat& GetSrcMain(void);
    int FindShip(Mat& ShipImg, Rect ROI);
    void StartProcess(void);
    void EndProcess(bool isDraw = 1);
    int findImageCoord(Mat& SearchImg, int& x, int& y);
    ship GetNearShip(int x, int y);
};