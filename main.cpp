/**/

#include "main.h"
#include "src\Logic\MainLogic.h"
#include "cocos2d.h"
#include <cmath>
USING_NS_CC;

int WINAPI _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // create the application instance
    UI::MainLogic mainLogic;

	return Application::getInstance()->run();
}

