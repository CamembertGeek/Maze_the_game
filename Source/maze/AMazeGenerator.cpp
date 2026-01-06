// Fill out your copyright notice in the Description page of Project Settings.

#include "AMazeGenerator.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Math/TransformNonVectorized.h" // FTransform
#include "Math/Vector.h"                // FVector
#include "Logging/LogMacros.h"          // UE_LOG
#include "Components/StaticMeshComponent.h"

#include <vector>    // std::vector
#include <cstdint>   // uint8_t
#include <utility>   // std::pair





// Class for generation of the maze by using backtraking technic.
class GenerateMaze {
public:

	// Generate a maze grid with Backtracking.
	std::vector<uint8_t> BacktrackingMazeGenerator(FRandomStream& Rng, int height = 100, int width = 101)
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

				const int x = Rng.RandRange(0, NbNeighbor - 1); // We select a random neighbor

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
	// This actor does not need per-frame updates (performance friendly).
	PrimaryActorTick.bCanEverTick = false;

	// Create a simple SceneComponent to serve as a stable root for all other components.
	// Using a dedicated root makes it easier to move/rotate the whole actor in the level.
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);

	// Create a Hierarchical Instanced Static Mesh (HISM) component that will render
	// all maze walls as instances of the same mesh (very efficient for large mazes).
	WallInstances = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("WallInstances"));
	WallInstances->SetupAttachment(SceneRoot);

	// Walls are static geometry (no movement at runtime), so mark them as Static
	// for better rendering/lighting performance.
	WallInstances->SetMobility(EComponentMobility::Static);

	// Create a StaticMeshComponent for the floor (single mesh scaled to maze size).
	Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	Floor->SetupAttachment(SceneRoot);

	// The floor also does not move at runtime.
	Floor->SetMobility(EComponentMobility::Static);
}






// Called every frame
void AAMazeGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void AAMazeGenerator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	BuildMaze(); // Visible in editor
}

void AAMazeGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	// If you already see it in editor thanks to OnConstruction,
	// you can skip rebuilding at runtime.
	// (Leave this if you spawn the actor dynamically at runtime)
	if (WallInstances->GetInstanceCount() == 0)
	{
		BuildMaze();
	}
}

void AAMazeGenerator::BuildMaze()
{
	// ---- Safety checks ----
	if (!WallInstances)
	{
		UE_LOG(LogTemp, Error, TEXT("BuildMaze(): WallInstances is null."));
		return;
	}

	if (!WallMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("BuildMaze(): WallMesh is not assigned."));
		return;
	}

	// ---- Prepare instancing ----
	WallInstances->SetStaticMesh(WallMesh);
	WallInstances->ClearInstances();

	// ---- Generate maze grid (stable with Seed) ----
	FRandomStream Rng(Seed);

	GenerateMaze gen;
	const int height = MazeHeight;
	const int width = MazeWidth;

	std::vector<uint8_t> grid = gen.BacktrackingMazeGenerator(Rng, height, width);

	// ---- Centering (maze centered around actor origin) ----
	const float halfX = (static_cast<float>(height - 1) * CellSize) * 0.5f;
	const float halfY = (static_cast<float>(width - 1) * CellSize) * 0.5f;

	// ---- Wall mesh bounds -> compute scale ----
	const FBoxSphereBounds Bounds = WallMesh->GetBounds();
	const FVector MeshSize = Bounds.BoxExtent * 2.0f;

	if (MeshSize.X <= KINDA_SMALL_NUMBER || MeshSize.Y <= KINDA_SMALL_NUMBER || MeshSize.Z <= KINDA_SMALL_NUMBER)
	{
		UE_LOG(LogTemp, Error, TEXT("BuildMaze(): WallMesh has invalid bounds size."));
		return;
	}

	// Scale walls so that each cell is exactly CellSize wide/deep, and height = WallHeight
	const float ScaleX = CellSize / MeshSize.X;
	const float ScaleY = CellSize / MeshSize.Y;
	const float ScaleZ = WallHeight / MeshSize.Z;

	// Place walls on the floor even if pivot isn't centered
	const float LocalBottomZ = Bounds.Origin.Z - Bounds.BoxExtent.Z;
	const float ZOffset = -LocalBottomZ * ScaleZ;

	// ---- Spawn wall instances ----
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			const int idx = i * width + j;

			if (grid[idx] == 1) // wall
			{
				const float X = (static_cast<float>(i) * CellSize) - halfX;
				const float Y = (static_cast<float>(j) * CellSize) - halfY;

				const FTransform T(
					FRotator::ZeroRotator,
					FVector(X, Y, ZOffset),
					FVector(ScaleX, ScaleY, ScaleZ)
				);

				WallInstances->AddInstance(T);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("BuildMaze(): %dx%d | Wall instances: %d"),
		height, width, WallInstances->GetInstanceCount());

	// ---- Floor auto scale ----
	if (Floor && FloorMesh)
	{
		Floor->SetStaticMesh(FloorMesh);

		// Total maze span in UU
		const float MazeSpanX = height * CellSize;
		const float MazeSpanY = width * CellSize;

		const FBoxSphereBounds FB = FloorMesh->GetBounds();
		const FVector FloorSize = FB.BoxExtent * 2.0f;

		UE_LOG(LogTemp, Warning, TEXT("BuildMaze(): Floor=%s FloorMesh=%s"),
			Floor ? TEXT("OK") : TEXT("NULL"),
			FloorMesh ? *FloorMesh->GetName() : TEXT("NULL"));

		if (FloorSize.X <= KINDA_SMALL_NUMBER || FloorSize.Y <= KINDA_SMALL_NUMBER)
		{
			UE_LOG(LogTemp, Error, TEXT("BuildMaze(): FloorMesh has invalid bounds size."));
			return;
		}

		const float ScaleX_Floor = MazeSpanX / FloorSize.X;
		const float ScaleY_Floor = MazeSpanY / FloorSize.Y;

		// Floor is attached to the root => use RELATIVE transforms
		Floor->SetRelativeScale3D(FVector(ScaleX_Floor, ScaleY_Floor, 1.0f));

		// Compensate pivot offset so the floor is centered under the maze
		const float OffsetX = -FB.Origin.X * ScaleX_Floor;
		const float OffsetY = -FB.Origin.Y * ScaleY_Floor;

		// Put the floor at FloorZ (if you want it exactly at Z=FloorZ)
		Floor->SetRelativeLocation(FVector(OffsetX, OffsetY, FloorZ));

		UE_LOG(LogTemp, Log, TEXT("BuildMaze(): Floor scale (X,Y) = (%.3f, %.3f)"),
			ScaleX_Floor, ScaleY_Floor);

		UE_LOG(LogTemp, Warning, TEXT("MazeSpanX=%.1f MazeSpanY=%.1f | FloorSizeX=%.1f FloorSizeY=%.1f | ScaleX=%.3f ScaleY=%.3f"),
			MazeSpanX, MazeSpanY, FloorSize.X, FloorSize.Y, ScaleX_Floor, ScaleY_Floor);
	}
}

