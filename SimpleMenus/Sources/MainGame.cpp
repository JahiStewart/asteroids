#include "Frog.h"
#include "MainGame.h"
#include "MainUpdate.h"

#include <random>
#include <string>
#include <array>
#include <cstdio>

#include <Actions.h>

/*Tonight:
1:
hp
gun
asteroid breaking
enemies

2:
Make score board

3:
sound

4:
animations
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
	/*Second value is type(for now)
	0-thrust
	1-rotate
	2-fire
	
	*/
	{ KEY_UP, { { 1, 0} } }, //Accelerate
	{ KEY_DOWN, { { -1, 0} } },//Brake

	{ KEY_LEFT, { { 1, 1 } } }, //Rotate Left
	{ KEY_RIGHT, { { -1, 1 } } },//Rotate Right
	{ KEY_SPACE, { { 0, 2 } } } //Fire

};
MainGame MainGame::instance;

//==============================================================================

/// Main GUI

//-----------------------------------------------------------------------------
MainGame::MainGame()
{
}

//-----------------------------------------------------------------------------

void MainGame::Init()
{
   Inherited::Init();
   unsigned int dt = theClock->LoopDurationGet();


   // Create player object
   Prop* ship = Pool::GrabProp("ship", NULL, NULL, 0, 0, .4, .9, .05);
   ship->actor = new PC();
   PC* pcPtr = static_cast<PC*>(ship->actor);//To use derived attributes
   pcPtr->Init(ship, inputs, 6, "player", FireBullet);
   PC::player1 = ship->actor;

   //Create Population handlers
   roid_maker = new Population();
   roid_maker->Init(10, SpawnRoid, 7);

   enemy_maker = new Population();
   enemy_maker->Init(2, SpawnEnemy, 30);

   // Throw some starting asteroids
   for (int i = 0; i < 7; ++i) {
	   // Your code here
	   SpawnRoid(roid_maker);
   }
   // Make scoreboard
   scoreboard = new Scoreboard();
   scoreboard->Init(50, 70);
   
}

//-----------------------------------------------------------------------------

void MainGame::Deinit()
{
   // Deinitialize and delete the ball.
	for (Prop* prop : Prop::props) {
		if (prop){
			Actor* actor = prop->actor;
			frog_delete actor;
			actor = NULL;

			prop->Deinit();
			frog_delete prop;
			prop = NULL;
		}
	}
	frog_delete roid_maker;
	roid_maker = NULL;
	frog_delete enemy_maker;
	enemy_maker = NULL;

	scoreboard->Deinit();

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

  
   std::vector<Actor*> actorsCopy = Actor::actors;
   for (Actor* actor : actorsCopy) {
	   if (actor->alive) {
		   actor->Update(dt);
		   //actor->prop->sprite->Update(dt); WILL actually play animation :o
	   }
   }
   roid_maker->Update();
   enemy_maker->Update();
   scoreboard->Update();
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
   for (Prop* prop : Prop::props) {
	   if (prop){
		   prop->Draw(dt);
	   }
   }
   scoreboard->scoreLabel->Draw();
}

//==============================================================================
