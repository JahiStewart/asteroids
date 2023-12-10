#include <Actions.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace Webfoot;
/*
Actions
*/
void FireBullet(Prop* prop) {
	// Calculate the position of the bullet relative to the ship

	// Use fmod to get the remainder after division
	float rotation = prop->sprite->RotationGet();
	float normalized = std::fmod(rotation, 360.0f);

	// Ensure the result is positive
	if (normalized < 0) {
		normalized += 360.0f;
	}

	// Convert degrees to radians
	float radians = (M_PI / 180.0) * normalized;
	float radius = prop->radius;
	float x = prop->position.x + (radius * cos(radians));
	float y = prop->position.y - (radius * sin(radians));//Works perfertly for some reason?

	// Calculate the velocity of the bullet
	float speed = 5.0f;
	float velocity_x = speed * cos(radians);
	float velocity_y = -speed * sin(radians);
	// Create a bullet object and initialize it with the calculated position and velocity
	Prop* bullet = Pool::GrabProp("bullet", x, y, velocity_x, velocity_y, 1);
	bullet->actor = new COM();
	COM* comPtr = static_cast<COM*>(bullet->actor);//To use derived attributes
	comPtr->Init(bullet, 1, 0, "bullet");
	comPtr->gunner = prop->actor;
	comPtr->duration = 9;
	//Add functions
	comPtr->update_funs.push_back(Bullet);
	comPtr->update_funs.push_back(BasicCollison);
	//Make obj ready
	comPtr->WakeUp();
	//Rotate sprite
	bullet->sprite->RotationSet(rotation);
}

void BasicCollison(Actor* actor) {
	Prop* prop = actor->prop;
	COM* comPtr = static_cast<COM*>(actor);
	for (Prop* prop2 : prop->IsCollision()) {
		if (actor->hp <= 0){
			return;
		}
		Actor* actor2 = prop2->actor;
		COM* comPtr2 = static_cast<COM*>(actor2);
		if (!(actor->type == actor2->type ||
			(actor->type == "bullet" && comPtr->gunner == comPtr2 || actor2->type == "bullet" && comPtr2->gunner == comPtr) ||
			(actor->type == "animation" || actor2->type == "animation") ||
			((comPtr2 == PC::player1 && static_cast<PC*>(actor2)->isHurt) || (comPtr == PC::player1 && static_cast<PC*>(actor)->isHurt)
			)
			)){
			/*
			DebugPrintf("A Position: %f, %f\n", prop->position.x, prop->position.y);
			DebugPrintf("A Type: %s\n", comPtr->type);
			DebugPrintf("B Position: %f, %f\n", prop2->position.x, prop2->position.y);
			DebugPrintf("A Type: %s\n", comPtr2->type);
			*/
			actor->hp--;
			actor2->hp--;
			actor->hp = std::max(actor->hp, 0);
			actor2->hp = std::max(actor2->hp, 0);

			actor->hitter = prop2;
			actor2->hitter = prop;
		}
	}
}
// Roids
void Asteroid(Actor* actor) {
	// Implementation for moving asteroids
	COM* comPtr = static_cast<COM*>(actor);
	Prop* prop = actor->prop;
	float radians = comPtr->angle * M_PI / 180.0;
	float x = prop->velocity.x * std::cos(radians) * prop->step;
	float y = prop->velocity.y * std::sin(radians) * prop->step;
	prop->Move(x, y);
	if (prop->OutBounds() && prop->onScreen){
		prop->OnBoundCollision();
	}
	else if (!(prop->OutBounds() && prop->onScreen)){//Wrap around effect begins once entered screen
		prop->onScreen = true;
	}
	prop->sprite->RotationSet(prop->sprite->RotationGet() + 2 * comPtr->spin);

}

void BreakUp(Actor* actor) {
	// Implementation for breaking up asteroids
	COM* comPtr = static_cast<COM*>(actor);
	Prop* prop = actor->prop;
	const char* file_name = prop->file_name;
	float x = prop->position.x;
	float y = prop->position.y;
	float velocity_x = prop->velocity.x * .9;
	float velocity_y = prop->velocity.y * .9;
	float scale = prop->sprite->ScaleGet().x*.5;
	float spin = comPtr->spin;
	if (prop->radius > 10) {
		for (int i = 0; i < 2; ++i) {
			float angle = static_cast<float>(rand() % 360);
			int points;
			switch (comPtr->points){
			case 100:
				points = 50;
				break;
			case 50:
				points = 10;
				break;
			default:
				points = 1;
			}
			Prop* roid = CreateAsteroid(file_name, x, y, velocity_x, velocity_y, angle, scale, spin, points);
			COM* comPtr2 = static_cast<COM*>(roid->actor);//To use derived attributes
			roid->onScreen = true;
		}
	}
}

// Enemies
void Hunt(Actor* actor) {
	// Implementation for hunting enemies
}

void Patrol(Actor* actor) {
	// Implementation for patrolling enemies
}

void TakeAim(Actor* actor) {
	// Implementation for aiming enemies
}

void Fire(Actor* actor) {
	// Implementation for firing enemies
}

void Dodge(Actor* actor) {
	// Implementation for dodging enemies
}

