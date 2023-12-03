#include "Frog.h"
#include "MainGame.h"
#include "MainUpdate.h"

#include <random>
#include <string>
#include <array>
#include <cstdio>

/*Next Objective:
Make score board
Fix step collision to avoid "over stepping"
*/
using namespace Webfoot;

MainGame MainGame::instance;

//==============================================================================

/// Main GUI
#define GUI_LAYER_NAME "MainGame"

//-----------------------------------------------------------------------------
Scoreboard* scoreboard;
MainGame::MainGame()
{
   ball = NULL;
   paddle1 = NULL;
   paddle2 = NULL;
   paddles[2] = NULL;
   player1 = NULL;
   player2 = NULL;
   scoreboard = NULL;
}

//-----------------------------------------------------------------------------

void MainGame::Init()
{
   Inherited::Init();
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
   ball = frog_new Ball();
   ball->Init("ball2", NULL, NULL, 1, -1, 8);
   paddle1 = frog_new Paddle();
   paddle1->Init("paddle", 50, NULL, 0, 0, 8);
   paddle2 = frog_new Paddle();
   paddle2->Init("paddle", 1000, NULL, 0, 0, 8);
   Paddle* paddles[2] = {paddle1, paddle2};

   player1 = frog_new PC();
   player1->Init(paddle1, inputs); 

   player2 = frog_new COM();
   player2->Init(paddle2, ball);

   scoreboard = frog_new Scoreboard();
   scoreboard->Init(paddle1, paddle2);
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

   ball->Update(dt, paddles);
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
   ball->Draw();
   paddle1->Draw();
   paddle2->Draw();
   scoreboard->score1->Draw();
   scoreboard->score2->Draw();
}

//==============================================================================

Prop::Prop()
{
   // Initialize pointers to NULL for safety.
   image = NULL;
   props.push_back(this);
}
std::vector<Prop*> Prop::props;
//------------------------------------------------------------------------------

void Prop::Init(const char* image_file, int x, int y, int velocity_x, int velocity_y, float step)
{
   Point2F center;
   float w, h;
   // Load the image of the ball.
   image = theImages->Load(image_file);//!
   size = image->SizeGet();//width, height

   center = Point2F::Create(theScreen->SizeGet() / 2);
   if (x == NULL){
	   x = center.x;
   }
   if (y == NULL){
	   y = center.y;
   }
   // Start the ball in the middle of the screen.
   position = Point2F::Create(x, y);
   
   w = size.x / 2;
   h = size.y / 2;
   br = Point2F::Create(x + w, y + h);
   tl = Point2F::Create(x - w, y - h);
   velocity.x = velocity_x;
   velocity.y = velocity_y;
   this->step = step;
}

//------------------------------------------------------------------------------

void Prop::Deinit()
{
   // Unload the image of the ball.
   if(image)
   {
      theImages->Unload(image);
      image = NULL;
   }
}

//------------------------------------------------------------------------------

void Prop::Update(unsigned int dt)
{
	//need mouse object
	//need window object
	//random values
	//Move(step * velocity.x, step * velocity.y);
	//Bounce();
	
}
void Prop::Move(float x, float y){
	position.x += x;
	position.y += y;
	br.x += x;
	br.y += y;
	tl.x += x;
	tl.y += y;
}

void Prop::Teleport(float x, float y){
	position.x = x;
	position.y = y;
	size = image->SizeGet();
	float w;
	float h;
	w = size.x / 2;
	h = size.y / 2;
	br = Point2F::Create(x + w, y + h);
	tl = Point2F::Create(x - w, y - h);
}
BOOLEAN Prop::OutBounds() { return false; }
//------------------------------------------------------------------------------

void Prop::Draw()
{
   // The center of the ball is in the center of the image, so use an offset.
   image->Draw(position - (Point2F::Create(image->SizeGet()) / 2.0f));
}


