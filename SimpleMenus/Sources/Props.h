#ifndef __PROPS_H__
#define __PROPS_H__

#include "Frog.h"
#include "MenuState.h"
#include <array>
#include <map>

#include <random>
#include <string>
#include <cstdio>

#include <vector>
#include <functional>
#include <chrono>
#include <cmath>

#define GUI_LAYER_NAME "MainGame"
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
	class Ship;
	//
	class Actor;
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
		//type: ship, roid, enemy, bullet, animation
		void Init(const char* file_name, float x, float y, float velocity_x, float velocity_y, float step,
				float acceleration=0, float friction=0);
		/// Clean up the prop
		void Deinit();
		/// Make any changes for the given frame.  'dt' is the amount of time that
		/// has passed since the last frame, in milliseconds.
		virtual void Update(unsigned int dt);
		/// Draw the prop.
		void Draw(unsigned int dt);
		
		//For collision
		void Move(float x, float y);
		void Teleport(float x, float y);
		std::vector<Prop*> IsCollision();

		//AABB collision
		bool AABB(Prop* other);
		//Radius
		bool Radius(Prop* other);

		//Wrap around to other side of the screen
		void OnBoundCollision();
		//
		bool OutBounds();

		void Rescale(float scale);
		Point2F position;
		Point2F br;
		Point2F tl;
		float step;
		float radius;
		bool onScreen; //COMs spawn off screen 
		/// Appearance of the prop.
		Sprite* sprite;
		const char* file_name;
		Actor* actor;//initialized as null and set later
		void Reset();//Reset for pool
		//Current velocity
		Point2F velocity;
		float acceleration;
		float friction;
		//Hitbox
		Point2F size;
	protected:
		/// Current position of the prop.
		
		
		
		

	};

	//Resource Efficency
	class Pool {
	public:
		Pool();
		static std::vector<Prop*> pool; //pool of objects ready to use
		static Prop* GrabProp(const char* file_name, float x, float y, float velocity_x, float velocity_y, float step,
			float acceleration = 0, float friction = 0);// If no objects in pool create object, else resuse from pool
		static void ReturnProp(Prop* prop); //Reset the object and return to pool
	};
	class Population{
		/*
		Actor asteroid population:
		Spawns asteroids that out of bounds that head to bounds
		If
		*/
	public:
		Population();
		void Init(int cap, std::function<void(Population*)> spawner, int spawn_rate);
		int pop;
		int cap;
		int spawn_rate;
		Pool* pool;
		std::function<void(Population*)> spawner;
		std::chrono::steady_clock::time_point startTime;
		void Update();//Populate if population below cap after 10s.
	};
	//In init, if pool, pop to use again

	//reset to use again
	//==============================================================================
	class Actor{
	public:
		Actor();
		static std::vector<Actor*> actors;
		virtual void Init(Prop* prop, int hp, const char* type);
		virtual void Update(unsigned int dt) = 0;
		const char* type;
		int hp;
		int max_hp;
		int points;
		bool alive;
		Prop* hitter;//Other object that hit it; helpful for hit_funs
		
		virtual void Reset();
		Prop* prop;
		unsigned int dt; //will do the legwork of a global variable till I can trust externs
	};
	class PC : public Actor{
	public:
		static Actor* player1;
		PC();
		void Init(Prop* prop, std::map<KeyCode, std::array<int, 2>> inputs, int hp, const char* type,
			std::function<void(Prop*)> gun); //gun == FireBullet
		void Update(unsigned int dt);//Actor and gun
		int score;
		std::chrono::steady_clock::time_point canShoot;//intervel between shots
		std::chrono::steady_clock::time_point hurtTime;//time inviciblitly frame
		bool isHurt;
		std::function<void(Prop*)> gun; 
		/*^Only exist to avoid circular dependency. 
		Player update takes  from action and it creates a bullet 
		*/
	protected:
		std::map<KeyCode, std::array<int, 2>> inputs;
	};
	//Used by anything not the player(asteroid, enemies, bullets, animation)
	class COM : public Actor{
	public:
		COM();
		void Init(Prop* prop, int hp, int score, const char* type);
		void Update(unsigned int dt);

		//add entries after object creation
		std::vector<std::function<void(Actor*)>> update_funs;//array of functions that runs every time it updates
		std::vector<std::function<void(Actor*)>> death_funs;//array of functions that runs when it dies
		std::vector<std::function<void(Actor*)>> hit_funs;//array of functions that runs every time it takes a hit
		//startTimes
		//Enemies
		std::chrono::steady_clock::time_point hunt;
		std::chrono::steady_clock::time_point patrol;
		std::chrono::steady_clock::time_point takeAim;
		std::chrono::steady_clock::time_point fire;
		std::chrono::steady_clock::time_point dodge;

		bool isHunt;
		bool isPatrol;
		bool isTakeAim;
		bool isDodge;
		//Shots/animations
		std::chrono::steady_clock::time_point live;//time till disappear
		Actor* gunner;

		//Roids
		float angle;
		float spin;
		
		//Animations
		float duration;

		//For pool
		void Reset();
		void WakeUp(); //Get ready action again! Hp = max_hp and timers reset

		Population* population;//For roid and enemy to decrement upon death
	protected:
	
	};
	class Scoreboard{
	public:
		Scoreboard();
		void Init(float x, float y);//For hp display
		void Deinit();//Undraw label
		void Update();//Grab player object from class player and updates hpShips and Score based on it
		LabelWidget* scoreLabel;
		std::vector<Prop*> hpShips;
		Point2F tl;

	protected:
	};
	//==============================================================================
}

void Thrust(Webfoot::Prop* prop, int direction);

#endif //#ifndef __PROPS_H__