// StarTrekBotApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define GLOBAL_TRESHOLD 0.87

#include <iostream>
#include <windows.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "BaseFunctions.h"
#include "MainScene.h"


using namespace cv;
using namespace std;


enum BotStates { findShip, ShipClick, FindButtonFight, ButtonFightClick, WaitEndFight , WaitRepair};

enum ShipStates {shSleep,shGo,shAim,shFight,shRepair};

bool isImageFound(Mat src, Mat SearchImg)
{
    Mat ftmp;

    ftmp.create(src.rows + 1 - SearchImg.rows, src.cols + 1 - SearchImg.cols, CV_8UC4);
    matchTemplate(src, SearchImg, ftmp, 5);
    threshold(ftmp, ftmp, GLOBAL_TRESHOLD, 1, 0);
    for (int r = 0; r < ftmp.rows; ++r) {
        const float* row = ftmp.ptr<float>(r);
        for (int c = 0; c < ftmp.cols; ++c) {
            if (row[c] >= 1) // = thresholdedImage(r,c) == 0
              return true;
        }
    }
    return false;
}

HWND WindowHandle;
//ищем окно MEmu
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    char ClName[50];
    GetWindowTextA(hwnd, ClName, sizeof(ClName));
    if (strcmp(ClName,"MEmu")==0)
    {
        long Style;
        Style = GetWindowLong(hwnd, GWL_STYLE); 
        if (WS_VISIBLE & Style)
        {
            WindowHandle = hwnd;
            return FALSE;
        }
    }
    return true;
}

void MouseClick(HWND hwnd1)
{
    printf("Click HWND %i \n", LONG(hwnd1));

    SendMessage(hwnd1, WM_LBUTTONDOWN, 0, ((300) << 16) | (300));
    Sleep(100);
    
    SendMessage(hwnd1, WM_LBUTTONUP, 0, ((300) << 16) | 300);
    Sleep(100);

}

