/**/

#include "main.h"
#include "src\Logic\MainLogic.h"
#include "cocos2d.h"
#include <cmath>
#include <iostream>
#include <fstream>
USING_NS_CC;



int WINAPI _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // create the application instance
	
	UI::MainLogic::WriteLog("Before construction of MainLogic");
    UI::MainLogic mainLogic;
	UI::MainLogic::WriteLog( "MainLogic constructed\n");
	return Application::getInstance()->run();
}

