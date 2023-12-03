#include "Frog.h"
#include <windows.h>
#include <crtdbg.h>
#include <shlobj.h>
#include "Screen.h"
#include "resource.h"
#include "../MainUpdate.h"

using namespace Webfoot;

/// Width of the viewport in pixels.
#define SCREEN_WIDTH 1024
/// Height of the viewport in pixels.
#define SCREEN_HEIGHT 768

/// Sleep for this many milliseconds per loop when the application window loses the focus.
#define BACKGROUND_SLEEP_DURATION 250

namespace Webfoot {

static FileManagerStdio fileManager;

/// Initialize memory
void ProjectMemoryInit();
/// Deinitialize memory
void ProjectMemoryDeinit();
/// Initialize the file system(s).
static void ProjectFileSystemInit();
/// Clean up the file system(s).
static void ProjectFileSystemDeinit();

} //namespace Webfoot {

//-----------------------------------------------------------------------------

long FAR PASCAL WndProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
   switch (wMsg)
   {
      case WM_CLOSE:
         //PostQuitMessage(0); // Doesn't work with ManyMouse active
         theMainUpdate->Exit();
         return 0;
   }
   long returnValue = 0;
   bool skipDefWindowProc = theMouseManager->MessageNotify(hWnd, wMsg, wParam, lParam, &returnValue);
   skipDefWindowProc = skipDefWindowProc || theKeyboardManager->MessageNotify(hWnd, wMsg, wParam, lParam, &returnValue);
   skipDefWindowProc = skipDefWindowProc || theScreen->MessageNotify(hWnd, wMsg, wParam, lParam, &returnValue);
   if(!skipDefWindowProc)
      returnValue = DefWindowProc(hWnd, wMsg, wParam, lParam);
   return returnValue;
}

//-----------------------------------------------------------------------------

void Webfoot::ProjectMemoryInit()
{
}

//-----------------------------------------------------------------------------

void Webfoot::ProjectMemoryDeinit()
{
   theMemory->LogDisable();
}

//-----------------------------------------------------------------------------

void Webfoot::ProjectFileSystemInit()
{
   // Initialize the main filesystem to use the working directory
   fileManager.Init();
   theFiles = &fileManager;
   
   // Load the constants.
   theMainUpdate->ConstsInit();
}

//-----------------------------------------------------------------------------

void Webfoot::ProjectFileSystemDeinit()
{
   theFiles = NULL;
}

//-----------------------------------------------------------------------------

/*
 * WinMain - initialization, message loop
 */
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow)
{
   int errorCode = 0;
   MSG msg;

#if PLATFORM_IS_WINDOWS
   #ifdef _DEBUG
      // With Visual C, this will print debug output on memory leaks in
      // in the regular non-Webfoot heap when the program closes.
      _CrtSetDbgFlag (_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF); // | _CRTDBG_CHECK_CRT_DF);
   #endif
#endif
   theClock->Init();

   // Initialize memory if it isn't already
   FrogMemoryInit();
   // Don't generally bother tracking any allocations made before this point.
   // If any were made, they were probably in the constructors of objects
   // that won't be destroyed until after main() returns.
   theMemory->LogEnable();

   theEvents->Init();

   // Initialize file system
   ProjectFileSystemInit();
   DebugInit();

   // Initialize Lua
   theScripts->Init();

   // Initialize screen
   ScreenParameters screenParameters;
   screenParameters.screenSize.x   = SCREEN_WIDTH;
   screenParameters.screenSize.y   = SCREEN_HEIGHT;
   screenParameters.windowTitle    = theConsts->Get(WINDOW_TITLE_KEY);
   screenParameters.iconResource   = MAKEINTRESOURCE(IDI_MAIN);
   screenParameters.cursorResource = NULL;//MAKEINTRESOURCE(IDC_MAIN);
   screenParameters.hInstance      = hInstance;
   screenParameters.wndProc        = WndProc;
   screenParameters.nCmdShow       = nCmdShow;

   theScreen->Init(&screenParameters);

   theMouseManager->Init();
   theKeyboardManager->Init();

   theBitmaps->Init();
   theTextures->Init();
   theImages->Init();
   theFonts->Init();

   theSounds->Init();

   theMainUpdate->Init();

   bool hadFocus = theScreen->OSFocusCheck();
   while(!theMainUpdate->ExitingCheck())
   {
      //Poll events without blocking unless our application is in the background
      bool hasFocus = theScreen->OSFocusCheck();
      while (PeekMessage (&msg,NULL,0,0,(hasFocus?PM_NOYIELD:0) | PM_REMOVE))
      {
         if (msg.message==WM_QUIT)
         {
            errorCode = ((int)msg.wParam);
            break;
         }
         TranslateMessage (&msg);
         DispatchMessage (&msg);
      }
      if(theMainUpdate->ExitingCheck())
      {
         break;
      }
      // Make sure that we're in control
      hasFocus = theScreen->OSFocusCheck();
      if(hasFocus && !hadFocus)
         theClock->LongLoopNotify();
      if(hasFocus)
         theClock->Update();
      theMouseManager->Update(hasFocus);
      theKeyboardManager->Update(hasFocus);
      if(hasFocus)
      {
         theScripts->Update(false);
         theEvents->Update();
         theMainUpdate->Update();
      }
      else
      {
         //main_sleep();
         Thread::Sleep(BACKGROUND_SLEEP_DURATION);
      }

      hadFocus = hasFocus;
   }

   // Clean up
   theMainUpdate->Deinit();
   theMainUpdate->ConstsDeinit();
   theSounds->Deinit();
   theKeyboardManager->Deinit();
   theMouseManager->Deinit();
   theScreen->Deinit();
   theFonts->Deinit();
   theImages->Deinit();
   theTextures->Deinit();
   theBitmaps->Deinit();
   theScripts->Deinit();
   theEvents->Deinit();
   DebugDeinit();
   ProjectFileSystemDeinit();
   FrogMemoryDeinit();
   
   return errorCode;
}

//-----------------------------------------------------------------------------
