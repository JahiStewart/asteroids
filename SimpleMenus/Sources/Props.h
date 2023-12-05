#ifndef __PROPS_H__
#define __PROPS_H__

#include "Frog.h"
#include "MenuState.h"
#include <array>
#include <map>

#include <random>
#include <string>
#include <cstdio>


/*
std::map<KeyCode, int[2]> inputs2 = {
{ KEY_RIGHT, { 1, 0 } },
{ KEY_LEFT, { -1, 0 } },
{ KEY_UP, { 0, -1 } },
{ KEY_DOWN, { 0, 1 } },
};
/*/
// Create and initialize the ball.

namespace Webfoot {

	class Prop;
	class Ball;
	class Paddle;
	//
	class Controls;
	class PC;
	class COM;
	//
	class Scoreboard;
	//Base Class
	class Prop {
	public:
		Prop();
		static std::vector<Prop*> props;
		/// Initialize the prop
		void Init(const char* file_name, int x, int y, int velocity_x, int velocity_y, float step, unsigned int dt);
		/// Clean up the prop
		void Deinit();
		/// Make any changes for the given frame.  'dt' is the amount of time that
		/// has passed since the last frame, in milliseconds.
		virtual void Update(unsigned int dt);
		/// Draw the prop.
		void Draw(unsigned int dt);

		//For collision
		void Move(float x, float y);
		void Teleport(float x, float y, unsigned int dt);
		//
		virtual BOOLEAN OutBounds();
		Point2F position;
		Point2F br;
		Point2F tl;
		float step;
		/// Appearance of the prop.
		Sprite* sprite;
	protected:
		/// Current position of the prop.
		//Current velocity
		Point2F velocity;
		//Hitbox
		Point2F size;


	};
	/// A bouncing ball
	class Ball : public Prop{
	public:
		Ball();
		static std::vector<Ball*> balls;
		void Init(const char* file_name, int x, int y, int velocity_x, int velocity_y, float step, unsigned int dt);
		/// Bounce the ball
		void Bounce(unsigned int dt);
		void Update(unsigned int dt);
		Scoreboard* scoreboard;
	};
	class Paddle : public Prop{
	public:
		Paddle();
		BOOLEAN OutBounds();
		int score;
	protected:

	};
	//==============================================================================
	class Controls{
	public:
		Controls();
		virtual void Init() {};
		virtual void Update(unsigned int dt) = 0;
	protected:
		Prop* actor;
	};
	class PC : public Controls{
	public:
		PC();
		void Init(Prop* actor, std::map<KeyCode, std::array<int, 3>> inputs);
		void Update(unsigned int dt);
	protected:
		std::map<KeyCode, std::array<int, 3>> inputs;
	};
	class COM : public Controls{
	public:
		COM();
		void Init(Prop* actor, Ball* ball);
		void Update(unsigned int dt);
	protected:
		Ball* ball;
		float prev;
	};
	//==============================================================================
	class Scoreboard{
	public:
		Scoreboard();
		void Init(Paddle* paddle1, Paddle* paddle2, unsigned int dt);
		void Update(unsigned int dt);
		void UpdateScore(Prop* &score, Paddle* paddle, int side, unsigned int dt);
		Paddle* paddle1;
		Paddle* paddle2;
		Prop *score1;
		Prop *score2;

	};
};
#endif //#ifndef __PROPS_H__