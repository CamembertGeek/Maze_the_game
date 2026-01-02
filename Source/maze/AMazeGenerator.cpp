// Fill out your copyright notice in the Description page of Project Settings.


#include "AMazeGenerator.h"
#include <vector>

// Sets default values
AAMazeGenerator::AAMazeGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAMazeGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAMazeGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}




// Class for generation of the maze by using backtraking technic.
class GenerateMaze {
private:
	int height;
	int width;

	// Generate a maze grid with Backtracking.
	void BacktrackingMazeGenerator(int height, int width) {

		// Initialisation of the maze grid
		std::vector<uint8_t> Grid(height * width, 1);  // 1 = wall

		
	}

	// Add Loops inside of the maze.
	void AddLoops(){

	}
};

