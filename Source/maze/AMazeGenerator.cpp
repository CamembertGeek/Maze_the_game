// Fill out your copyright notice in the Description page of Project Settings.


#include "AMazeGenerator.h"
#include <vector>
#include <cstdint>
#include <utility> // std::pair

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
	std::vector<uint8_t> BacktrackingMazeGenerator(int height, int width)
	{
		// Initialisation of the maze grid.
		std::vector<uint8_t> Grid(height * width, 1); // 1 = wall

		// Initialisation of the starting point (cell coordinates).
		std::pair<int, int> StartingPoint = { 1, 1 };

		// Convert (1,1) to 1D index and open it
		int startIndex = Indexation(StartingPoint.first, StartingPoint.second, width);
		Grid[startIndex] = 0; // 0 = path

		// Stocking of all the visited point
		std::vector<int> stack;
		stack.push_back(startIndex);

		while (!stack.empty()){
			// Initialization of the neighbors list
			std::vector<std::pair<int, int>> neighbors;

			int current = stack.back(); // Current cell

			int ci = current / width;
			int cj = current % width;


			// We test all direction to see if there is neighbors
			// UP
			int UPi = ci - 2;
			int UPj = cj;
			
			if (UPi >= 1 && UPi < height - 1) {
				if (UPj >= 1 && UPj < width - 1) {

					int idx = UPi * width + UPj;

					if (Grid[idx] == 1) {
						neighbors.push_back({ UPi, UPj });
					}
				}
			}

			// DOWN
			int DOWNi = ci + 2;
			int DOWNj = cj;

			if (DOWNi >= 1 && DOWNi < height - 1) {
				if (DOWNj >= 1 && DOWNj < width - 1) {
				
					int idx = DOWNi * width + DOWNj;

					if (Grid[idx] == 1) {
						neighbors.push_back({ DOWNi, DOWNj });
					}
				}
			}

			// LEFT
			int LEFTi = ci;
			int LEFTj = cj - 2;

			if (LEFTi >= 1 && LEFTi < height - 1) {
				if (LEFTj >= 1 && LEFTj < width - 1) {

					int idx = LEFTi * width + LEFTj;

					if (Grid[idx] == 1) {
						neighbors.push_back({ LEFTi, LEFTj });
					}
				}
			}

			// RIGHT
			int RIGHTi = ci;
			int RIGHTj = cj + 2;

			if (RIGHTi >= 1 && RIGHTi < height - 1) {
				if (RIGHTj >= 1 && RIGHTj < width - 1) {

					int idx = RIGHTi * width + RIGHTj;

					if (Grid[idx] == 1) {
						neighbors.push_back({ RIGHTi, RIGHTj });
					}
				}
			}
		
		}
			 

		return Grid;
	}

	// Convert (i, j) into 1D index
	int Indexation(int i, int j, int width)
		{
			return i * width + j;
		}

	// Add Loops inside of the maze.
	void AddLoops(){

	}
};

