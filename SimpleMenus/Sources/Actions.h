#ifndef __ACTIONS_H__
#define __ACTIONS_H__

#include "Props.h"


void FireBullet(Webfoot::Prop* prop);//Here to avoid circular dependency

//Actions:

//If same type ignore; else exchange hp and tookHit = true
/*Special:
type enemy to type asteroid is safe
type enemy bullet to asteroid safe
type gunner to their type bullet safe
type enemy to type enemy bullet
anything type bomb not safe
anything type animation is safe
*/
void BasicCollison(Webfoot::Actor* actor);


//Roids
void Asteroid(Webfoot::Actor* actor);//move just spin
void BreakUp(Webfoot::Actor* actor);//break up to 

//Enemies
void Hunt(Webfoot::Actor* actor);//Go towards player if in range and facing them
void Patrol(Webfoot::Actor* actor);//Go in cirle
void TakeAim(Webfoot::Actor* actor);//Spin gun at player if in range
void Fire(Webfoot::Actor* actor);//Fire if in range
void Dodge(Webfoot::Actor* actor);//Move out of area if bullet 

//Shots
void Bullet(Webfoot::Actor* actor);//Move till time out or till hit an object


//Misc
void UpdateScore(Webfoot::Actor* actor);//hits updates score

//Animations
void Animation(Webfoot::Actor* actor);//Moves prop and checks if duration has passed
void Frames(Webfoot::Actor* actor);//Changes frames of prop 

bool DeathByPlayer(Webfoot::Actor* actor);//Check if death by player ship
void PointsPopup(Webfoot::Actor* actor);
void Explode(Webfoot::Actor* actor);

//Spawners for Population
void SpawnRoid(Webfoot::Population* pop);
void SpawnEnemy(Webfoot::Population* pop);

void DecreasePop(Webfoot::Actor* actor);//death_fun for enemies and asteriods
Webfoot::Prop* CreateAsteroid(const char* fileName, float x, float y, float velocity_x, float velocity_y, 
	float angle, float scale, float spin, int points);

Webfoot::Prop* CreateEnemy(const char* file_name, float x, float y, float velocity_x, float velocity_y,
	float angle, float step, int points,
	float acceleration, float friction);
//AddPoints (coming soon)
//Update funs(will get own file soon):


#endif //#ifndef __ACTION_H__