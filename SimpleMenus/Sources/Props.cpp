#include "Frog.h"

#include <random>
#include <string>
#include <array>
#include <cstdio>
#include <map>

#include <Props.h>

using namespace Webfoot;

Prop::Prop()
{
	// Initialize pointers to NULL for safety.
	sprite = NULL;
	props.push_back(this);
}
std::vector<Prop*> Prop::props;
//------------------------------------------------------------------------------

void Prop::Init(const char* file_name, int x, int y, int velocity_x, int velocity_y, float step, unsigned int dt)
{
	Point2F center;
	float w, h;
	// Load the image of the ball.
	sprite = frog_new Sprite();
	sprite->Init("Sprites/Sprites", file_name);
	size = sprite->SizeGet(dt);//width, height

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
	if (sprite)
	{
		SmartDeinitDelete(sprite);
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

void Prop::Teleport(float x, float y, unsigned int dt){
	position.x = x;
	position.y = y;
	size = sprite->SizeGet(dt);
	float w;
	float h;
	w = size.x / 2;
	h = size.y / 2;
	br = Point2F::Create(x + w, y + h);
	tl = Point2F::Create(x - w, y - h);
}
BOOLEAN Prop::OutBounds() { return false; }
//------------------------------------------------------------------------------

void Prop::Draw(unsigned int dt)
{
	// The center of the ball is in the center of the image, so use an offset.
	sprite->PositionSet(position);
	sprite->Draw();
}


Ball::Ball() {
	sprite = NULL;

}
void Ball::Init(const char* file_name, int x, int y, int velocity_x, int velocity_y, float step, unsigned int dt,
	Paddle *paddles[2], Scoreboard* scoreboard)
{
	Prop::Init(file_name, x, y, velocity_x, velocity_y, step, dt);
	for (int i = 0; i < 2; ++i) {
		this->paddles[i] = paddles[i];
	}
	this->scoreboard = scoreboard;
}

//------------------------------------------------------------------------------
void Ball::Update(unsigned int dt)
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
	Bounce(dt);
	sprite->RotationSet(sprite->RotationGet() + 1);

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
void Ball::Bounce(unsigned int dt){
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
		scoreboard->Update(dt);
		Teleport(SCREEN_WIDTH_DEFAULT / 2, SCREEN_HEIGHT_DEFAULT / 2, dt);
		step = 2;
	}

};
//------------------------------------------------------------------------------
Paddle::Paddle()
{
	sprite = NULL;
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
void Scoreboard::Init(Paddle* paddle1, Paddle* paddle2, unsigned int dt)
{
	this->paddle1 = paddle1;
	this->paddle2 = paddle2;

	score1 = frog_new Prop();
	score1->Init("numbers/00", SCREEN_WIDTH_DEFAULT / 2 - 50, 50, 0, 0, 0, dt);
	score2 = frog_new Prop();
	score2->Init("numbers/00", SCREEN_WIDTH_DEFAULT / 2 + 50, 50, 0, 0, 0, dt);
}
void Scoreboard::Update(unsigned int dt){
	if (paddle1->score > 9 || paddle2->score > 9) {
		paddle1->score = 0;
		paddle2->score = 0;
	}
	UpdateScore(score1, paddle1, 1, dt);
	UpdateScore(score2, paddle2, -1, dt);
}
void Scoreboard::UpdateScore(Prop* &score, Paddle* paddle, int side, unsigned int dt){
	score->Teleport(-100, -100, dt);//Into the backrooms
	score = frog_new Prop();
	std::string str = "numbers/0" + std::to_string(paddle->score);
	const char* char_ptr = str.c_str();
	score->Init(char_ptr, SCREEN_WIDTH_DEFAULT / 2 + 50 * side, 50, 0, 0, 0, dt);
	score->Draw(dt);
}
