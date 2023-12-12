#include <Props.h>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace Webfoot;

Prop::Prop()
{
	// Initialize pointers to NULL for safety.
	sprite = NULL;
	props.push_back(this);
}
std::vector<Prop*> Prop::props;
//------------------------------------------------------------------------------

void Prop::Init(const char* file_name, float x, float y, float velocity_x, float velocity_y, float step,
	float acceleration, float friction)
{
	Point2F center;
	// Load the image of the ball.
	sprite = frog_new Sprite();
	sprite->Init("Sprites/Sprites", file_name);
	size = sprite->SizeGet(0);//width, height

	center = Point2F::Create(theScreen->SizeGet() / 2);
	if (x == NULL){
		x = center.x;
	}
	if (y == NULL){
		y = center.y;
	}
	// Start the ball in the middle of the screen.
	position = Point2F::Create(x, y);

	Rescale(1);
	
	//For inertia
	this->acceleration = acceleration;
	this->friction = friction;

	//Constant slide. If uses acceleration or friction this is iniatl velocity
	velocity.x = velocity_x;
	velocity.y = velocity_y;
	
	this->step = step;//if acceleration is used, this is inital velocity
	onScreen = FALSE;
	actor = nullptr;
	this->file_name = file_name;
}
void Prop::Rescale(float scale){
	sprite->ScaleSet(Point2F::Create(scale, scale));
	size = sprite->SizeGet(0);
	float w, h;
	w = size.x * scale / 2; //sprite->SizeGet(0) is too good for keeping track of scaled images >:(
	h = size.y * scale / 2;

	br = Point2F::Create(position.x + w, position.y + h);
	tl = Point2F::Create(position.x - w, position.y - h);

	radius = 0.5 * std::sqrt(std::pow(br.x - tl.x, 2) + std::pow(br.y - tl.y, 2));
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

void Prop::Teleport(float x, float y){
	position.x = x;
	position.y = y;
	size = sprite->SizeGet(0);
	float w;
	float h;
	w = size.x / 2;
	h = size.y / 2;
	br = Point2F::Create(x + w, y + h);
	tl = Point2F::Create(x - w, y - h);
}

std::vector<Prop*> Prop::IsCollision() {
	std::vector<Prop*> collidedProps;
	// Check all props for collision
	for (Prop* prop : props) {
		// Check if the current prop is equal to the provided obj and satisfies the condition
		//DebugPrintf("Radius: %s\n", this->Radius(prop) ? "true" : "false");
		//DebugPrintf("A Types: %s, %s\n", this->actor->type, prop->actor->type);
		if (prop->actor->alive && prop != this && this->Radius(prop)) {
			collidedProps.push_back(prop);  // Add the matching object to the vector
		}
	}

	return collidedProps;
}
bool Prop::OutBounds() {
	return (br.x < 0 || tl.x > SCREEN_WIDTH_DEFAULT ||
		br.y < 0 || tl.y > SCREEN_HEIGHT_DEFAULT);
}
//------------------------------------------------------------------------------

void Prop::Draw(unsigned int dt)
{
	// The center of the ball is in the center of the image, so use an offset.
	sprite->PositionSet(position);
	sprite->Draw();
}

void Prop::OnBoundCollision()
{
	//Wrap around
	if (position.x > SCREEN_WIDTH_DEFAULT) {
		Teleport(0, position.y);
	}
	else if (position.x < 0) {
		Teleport(SCREEN_WIDTH_DEFAULT, position.y);
	}

	if (position.y > SCREEN_HEIGHT_DEFAULT) {
		Teleport(position.x, 0);
	}
	else if (position.y < 0) {
		Teleport(position.x, SCREEN_HEIGHT_DEFAULT);
	}

}

bool Prop::Radius(Prop* other){
	// Calculate the distance between the centers of the two objects.
	auto pos_a = this->position;
	auto pos_b = other->position;
	float distance = std::sqrt(std::pow(pos_b.x - pos_a.x, 2) + std::pow(pos_b.y - pos_a.y, 2));

	// Check if the distance is less than the sum of the radii, indicating a collision.
	//DebugPrintf("Distance: %f\n", distance);
	//DebugPrintf("Sum: %f\m", this->radius + other->radius);
	return distance < this->radius + other->radius;
}

bool Prop::AABB(Prop* other){
	return (
		br.x > other->tl.x &&
		other->br.x > tl.x &&
		br.y > other->tl.y &&
		other->br.y > tl.y
		);
}

void Prop::Reset(){
	//Move off screen
	Teleport(-9999, -9999);
	onScreen = false;
	// Reset scale
	Rescale(1);
	Pool::ReturnProp(this);
}
//------------------------------------------------------------------------------
Pool::Pool() {};
std::vector<Prop*> Pool::pool;
Prop* Pool::GrabProp(const char* file_name, float x, float y, float velocity_x, float velocity_y, float step,
					float acceleration, float friction) {
	// Check if there's an object with the same file_name in the pool
	auto target = std::find_if(pool.begin(), pool.end(),
		[file_name](const Prop* prop) {
		return prop->file_name == file_name;
	});

	if (target == pool.end()) {
		// No object with the same file_name found, create a new one
		Prop* prop = new Prop();
		prop->Init(file_name, x, y, velocity_x, velocity_y, step, acceleration, friction);
		return prop;

	}
	else {
		// Return the object with the same file_name from the pool
		Prop* prop = *target;
		//Take new values
		prop->Teleport(x, y);
		prop->velocity.x = velocity_x;
		prop->velocity.y = velocity_y;
		prop->step = step;
		prop->acceleration = acceleration;
		prop->friction = friction;
		pool.erase(target); // Remove the object from the pool
		return prop;
	}
}


void Pool::ReturnProp(Prop* prop) {
	pool.push_back(prop);
}

//------------------------------------------------------------------------------


//
Actor::Actor() {
	actors.push_back(this);
}
std::vector<Actor*> Actor::actors;

void Actor::Init(Prop* prop, int hp, const char* type){
	this->prop = prop;
	this->hp = hp;
	this->max_hp = hp;
	alive = true;
	this->type = type;
}
void Actor::Update(unsigned int dt){
	this->dt = dt;
}
void Actor::Reset(){
	hp = max_hp;
	alive = false;
}
PC::PC() {};
Actor* PC::player1;
void PC::Init(Prop* prop, std::map<KeyCode, std::array<int, 2>> inputs, int hp, const char* type, 
	std::function<void(Prop*)> gun)
{
	Actor::Init(prop, hp, type);
	this->prop = prop;
	this->inputs = inputs;
	score = 0;
	canShoot = std::chrono::steady_clock::now();
	this->gun = gun;
}
void PC::Update(unsigned int dt)
{
	Actor::Update(dt);
	float x, y, step;
	int direction;
	step = prop->step;
	Point2F* velocity = &(prop->velocity);
	bool isThrusting = false;
	float rotation = prop->sprite->RotationGet();
	auto currentTime = std::chrono::steady_clock::now();
	for (const auto &p : inputs)
	{
		p.first; //key
		p.second;//value
		if (theKeyboard->KeyPressed(p.first)){
			switch (p.second[1]) {
			case 0://move
				/*
				x = p.second[0] * step;
				y = p.second[1] * step;
				prop->Move(x, y);
				*/
				direction = p.second[0];
				Thrust(prop, direction);
				break;
			case 1://rotate
				if (auto *sprite = dynamic_cast<Sprite *>(prop->sprite))
				{
					sprite->RotationSet(rotation + 5 * p.second[0]);
					rotation = prop->sprite->RotationGet();
				}
				break;
			case 2://Fire
				auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - canShoot).count();
				if (elapsedSeconds >= .01 || true) {  // Can only shoot again after .01 second
					canShoot = std::chrono::steady_clock::now();
					gun(prop);//Fire bullet
				}
				break;

			}
		}

	}
	//Friction effect
	if (!(isThrusting) || isHurt){//Slow down after hit
		velocity->x -= velocity->x * prop->friction;
		velocity->y -= velocity->y * prop->friction;
		}
	// Move the prop based on the calculated speed and direction
	float normalized = std::fmod(rotation, 360.0f);

	// Ensure the result is positive
	if (normalized < 0) {
		normalized += 360.0f;
	}

	// Convert degrees to radians
	float radians = (M_PI / 180.0) * normalized;

	// Calculate the speed (you might want to adjust this based on your needs)


	// Set velocity based on the direction
	x = velocity->x * cos(radians);
	y = velocity->y * -sin(radians);

	prop->Move(x, y);
	if (prop->OutBounds()){
		prop->OnBoundCollision();
	}
	// React to getting hit
	if (hitter){
		isHurt = true;
		hurtTime = std::chrono::steady_clock::now();
		hitter = nullptr;
	}
	if (isHurt){
		prop->sprite->Update(dt); //animate hurt
		auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - hurtTime).count();
		if (elapsedSeconds >= 2) {  // Can only shoot again after .05 second
			isHurt = false;
			prop->sprite->AnimationSet("Sprites/Sprites", prop->file_name);
		}
	}
}
	
	
COM::COM() {};

