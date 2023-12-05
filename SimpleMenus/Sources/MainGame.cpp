#include "Frog.h"
#include "MainGame.h"
#include "MainUpdate.h"

#include <random>
#include <string>
#include <array>
#include <cstdio>

#include "Props.h"

/*Next Objective:
Make score board
Fix step collision to avoid "over stepping"
*/
using namespace Webfoot;
Scoreboard* scoreboard = nullptr;
/*
std::map<KeyCode, int[2]> inputs2 = {
{ KEY_RIGHT, { 1, 0 } },
{ KEY_LEFT, { -1, 0 } },
{ KEY_UP, { 0, -1 } },
{ KEY_DOWN, { 0, 1 } },
};
/*/
// Create and initialize the ball.
std::map<KeyCode, std::array<int, 2>> inputs = {
	{ KEY_RIGHT, { { 1, 0 } } },
	{ KEY_LEFT, { { -1, 0 } } },
	{ KEY_UP, { { 0, -1 } } },
	{ KEY_DOWN, { { 0, 1 } } }
};
MainGame MainGame::instance;

//==============================================================================

/// Main GUI
#define GUI_LAYER_NAME "MainGame"

//-----------------------------------------------------------------------------
MainGame::MainGame()
{
   ball = NULL;
   paddle1 = NULL;
   paddle2 = NULL;
   paddles[2] = NULL;
   player1 = NULL;
   player2 = NULL;
}

//-----------------------------------------------------------------------------

void MainGame::Init()
{
   Inherited::Init();
   unsigned int dt = theClock->LoopDurationGet();

   paddle1 = frog_new Paddle();
   paddle1->Init("paddle", 50, NULL, 0, 0, 8, dt);
   paddle2 = frog_new Paddle();
   paddle2->Init("paddle", 1000, NULL, 0, 0, 8, dt);
   Paddle* paddles[2] = {paddle1, paddle2};

   scoreboard = frog_new Scoreboard();
   scoreboard->Init(paddle1, paddle2, dt);

   ball = frog_new Ball();
   ball->Init("clock", NULL, NULL, 1, -1, 8, dt, paddles, scoreboard);

   player1 = frog_new PC();
   player1->Init(paddle1, inputs); 

   player2 = frog_new COM();
   player2->Init(paddle2, ball);

   
}

//-----------------------------------------------------------------------------

void MainGame::Deinit()
{
   // Deinitialize and delete the ball.
	for (Prop* prop : Prop::props) {
		if (prop){
			prop->Deinit();
			frog_delete prop;
			prop = NULL;
		}
	}
	frog_delete player1;
	player1 = NULL;
	frog_delete player2;
	player2 = NULL;
	frog_delete scoreboard;
	scoreboard = NULL;
}

//-----------------------------------------------------------------------------

const char* MainGame::GUILayerNameGet()
{
   return GUI_LAYER_NAME;
}

//-----------------------------------------------------------------------------

void MainGame::Update()
{
   Inherited::Update();

   unsigned int dt = theClock->LoopDurationGet();

   Paddle* paddles[2] = { paddle1, paddle2};

   ball->Update(dt);
   player1->Update(dt);
   player2->Update(dt);

   // Return to the previous menu if the escape key is pressed.
   if(!theStates->StateChangeCheck() && theKeyboard->KeyJustPressed(KEY_ESCAPE))
   {
      theMainGame->StateChangeTransitionBegin(true);
      theStates->Pop();
   }
}

//-----------------------------------------------------------------------------

void MainGame::Draw()
{
   unsigned int dt = theClock->LoopDurationGet();
   ball->Draw(dt);
   paddle1->Draw(dt);
   paddle2->Draw(dt);
   scoreboard->score1->Draw(dt);
   scoreboard->score2->Draw(dt);
}

//==============================================================================