Ball::Ball() {
	image = NULL;

}
//------------------------------------------------------------------------------
void Ball::Update(unsigned int dt, Paddle *paddles[2])
{
	//need mouse object
	//need window object
	//random values
	Point2F old;
	BOOLEAN x_state, y_state;
	float x, y;

	old = Point2F::Create(position.x, position.y);
	x = step * velocity.x;
	y = step * velocity.y;

	Move(x, 0);
	x_state = IsCollision(paddles);
	Move(-x, 0);

	Move(0, y);
	y_state = IsCollision(paddles);
	Move(0, -y);
	//Final Move
	if (x_state) {
		x *= -1;
		velocity.x *= -1;
		step = 8;
	}
	if (y_state) {
		y *= -1;
		velocity.y *= -1;
		step = 8;
	}
	Move(x, y);
	Bounce(paddles);
};
//hit paddle
boolean Ball::HitPaddle(Paddle* paddle)
{
	return (
		br.x > paddle->tl.x &&
		paddle->br.x > tl.x &&
		br.y > paddle->tl.y &&
		paddle->br.y > tl.y
		);
	
}
boolean Ball::IsCollision(Paddle *paddles[2])
{
	for (int i = 0; i < 2; ++i) {

		Paddle* paddle = paddles[i];
		if (paddle != NULL && HitPaddle(paddle)){
			return true;
		}
	}
	return false;
}
void Ball::Bounce(Paddle *paddles[2]){
	if (br.y >= SCREEN_HEIGHT_DEFAULT) {
		velocity.y = -1;
	}
	else if (tl.y <= 0) {
		velocity.y = 1;
	}
	if (br.x >= SCREEN_WIDTH_DEFAULT || tl.x <= 0){
		if (br.x >= SCREEN_WIDTH_DEFAULT) {
			velocity.x = -1;
			paddles[1]->score++;
		}
		else if (tl.x <= 0) {
			velocity.x = 1;
			paddles[0]->score++;
		}
		scoreboard->Update();
		Teleport(SCREEN_WIDTH_DEFAULT / 2, SCREEN_HEIGHT_DEFAULT / 2);
		step = 2;
	}

};
//------------------------------------------------------------------------------
Paddle::Paddle()
{
	image = NULL;
	score = 0;
}
boolean Paddle::OutBounds()
{
	return (
		br.x >= SCREEN_WIDTH_DEFAULT ||
		tl.x <= 0 ||
		br.y >= SCREEN_HEIGHT_DEFAULT ||
		tl.y <= 0
		);
}
//------------------------------------------------------------------------------
Controls::Controls() {};
PC::PC() {};
void PC::Init(Prop* actor, std::map<KeyCode, std::array<int, 2>> inputs)
{
	this->actor = actor;
	this->inputs = inputs;

}
void PC::Update(unsigned int dt)
{
	float x, y, step;
	step = actor->step;
	for (const auto &p : inputs)
	{
		p.first; //key
		p.second;//value
		if (theKeyboard->KeyPressed(p.first)){
			x = p.second[0] * step;
			y = p.second[1] * step;
			actor->Move(x, y);
			if (actor->OutBounds()){ actor->Move(-x, -y); }
		}
	}

}
COM::COM() {};
void COM::Init(Prop* actor, Ball* ball)
{
	this->actor = actor;
	this->ball = ball;
	this->prev = 0;

}
void COM::Update(unsigned int dt)
{
	float y, step;
	y = ball->position.y;
	step = actor->step;
	if (y > actor->position.y) {
		y = 1;
	}
	else if (y < actor->position.y){
		y = -1;
	}
	else {
		y = 0;
	}
	this->prev = y;
	y = step * y;
	//Chance of taking random move
	std::random_device seed;
	std::mt19937 gen{ seed() }; // seed the generator
	std::uniform_int_distribution<> dist(1, 100);
	if (dist(gen) < 25) {//0 means will always chose right decision for testing purposes
		actor->Move(0, prev);
	}
	else {
		actor->Move(0, y);
	}
	if (actor->OutBounds()){ actor->Move(0, -y); }
	
	
}
//------------------------------------------------------------------------------
Scoreboard::Scoreboard(){}
void Scoreboard::Init(Paddle* paddle1, Paddle* paddle2)
{
	this->paddle1 = paddle1;
	this->paddle2 = paddle2;

	score1 = frog_new Prop(); 
	score1->Init("numbers/00", SCREEN_WIDTH_DEFAULT / 2 - 50, 50, 0, 0, 0);
	score2 = frog_new Prop();
	score2->Init("numbers/00", SCREEN_WIDTH_DEFAULT / 2 + 50, 50, 0, 0, 0);
}
void Scoreboard::Update(){
	if (paddle1->score > 9 || paddle2->score > 9) {
		paddle1->score = 0;
		paddle2->score = 0;
	}
	UpdateScore(score1, paddle1, 1);
	UpdateScore(score2, paddle2, -1);
}
void Scoreboard::UpdateScore(Prop* &score, Paddle* paddle, int side){
	score->Teleport(-100, -100);//Into the backrooms
	score = frog_new Prop();
	std::string str = "numbers/0" + std::to_string(paddle->score);
	const char* char_ptr = str.c_str();
	score->Init(char_ptr, SCREEN_WIDTH_DEFAULT / 2 + 50 * side, 50, 0, 0, 0);
	score->Draw();
}
