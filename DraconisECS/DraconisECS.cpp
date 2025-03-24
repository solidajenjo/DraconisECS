#define SDL_MAIN_HANDLED
#include <iostream>
#include "app.h"
#include "core/ecs/ecs.h"

struct Position
{
	float x;
	float y;
};

struct Velocity
{
	float x;
	float y;
	float z;
};

struct Sprite
{
	const char* name = "SPRITE";
};

int main()
{
	/*
	if( !app::appInstance.init() )
	{
			std::cerr << "Failed to initialize app\n";
			return 1;
	}

	// Main loop
	while( app::appInstance.update() )
	{
	}

	// Shutdown the app
	app::appInstance.shutdown();
	*/

	auto& ecs	= ecs::Ecs::getInstance();
	auto entity = ecs.createEntity( Position{ 10, 10 }, Velocity{ 1, 1 } );	
	std::cout << "Entity created" << std::endl << std::endl;
	auto entity2 = ecs.createEntity( Position{ 20, 20 }, Sprite{}, Velocity{ 2, 2 } );
	std::cout << "Entity2 created" << std::endl << std::endl;
	auto entity3 = ecs.createEntity( Position{ 30, 30 }, Velocity{ 3, 3 } );
	std::cout << "Entity3 created" << std::endl << std::endl;
	auto entity4 = ecs.createEntity( Velocity{ 4, 4 }, Position{ 40, 40 } );
	std::cout << "Entity4 created" << std::endl << std::endl;
	// auto pos	= ecs.getComponent<Position>( entity );
	// auto vel	= ecs.getComponent<Velocity>( entity );

	ecs.addComponent( entity, Sprite{} );

	// pos		 = ecs.getComponent<Position>( entity );
	// vel		 = ecs.getComponent<Velocity>( entity );
	// auto spr = ecs.getComponent<Sprite>( entity );
	/*
	auto entity2 = ecs.createEntity();
	ecs.addComponent<Position>( entity2, Position{ 20, 20 } );
	ecs.addComponent<Velocity>( entity2, Velocity{ 2, 2 } );

	auto& position = ecs.getComponent<Position>( entity );
	auto& velocity = ecs.getComponent<Velocity>( entity );

	std::cout << "Position: " << position.x << ", " << position.y << std::endl;
	std::cout << "Velocity: " << velocity.x << ", " << velocity.y << std::endl;

	auto& position2 = ecs.getComponent<Position>( entity2 );
	auto& velocity2 = ecs.getComponent<Velocity>( entity2 );

	std::cout << "Position2: " << position2.x << ", " << position2.y <<
	std::endl; std::cout << "Velocity2: " << velocity2.x << ", " << velocity2.y
	<< std::endl;
	*/
	return 0;
}