// Shots
void Bullet(Actor* actor) {
	// Implementation for moving bullets
	//Bullets wrap around, animations don't
	Animation(actor);
	Prop* prop = actor->prop;
	if (prop->OutBounds()){
		prop->OnBoundCollision();
	}
}
void DecreasePop(Actor* actor){
	COM* comPtr = static_cast<COM*>(actor);
	if (comPtr->population){//means roid was created by population handler so it should decrement population
		comPtr->population->pop--;
	}
}
//Animations--------------------------------------------------------------------
void PointsPopup(Actor* actor) {
	Prop* hitter = actor->hitter;
	COM* hitterPtr = static_cast<COM*>(hitter->actor);
	if (hitterPtr == nullptr || hitterPtr->gunner == nullptr){//not bullet
		return;
	}
	PC* gunnerPtr = static_cast<PC*>(hitterPtr->gunner);
	if (actor->hp > 0 || gunnerPtr->prop != PC::player1->prop){ //only
		return;
	}
	float scale;
	float duration;
	float velocity_y;
	int points = actor->points;
	switch (points){
	case 500:
		scale = 4;
		duration = 3;
		velocity_y = .5;
	case 100:
		scale = 2;
		duration = 2;
		velocity_y = -.5;
	default:
		scale = 1.2;
		duration = 2;
		velocity_y = -.5;
	}
	float x, y;
	x = actor->prop->position.x;
	y = actor->prop->position.y;
	std::string temp = std::to_string(points) + "pts";
	const char* file_name = _strdup(temp.c_str());
	Prop* popup = Pool::GrabProp(file_name, x, y, 0, velocity_y, 1, .1);
	popup->Rescale(scale);
	popup->actor = new COM();

	COM* comPtr = static_cast<COM*>(popup->actor);
	comPtr->Init(popup, 1, 0, "animation");
	comPtr->duration = duration;
	comPtr->update_funs.push_back(Animation);
	comPtr->update_funs.push_back(Frames);

	gunnerPtr->points += points;
}
//--------------------------------------------------
void SpawnRoid(Population* population){
	// Randomly determine initial position outside bounds
	float x, y;

	// Randomly determine whether to spawn on the left/right or top/bottom
	if (rand() % 2 == 0) {
		// Spawn on the left or right side
		x = (rand() % 2 == 0) ? -50.0f : SCREEN_WIDTH_DEFAULT + 50.0f;
		y = static_cast<float>(rand() % SCREEN_HEIGHT_DEFAULT);
	}
	else {
		// Spawn on the top or bottom
		x = static_cast<float>(rand() % SCREEN_WIDTH_DEFAULT);
		y = (rand() % 2 == 0) ? -50.0f : SCREEN_HEIGHT_DEFAULT + 50.0f;
	}

	// Randomly determine initial velocity and angle
	float velocity = static_cast<float>(rand() % 100 + 1) * .0125;  // Adjust the range as needed
	float angle = static_cast<float>(rand() % 360);

	// Return a new asteroid
	//random choose between 1-5 debris
	std::string temp = "debris" + std::to_string(rand() % 5 + 1);
	const char* file_name = _strdup(temp.c_str());// Allocates memory and copies the string

	int randomValue = rand() % 2;
	float spin = spin = (randomValue == 0) ? -.125 : .125;

	int points = 100;
	Prop* roid = CreateAsteroid(file_name, x, y, velocity, velocity, angle, 2.3, spin, points);
	COM* comPtr = static_cast<COM*>(roid->actor);//To use derived attributes
	comPtr->population = population;
	population->pop++;
}
void SpawnEnemy(Population* population){

}
Prop* CreateAsteroid(const char* file_name, float x, float y, float velocity_x, float velocity_y,
	float angle, float scale, float spin, int points) {
	//DebugPrintf("FILE: %s\n", fileName);
	Prop* roid = Pool::GrabProp("debris1", x, y, velocity_x, velocity_y, 1);
	roid->Rescale(scale);

	roid->actor = new COM();
	COM* comPtr = static_cast<COM*>(roid->actor);

	// To use derived attributes
	comPtr->Init(roid, 1, 100, "roid");
	comPtr->angle = angle;
	comPtr->spin = spin;
	comPtr->points = points;

	// Add functions
	comPtr->update_funs.push_back(Asteroid);
	comPtr->update_funs.push_back(BasicCollison);
	comPtr->death_funs.push_back(DecreasePop);
	comPtr->death_funs.push_back(BreakUp);
	comPtr->death_funs.push_back(PointsPopup);

	// Make obj ready
	comPtr->WakeUp();

	return roid;
}

void Animation(Actor* actor) {
	Prop* prop = actor->prop;

	float x = prop->step * prop->velocity.x;
	float y = prop->step * prop->velocity.y;

	prop->Move(x, y);
	/*
	prop->velocity.x += prop->acceleration;
	prop->velocity.y += prop->acceleration;
	*/

	COM* comPtr = static_cast<COM*>(actor);
	auto currentTime = std::chrono::steady_clock::now();
	auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - comPtr->live).count();
	if (elapsedSeconds >= comPtr->duration) {
		actor->hp--;
	}
}
void Frames(Actor* actor){
	actor->prop->sprite->Update(actor->dt);
}
//------------------------------------------------------------------------------
Population::Population() {};
void Population::Init(int cap, std::function<void(Population*)> spawner) {
	this->cap = cap;
	this->spawner = spawner;
}
void Population::Update(){
	auto currentTime = std::chrono::steady_clock::now();
	auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();

	if (elapsedSeconds >= 4 && pop < cap){
		spawner(this);
		startTime = std::chrono::steady_clock::now();
	}
}