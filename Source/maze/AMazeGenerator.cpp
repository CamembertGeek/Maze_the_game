// Fill out your copyright notice in the Description page of Project Settings.

#include "AMazeGenerator.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Math/TransformNonVectorized.h" // FTransform
#include "Math/Vector.h"                // FVector
#include "Logging/LogMacros.h"          // UE_LOG

#include <vector>    // std::vector
#include <cstdint>   // uint8_t
#include <utility>   // std::pair
#include <cstdlib>   // rand(), srand()
#include <ctime>     // std::time() For the seed in rand()





// Class for generation of the maze by using backtraking technic.
class GenerateMaze {
public:

	// Generate a maze grid with Backtracking.
	std::vector<uint8_t> BacktrackingMazeGenerator(int height = 100, int width = 101)
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

		while (!stack.empty()) {
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


			// We select a random neighbor in the list and we "dig" to it if the neighbor list is not empty
			if (!neighbors.empty()) {

				int NbNeighbor = neighbors.size();

				int x = rand() % NbNeighbor; // We select a random neighbor

				auto neighbor = neighbors[x];
				int ni = neighbor.first;
				int nj = neighbor.second;

				int neighborIdx = Indexation(ni, nj, width);

				Grid[neighborIdx] = 0; // We dig the neighbor cell

				int mi = (ni + ci) / 2;
				int mj = (nj + cj) / 2;

				int wallIdx = Indexation(mi, mj, width);

				Grid[wallIdx] = 0; // we dig the intermediate cell


				stack.push_back(neighborIdx);
			}

			else {
				stack.pop_back();
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
	void AddLoops() {

	}
};




AAMazeGenerator::AAMazeGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create the HISM component that will hold all wall instances.
	WallInstances = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("WallInstances"));

	// Make it the root component so its transform follows the actor
	RootComponent = WallInstances;

	// Optional: helps with performance; you can tweak later
	WallInstances->SetMobility(EComponentMobility::Static);

}



void AAMazeGenerator::BeginPlay()
{
	Super::BeginPlay();

	// Seed the random generator ONCE per play session.
	// (If you call srand multiple times quickly, you may get identical mazes.)
	srand(static_cast<unsigned>(time(nullptr)));

	// Safety checks
	if (!WallInstances)
	{
		UE_LOG(LogTemp, Error, TEXT("WallInstances is null. Component was not created."));
		return;
	}

	if (!WallMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("WallMesh is not assigned. Please assign a mesh in the editor."));
		return;
	}

	// Assign the mesh that will be instanced for each wall cell
	WallInstances->SetStaticMesh(WallMesh);

	// Clear old instances (useful if you regenerate later)
	WallInstances->ClearInstances();

	// Generate the maze grid
	GenerateMaze gen;
	const int height = MazeHeight;
	const int width = MazeWidth;

	std::vector<uint8_t> grid = gen.BacktrackingMazeGenerator(height, width);

	// Centering: we want the maze centered on the actor location.
	// If CellSize = 100, each cell is 1 meter.
	//
	// Maze spans:
	//  X size = height * CellSize
	//  Y size = width  * CellSize
	//
	// Center offset moves the "origin cell (0,0)" so that the maze is centered around (0,0).
	const float halfX = (static_cast<float>(height - 1) * CellSize) * 0.5f;
	const float halfY = (static_cast<float>(width - 1) * CellSize) * 0.5f;

	// We'll place walls at Z=0. You can later raise them or set a wall height.
	const float Z = 0.0f;

	// Add one instance per wall cell
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			const int idx = i * width + j;

			// If it's a wall (1), spawn an instance
			if (grid[idx] == 1)
			{
				// Position relative to the actor (maze centered)
				const float X = (static_cast<float>(i) * CellSize) - halfX;
				const float Y = (static_cast<float>(j) * CellSize) - halfY;

				const FVector Location(X, Y, Z);

				// Transform for this instance (no rotation, scale=1)
				const FTransform InstanceTransform(FRotator::ZeroRotator, Location, FVector(1.0f, 1.0f, 1.0f));

				WallInstances->AddInstance(InstanceTransform);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Maze generated: %dx%d, instances: %d"),
		height, width, WallInstances->GetInstanceCount());
}



// Called every frame
void AAMazeGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}