void COM::Reset(){
	update_funs.clear();
	death_funs.clear();
	hit_funs.clear();
}

void COM::WakeUp(){
	hp = max_hp;
	hunt = std::chrono::steady_clock::now();
	patrol = std::chrono::steady_clock::now();
	takeAim = std::chrono::steady_clock::now();
	fire = std::chrono::steady_clock::now();
	dodge = std::chrono::steady_clock::now();
	live = std::chrono::steady_clock::now();

	isHunt = false;
	isPatrol = false;
	isTakeAim = false;
	isDodge = false;

}
void COM::Init(Prop* prop, int hp, int points, const char* type)
{
	Actor::Init(prop, hp, type);
	this->points = points;
	WakeUp();
}
void COM::Update(unsigned int dt){
	Actor::Update(dt);
	//Run update functions
	for (const auto& func : update_funs) {
		func(this);
	}
	if (hitter){
		for (const auto& func : hit_funs) {
			func(this);
		}
	}
	if (hp <= 0 && alive){
		for (const auto& func : death_funs) {
			func(this);
		}
		prop->Reset();
		Actor::Reset();
		Reset();
	}
	hitter = nullptr;
}
//------------------------------------------------------------------------------
//Just adjust to player stats
Scoreboard::Scoreboard() {};
void Scoreboard::Init(float x, float y){
	char tempString[20];
	UTF8Snprintf(tempString, 20, "Score: %d", 0);
	scoreLabel = dynamic_cast<LabelWidget*>(theGUI->WidgetGetByPath(GUI_LAYER_NAME ".Score"));
	tl = Point2F::Create(x, y);
	std::vector<Prop*> hpShips;
	
}
void Scoreboard::Deinit(){
	if (scoreLabel){
		SmartDeinitDelete(scoreLabel);
	}
}
void Scoreboard::Update() {
	Actor* player1 = PC::player1;

	char tempString[20];
	UTF8Snprintf(tempString, 20, "SCORE: %d", player1->points);
	scoreLabel->TextSet(tempString);
	
	int hp = player1->hp;
	scoreLabel->TextBoundsGet();
	int length = hpShips.size();
	Prop* ship;
	float x, y;
	while (length != hp) { //Make hp display match current 
		if (length < hp){
			x = tl.x + length * 40;
			y = tl.y;
			ship = Pool::GrabProp("ship", x, y, 0, 0, 0);
			ship->Rescale(.7);
			ship->sprite->RotationSet(270);
			ship->actor = new COM();

			COM* comPtr = static_cast<COM*>(ship->actor);
			comPtr->Init(ship, 1, 0, "animation");
			hpShips.push_back(ship);
		}
		else {
			ship = hpShips.back();
			hpShips.pop_back();
			ship->actor->hp--;
		}
		length = hpShips.size();
	}

}
//------------------------------------------------------------------------------
void Thrust(Webfoot::Prop* prop, int direction) {
	Point2F* velocity = &(prop->velocity);
	// Set velocity based on the direction
	switch (direction) {
	case -1:
		velocity->x -= velocity->x * prop->friction + prop->step;
		velocity->y -= velocity->y * prop->friction + prop->step;
		break;
	case 1:
		velocity->x += prop->step * prop->acceleration;
		velocity->y += prop->step * prop->acceleration;
	}
	
}

