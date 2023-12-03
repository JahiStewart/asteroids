#ifndef __MAINGAME_H__
#define __MAINGAME_H__

#include "Frog.h"
#include "MenuState.h"


//#include <string>
#include <vector>
#include <map>
#include <array>

#include "Props.h"

namespace Webfoot {

//==============================================================================

//extern Scoreboard* scoreboard;
class MainGame : public MenuState
{
public:
   typedef MenuState Inherited;

   MainGame();
   virtual ~MainGame() {};
   
   virtual void Init();
   virtual void Deinit();

   /// Call this on every frame to update the positions.
   virtual void Update();
   /// Call this on every frame to draw the images.
   virtual void Draw();

   static MainGame instance;

protected:
   /// Returns the name of the GUI layer
   virtual const char* GUILayerNameGet();

   /// The ball that bounces around the screen.
   Paddle* paddle1;
   Paddle* paddle2;
   Ball* ball;
   Paddle* paddles[2];//Curently doesn't work(values would become null in main update)

   PC* player1;
   COM* player2;
};

MainGame* const theMainGame = &MainGame::instance;

//==============================================================================
//Old props
//==============================================================================
 //namespace Webfoot {
};
#endif //#ifndef __MAINGAME_H__