int main()
{

    Mat image, imageFight, imageShipB, iMiner;
    Mat ishipZZ, ishipAtFight, ishipAim,ishipGo,ishipRepair;
    
    ishipZZ = imread("ShipStates\\shipZZ.png", IMREAD_UNCHANGED);
    ishipAtFight = imread("ShipStates\\shipAtFight.png", IMREAD_UNCHANGED);
    ishipAim = imread("ShipStates\\shipAim.png", IMREAD_UNCHANGED);
    ishipGo = imread("ShipStates\\shipGo.png", IMREAD_UNCHANGED);
    ishipRepair = imread("ShipStates\\shipRepair.png", IMREAD_UNCHANGED);

    // image = imread("issled.png", IMREAD_UNCHANGED); // Read the file
    imageFight = imread("Fight.png", IMREAD_UNCHANGED);
    imageShipB = imread("ShipB.png", IMREAD_UNCHANGED);
    image = imread("miner.png", IMREAD_UNCHANGED);
    /*
    if (!image.data) // Check for invalid input
    {
        cout << "Could not open or find the image" << std::endl;
        return -1;
    }
    */
    int key = 0;
    namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
    //imshow("Display window", image); // Show our image inside it.

    //HWND hw = FindWindow(NULL, TEXT("BlueStacks"));
    //HWND hwGame = FindWindowEx(hw, NULL, NULL, NULL);
    WindowHandle = NULL;
    EnumWindows(EnumWindowsProc, NULL);
    HWND hw = FindWindow(NULL, TEXT("MEmu"));
    
    while ( WindowHandle == NULL)
    {
        Sleep(10);
    }
    hw = WindowHandle;
    MouseClick(hw);
    RECT windowsize;    // get the height and width of the screen
    GetClientRect(hw, &windowsize);
    HWND hwGame = FindWindowEx(hw, NULL, NULL, TEXT("MainWindowWindow"));
    MouseClick(hwGame);
    hwGame = FindWindowEx(hwGame, NULL, NULL, TEXT("CenterWidgetWindow"));
    MouseClick(hwGame);
    hwGame = FindWindowEx(hwGame, NULL, NULL, TEXT("RenderWindowWindow"));
    MouseClick(hwGame);
    hwGame = FindWindowEx(hwGame, NULL, NULL, TEXT("sub"));
    MouseClick(hwGame);
    hwGame = FindWindowEx(hwGame, NULL, NULL, TEXT("sub"));
    MouseClick(hwGame);
    //debug
  /*  int krrr = 0;
    while (krrr != 27)
    {
        Mat tmpDebag = hwnd2mat(hw);
        imshow("Display window", tmpDebag);
        int krrr = waitKey(15);
    };*/
   
    //Sleep(10000);
    //end debug
    
    
   // hwGame = hw;
    
    int err = GetLastError();


    Mat ftmp;
    ftmp.create(windowsize.bottom + 1 - image.rows, windowsize.right + 1 - image.cols, CV_8UC4);

    RECT windowsizeGame;    // get the height and width of the screen
    GetWindowRect(hwGame, &windowsizeGame);
    POINT ChildTopLeft{ windowsizeGame.left, windowsizeGame.top }, ChildBottomRight{ windowsizeGame.right, windowsizeGame.bottom };
    ScreenToClient(hw, &ChildTopLeft);
    ScreenToClient(hw, &ChildBottomRight);

    GetClientRect(hwGame, &windowsizeGame);
    int i = 0;
    bool fightMode = false;
    int fightModeCount = 0;

    MainScene MainSc(hwGame);

    BotStates currState;
    currState = findShip;
    int StateTime = 0;
    int x = 0, y = 0;
    ship clck;

    Rect myROIShipsCoord;
    myROIShipsCoord.x = windowsizeGame.right * 200 / 1395;//200
    myROIShipsCoord.y = windowsizeGame.bottom * 160 / 784;//140
    myROIShipsCoord.width = windowsizeGame.right * 950 / 1395;//950
    myROIShipsCoord.height = windowsizeGame.bottom * 470 / 784;//490

    Rect shipBRegion;
    shipBRegion.x = windowsizeGame.right * 530 / 1290;
    shipBRegion.y = windowsizeGame.bottom * 555 / 725;
    shipBRegion.width = windowsizeGame.right * 115 / 1290;
    shipBRegion.height = windowsizeGame.bottom * 165 / 725;

    Mat ShipB;
    ShipStates ShipBState;//тут будем хранить текущее состояние корабля
    ShipBState = shSleep;

    while (key != 27)
    {
        MainSc.StartProcess();
        ShipB = MainSc.GetSrcMain()(shipBRegion);
        if (isImageFound(ShipB, ishipZZ)) //
            ShipBState = shSleep;
        else 
            if (isImageFound(ShipB, ishipAim))
                ShipBState = shAim;
            else
                if (isImageFound(ShipB, ishipAtFight))
                {
                    ShipBState = shFight;
                    currState = WaitEndFight;
                }
                else
                    if (isImageFound(ShipB, ishipGo))
                        ShipBState = shGo;
                    else
                        if (isImageFound(ShipB, ishipRepair))
                        {
                            ShipBState = shRepair;
                            currState = WaitRepair;
                        }
                        else
                            ShipBState = shSleep;
        switch (ShipBState)
        {
        case shSleep:
            printf("Ship sleep \n");
            break;
        case shGo:
            printf("Ship go \n");
            break;
        case shAim:
            printf("Ship aim \n");
            break;
        case shFight:
            printf("Ship fight \n");
            break;
        case shRepair:
            printf("Ship repair \n");
            break;
        default:
            break;
        }

        switch (currState)
        {
        case findShip:


            if (ShipBState != shSleep) break; //если корабль занят то не лезем к нему
            printf("Search ships... \n");
            MainSc.FindShip(image, myROIShipsCoord);//Находим все корабли
            if (MainSc.GetCount() < 1)
            {
                printf("nothing found...wait 0.5 sec \n");
                Sleep(500);
               // StateTime += 100;
                break;
            }
            printf("found %i target \n", MainSc.GetCount());
          //  for (int i = 0; i < MainSc.GetCount(); i++)
           //     circle(MainSc.GetSrcMain(), Point(MainSc.GetShipsList()[i].x + image.cols / 2, MainSc.GetShipsList()[i].y + image.rows / 2), image.cols / 2, Scalar(0, 0, 255), -1);
            x = 0, y = 0;
            ship attackedShip;
            if (MainSc.findImageCoord(imageShipB, x, y) > 0)//Если наш корабль на экране то ищем ближайший к нему и кликаем
            {
                printf("Our ship founded at x: %i, y: %i \n", x, y);
                attackedShip = MainSc.GetNearShip(x, y);
                printf("Find near ship. Founded at x: %i, y: %i \n", attackedShip.x, attackedShip.y);
            }
            else
            {
                attackedShip.x = MainSc.GetShipsList()[0].x;
                attackedShip.y = MainSc.GetShipsList()[0].y;
                printf("Our ship not found. Attack at x: %i, y: %i \n", attackedShip.x, attackedShip.y);
            }

          //  circle(MainSc.GetSrcMain(), Point(attackedShip.x + image.cols / 2, attackedShip.y + image.rows / 2), image.cols / 2, Scalar(0, 255, 0), -1);
            //к этому моменту у нас есть цель, кликаем её

            currState = ShipClick;

        case ShipClick:
            clck.x = attackedShip.x - 56;
            clck.y = attackedShip.y + 7;

            line(MainSc.GetSrcMain(), Point(clck.x - 10, clck.y), Point(clck.x + 10, clck.y), Scalar(0, 255, 255));
            line(MainSc.GetSrcMain(), Point(clck.x, clck.y - 10), Point(clck.x, clck.y + 10), Scalar(0, 255, 255));
       
            printf("Click at x: %i, y: %i \n", clck.x, clck.y);
            SendMessage(hwGame, WM_LBUTTONDOWN, 0, ((clck.y) << 16) | clck.x);

            Sleep(10);
            SendMessage(hwGame, WM_LBUTTONUP, 0, ((clck.y) << 16) | clck.x);
            
            currState = FindButtonFight;
            StateTime = 0;// считаем сколько ждём кнопку бой
            Sleep(500);
            break;
        case FindButtonFight: 
            printf("Search fight button.... \n");
            if (MainSc.findImageCoord(imageFight, x, y))
            {//если найдена кнопка "бой" то кликаем её
                printf("Button fight finded, click!\n");
                SendMessage(hwGame, WM_LBUTTONDOWN, 0, ((y) << 16) | (x));
                Sleep(10);
                SendMessage(hwGame, WM_LBUTTONUP, 0, ((y) << 16) | x);
                currState = WaitEndFight;
                Sleep(1000);
                StateTime =0;
                break;
            }
            else
            {
                printf("Button fight not fonded, wait 1 sec!\n");
                Sleep(2000);
                StateTime += 2000;
            }

            if (StateTime > 4000)
            {
                printf("Cant find fight button, return to search ship!\n");
                StateTime = 0;
                currState = findShip;

            }
            break;
        case ButtonFightClick:
            break;
        case WaitEndFight:
            if ((ShipBState != shSleep) || (StateTime<3000))
            {
                printf("Wait 2 sec, until battle stop! Ship state: %i \n", ShipBState);
                Sleep(2000);
                if (ShipBState==shAim)
                  StateTime = 0;
                else
                  StateTime += 2000;
            }
            else
                currState = findShip;
            break;
        case WaitRepair:
            if (ShipBState == shRepair)
            {
                printf("Ship need repair. Wait 5 sec \n");
                Sleep(5000);
            }
            else
                currState = findShip;
            break;
        default:
            Sleep(100);
            break;
        }

        //circle(MainSc.GetSrcMain(), Point(clck.x, clck.y), 5, Scalar(0, 255, 255), -1);

        //рисуем всё что мы увидели
        for (int i = 0; i < MainSc.GetCount(); i++)
            circle(MainSc.GetSrcMain(), Point(MainSc.GetShipsList()[i].x + image.cols / 2, MainSc.GetShipsList()[i].y + image.rows / 2), image.cols / 2, Scalar(0, 0, 255), -1);


        rectangle(MainSc.GetSrcMain(), myROIShipsCoord, Scalar(0, 255, 255), 2, 1);
        rectangle(MainSc.GetSrcMain(), shipBRegion, Scalar(0, 125, 55), 2, 1);

        
    
        key = waitKey(10); // you can change wait time

        if (key == 13)
        {//пробуем свайп
            printf("Try swipe \n");
            printf("Button down 300:300 \n");
            SendMessage(hwGame, WM_LBUTTONDOWN, 0, ((300) << 16) | (100));
            Sleep(100);
            
            for (int z = 0; z < 100; z+=1)
            {
                int res = SendMessage(hwGame, WM_MOUSEMOVE, MK_LBUTTON, ((300+z) << 16) | (100+z));

                printf("MOVE %i:300 \n",z+300);
                Sleep(5);
            }
            printf("Button up 600:300 \n");
            SendMessage(hwGame, WM_LBUTTONUP, 0, ((300) << 16) | 610);
            Sleep(1000);


        }
    MainSc.EndProcess();
    }
    ReleaseCapture();
    destroyAllWindows();

    waitKey(0); // Wait for a keystroke in the window
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
