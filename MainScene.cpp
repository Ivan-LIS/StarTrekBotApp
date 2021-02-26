#include "MainScene.h"
#include "BaseFunctions.h"



int MainScene::findImageCoord(Mat& SearchImg, int& x, int& y)
{
    Point locat(0, 0);
    Mat ftmp;

    ftmp.create(src.rows + 1 - SearchImg.rows, src.cols + 1 - SearchImg.cols, CV_8UC4);
    matchTemplate(src, SearchImg, ftmp, 5);
    threshold(ftmp, ftmp, trshld, 1, 0);
    for (int r = 0; r < ftmp.rows; ++r) {
        const float* row = ftmp.ptr<float>(r);
        for (int c = 0; c < ftmp.cols; ++c) {
            if (row[c] >= 0.999999) // = thresholdedImage(r,c) == 0
            {
                x = c;
                y = r;
                return 1;
            }
        }
    }
    return 0;
}

void MainScene::StartProcess(void)
{
    if (!src.empty())
        src = hwnd2mat(GameWindow);
    else
    {
        src.release();
        src = hwnd2mat(GameWindow);
    }

}

void MainScene::EndProcess(bool isDraw)
{
    if (isDraw)
        imshow("Display window", src);
    if (!src.empty())
        src.release();
    shipCount = 0;
}

Mat& MainScene::GetSrcMain(void)
{
    return src;
}
int MainScene::GetCount(void)
{
    return shipCount;
}
ship* MainScene::GetShipsList(void)
{
    return shipsList;
}
MainScene::MainScene(void)
{
    shipCount = 0;
    trshld = 0.85;
    GameWindow = NULL;
}
MainScene::MainScene(HWND GW)
{
    shipCount = 0;
    trshld = 0.85;
    GameWindow = GW;
}
void MainScene::SetWindow(HWND GW)
{
    GameWindow = GW;
}
void MainScene::SetTreshold(float trs)
{
    trshld = trs;
}
void MainScene::ClearList()
{
    shipCount = 0;
}

int MainScene::FindShip(Mat& ShipImg, Rect ROI)
{
    Point locat(0, 0);
    Mat ftmp;

    ftmp.create(src.rows + 1 - ShipImg.rows, src.cols + 1 - ShipImg.cols, CV_8UC4);
    matchTemplate(src, ShipImg, ftmp, 5);
    threshold(ftmp, ftmp, trshld, 1, 0);
    for (int r = 0; r < ftmp.rows; ++r) {
        const float* row = ftmp.ptr<float>(r);
        for (int c = 0; c < ftmp.cols; ++c) {
            if (row[c] >= 0.999999) // = thresholdedImage(r,c) == 0
            {
                if ((c > ROI.x) && (c < (ROI.x + ROI.width)) && (r > ROI.y) && (r < (ROI.y + ROI.height)))
                {
                    shipsList[shipCount].x = c;
                    shipsList[shipCount].y = r;
                    shipCount++;
                }
                circle(ftmp, Point(c + ShipImg.cols / 2, r + ShipImg.rows / 2), ShipImg.cols / 2, Scalar(0, 0, 0), -1);

            }
        }
    }
    return shipCount;
}

ship MainScene::GetNearShip(int x, int y)
{
    ship nearSH{ 0,0 };
    double MIN_value = 0;
    if (shipCount > 0)
    {
        MIN_value = (x - shipsList[0].x) * (x - shipsList[0].x) + (y - shipsList[0].y) * (y - shipsList[0].y);
        nearSH.x = shipsList[0].x;
        nearSH.y = shipsList[0].y;
    }
    for (int i = 0; i < shipCount; i++)
        if (MIN_value > ((x - shipsList[i].x) * (x - shipsList[i].x) + (y - shipsList[i].y) * (y - shipsList[i].y)))
        {
            MIN_value = (x - shipsList[i].x) * (x - shipsList[i].x) + (y - shipsList[i].y) * (y - shipsList[i].y);
            nearSH.x = shipsList[i].x;
            nearSH.y = shipsList[i].y;

        }
    return nearSH;
}