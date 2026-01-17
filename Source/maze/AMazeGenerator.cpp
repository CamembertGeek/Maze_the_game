// Fill out your copyright notice in the Description page of Project Settings.

#include "AMazeGenerator.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Math/TransformNonVectorized.h" // FTransform
#include "Math/Vector.h"                // FVector
#include "Logging/LogMacros.h"          // UE_LOG
#include "Components/StaticMeshComponent.h"

#if WITH_EDITOR
#include "UObject/UnrealType.h" // FPropertyChangedEvent
#endif

#include <vector>    // std::vector
#include <cstdint>   // uint8_t
#include <utility>   // std::pair
#include <ctime>        // std::time
#include <algorithm>    // std::random_shuffle
#include <cstdlib>      // std::rand, std::srand
#include <span>
#include <cmath> // std::abs





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

		// Add Loops on the grid:
		std::vector<uint8_t> GridWithLoops = AddLoops(Grid, Rng, height, width, 0.1f);


		return GridWithLoops;
	}

	// Convert (i, j) into 1D index
	int Indexation(int i, int j, int width)
	{
		return i * width + j;
	}

	// Add Loops inside of the maze.
	std::vector<uint8_t> AddLoops(std::vector<uint8_t>& Grid, FRandomStream& Rng, int height, int width, float LoopFactor = 0.1) {

		if (LoopFactor < 0.0f || LoopFactor > 1.0f) {
			// Raise a value error
			UE_LOG(LogTemp, Error, TEXT("LoopFactor must be between 0 and 1."));
			return Grid;
		}
		
		std::vector<uint8_t> LoopGrid = Grid; // Creation of a copy of the Maze grid.

		std::vector<int> Walls;

		for (int i = 1; i <= height - 2; ++i) {
			for (int j = 1; j <= width - 2; ++j) {

				int LoopGridIdx = Indexation(i, j, width);

				bool LR = Grid[Indexation(i, j - 1, width)] == 0 && Grid[Indexation(i, j + 1, width)] == 0;
				bool UD = Grid[Indexation(i - 1, j, width)] == 0 && Grid[Indexation(i + 1, j, width)] == 0;

				if (Grid[LoopGridIdx] == 1 && (LR || UD))
					Walls.push_back(LoopGridIdx);
			}
		}

		// Select how many walls will be deleted:
		int k = static_cast<int>(LoopFactor * static_cast<float>(Walls.size()));
		k = FMath::Clamp(k, 0, static_cast<int>(Walls.size()));
		if (k == 0)
			return LoopGrid;

		// Select random walls throuht the wall list and delet them.
		// Fisher–Yates shuffle using FRandomStream (deterministic)
		for (int i = static_cast<int>(Walls.size()) - 1; i > 0; --i)
		{
			const int j = Rng.RandRange(0, i);
			std::swap(Walls[i], Walls[j]);
		}

		// Delet the selected walls:
		for (int n = 0; n < k; ++n)
		{
			LoopGrid[Walls[n]] = 0;
		}

		return LoopGrid;
	}


	std::vector<std::vector<uint8_t>> GenerateMultiLayerMaze(FRandomStream& Rng, int height = 100, int width = 101, int MinLayer = 7, int MaxLayer = 10, float ChanceForTheHolleToStay = 0.7, int minConnectorDistance = 5) {
		
		// First we randomly select a number of layer.
		int NumLayer = Rng.RandRange(MinLayer, MaxLayer);

		// Creation of the Mazes.
		std::vector<std::vector<uint8_t>> MultiGrid;

		for (int i = 0; i < NumLayer; ++i) {

			std::vector<uint8_t> Grid = BacktrackingMazeGenerator(Rng, height, width);

			MultiGrid.push_back(Grid);
		}

		// Now we dig holles between Maze grid.
		for (int i = 0; i < NumLayer - 1; ++i) {

			std::vector<uint8_t>& UpLayer = MultiGrid[i];
			const std::vector<uint8_t>& DownLayer = MultiGrid[i + 1];

			std::vector<int32> EligibleCellForDig;

			for (int m = 0; m < height; ++m) {
				for (int n = 0; n < width; ++n) {

					int GridIdx = Indexation(m, n, width);

					if (UpLayer[GridIdx] == 0 && DownLayer[GridIdx] == 0) {
						EligibleCellForDig.push_back(GridIdx);
					}
				}
			}

			// Shuffle the Eligible Cells
			for (int p = static_cast<int>(EligibleCellForDig.size()) - 1; p > 0; --p)
			{
				const int j = Rng.RandRange(0, p);
				std::swap(EligibleCellForDig[p], EligibleCellForDig[j]);
			}


			if (EligibleCellForDig.empty()) {
				continue;
			}
			
			std::vector<int32> Holles{ EligibleCellForDig[0] };

			for (int j = 1; j < static_cast<int>(EligibleCellForDig.size()); ++j)
			{
				const int32 Idx = EligibleCellForDig[j];

				const int row = Idx / width;
				const int col = Idx % width;

				bool bFarEnough = true;

				for (int k = 0; k < static_cast<int>(Holles.size()); ++k)
				{
					const int32 HolleIdx = Holles[k];
					const int HolleRow = HolleIdx / width;
					const int HolleCol = HolleIdx % width;

					const int d = std::abs(row - HolleRow) + std::abs(col - HolleCol);

					if (d < minConnectorDistance)
					{
						bFarEnough = false;
						break;
					}
				}

				if (bFarEnough)
				{
					Holles.push_back(Idx);
				}
			}

			// We randomly delet some holle to avoid patern.
			ChanceForTheHolleToStay = FMath::Clamp(ChanceForTheHolleToStay, 0.0f, 1.0f);

			std::vector<int32> FilteredHoles;
			FilteredHoles.reserve(Holles.size());

			for (int u = 0; u < static_cast<int>(Holles.size()); ++u)
			{
				if (Rng.FRand() <= ChanceForTheHolleToStay)
				{
					FilteredHoles.push_back(Holles[u]);
				}
			}

			Holles = std::move(FilteredHoles);

			// Adding of the holles in the grids, they will be represented by a 2.
			for (int j = 0; j < Holles.size(); ++j) {
				int HollesIdx = Holles[j];
				UpLayer[HollesIdx] = 2;
			}

		}
		return MultiGrid;
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

	SceneRoot->SetMobility(EComponentMobility::Static);

	// Create a Hierarchical Instanced Static Mesh (HISM) component that will render
	// all maze walls as instances of the same mesh (very efficient for large mazes).
	WallInstances = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("WallInstances"));
	WallInstances->SetupAttachment(SceneRoot);

	// Walls are static geometry (no movement at runtime), so mark them as Static
	// for better rendering/lighting performance.
	WallInstances->SetMobility(EComponentMobility::Static);

	// Floor tiles instances
	FloorTileInstances = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("FloorTileInstances"));
	FloorTileInstances->SetupAttachment(SceneRoot);
	FloorTileInstances->SetMobility(EComponentMobility::Static);

	// Hole marker instances (optional debug)
	HoleMarkerInstances = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HoleMarkerInstances"));
	HoleMarkerInstances->SetupAttachment(RootComponent);
	HoleMarkerInstances->SetMobility(EComponentMobility::Static);
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

	const bool bNeedWalls = (WallInstances && WallInstances->GetInstanceCount() == 0);
	const bool bNeedTiles = (FloorTileInstances && FloorTileInstances->GetInstanceCount() == 0);

	if (bNeedWalls || bNeedTiles)
	{
		BuildMaze();
	}
}




void AAMazeGenerator::BuildMaze()
{
	// -------------------------
	// Safety: avoid editor-time crashes
	// -------------------------

	// 1) Never build on the Class Default Object (CDO).
	// The CDO has no valid world and is used for defaults / blueprint class preview.
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

	// 2) If we don't have a valid World (can happen during editor transactions), abort.
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 3) Optional: when editor is shutting down / tearing down the world.
	if (World->bIsTearingDown)
	{
		return;
	}




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

	// Floor tile system checks (you want one floor per layer -> use HISM tiles)
	if (!FloorTileInstances)
	{
		UE_LOG(LogTemp, Error, TEXT("BuildMaze(): FloorTileInstances is null."));
		return;
	}

	if (!FloorTileMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("BuildMaze(): FloorTileMesh is not assigned."));
		// You can return or keep going without floors
		return;
	}

	// Optional hole markers
	const bool bUseHoleMarkers = (HoleMarkerInstances != nullptr && HoleMarkerMesh != nullptr);

	// ---- Prepare instancing (clear previous build) ----
	WallInstances->SetStaticMesh(WallMesh);
	WallInstances->ClearInstances();

	// HISM components can generate collision for every instance.
	// With a cube mesh, this can get heavy and sometimes unstable in editor.
	WallInstances->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WallInstances->SetGenerateOverlapEvents(false);


	FloorTileInstances->SetStaticMesh(FloorTileMesh);
	FloorTileInstances->ClearInstances();


	// -------------------------
	// Collision policy
	// -------------------------

	const bool bIsEditorWorld = (World && World->WorldType == EWorldType::Editor);

	// In Editor we usually disable collisions on huge HISMs to keep the editor responsive.
	// In Game/PIE we enable collisions so the character stands on the tiles.
	const bool bEnableFloorCollision = (!bIsEditorWorld) && bEnableFloorCollisionInGame;
	const bool bEnableWallCollision = (!bIsEditorWorld) && bEnableWallCollisionInGame;

	// Walls
	WallInstances->SetGenerateOverlapEvents(false);
	WallInstances->SetCanEverAffectNavigation(false);
	WallInstances->SetCollisionEnabled(bEnableWallCollision ? ECollisionEnabled::QueryAndPhysics
		: ECollisionEnabled::NoCollision);

	// Floor tiles
	FloorTileInstances->SetGenerateOverlapEvents(false);
	FloorTileInstances->SetCanEverAffectNavigation(false);
	FloorTileInstances->SetCollisionEnabled(bEnableFloorCollision ? ECollisionEnabled::QueryAndPhysics
		: ECollisionEnabled::NoCollision);

	// You can also force a simple collision profile:
	if (bEnableFloorCollision)
	{
		FloorTileInstances->SetCollisionProfileName(TEXT("BlockAll"));
	}
	else
	{
		FloorTileInstances->SetCollisionProfileName(TEXT("NoCollision"));
	}

	

	if (bUseHoleMarkers)
	{
		HoleMarkerInstances->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HoleMarkerInstances->SetGenerateOverlapEvents(false);
		HoleMarkerInstances->SetCanEverAffectNavigation(false);
		HoleMarkerInstances->SetCastShadow(false);
		HoleMarkerInstances->bVisibleInRayTracing = false;

		HoleMarkerInstances->SetStaticMesh(HoleMarkerMesh);
		HoleMarkerInstances->ClearInstances();

		HoleMarkerInstances->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HoleMarkerInstances->SetGenerateOverlapEvents(false);
	}

	// ---- Generate multilayer maze (stable with Seed) ----
	FRandomStream Rng(Seed);

	GenerateMaze gen;
	int32 Height = MazeHeight;
	int32 Width = MazeWidth;

	#if WITH_EDITOR
	if (World && World->WorldType == EWorldType::Editor && bPreviewReduceResolution && !bFullBuildInEditor)
	{
		Height = FMath::Clamp(PreviewHeight, 5, MazeHeight);
		Width = FMath::Clamp(PreviewWidth, 5, MazeWidth);
	}
	#endif

	std::vector<std::vector<uint8_t>> MultiGrid = gen.GenerateMultiLayerMaze(Rng, Height, Width);

	// -------------------------
	// Editor preview limiter (prevents OOM when changing meshes/properties)
	// -------------------------
	int32 LayersToBuild = static_cast<int32>(MultiGrid.size());

	#if WITH_EDITOR
	if (World && World->WorldType == EWorldType::Editor && !bFullBuildInEditor)
	{
		LayersToBuild = FMath::Min(LayersToBuild, PreviewMaxLayers);
	}
	#endif

	// ---- Centering (maze centered around actor origin) ----
	const float HalfX = (static_cast<float>(Height - 1) * CellSize) * 0.5f;
	const float HalfY = (static_cast<float>(Width - 1) * CellSize) * 0.5f;

	// ---- Wall mesh bounds -> compute wall scale ----
	const FBoxSphereBounds WallBounds = WallMesh->GetBounds();
	const FVector WallMeshSize = WallBounds.BoxExtent * 2.0f;

	if (WallMeshSize.X <= KINDA_SMALL_NUMBER || WallMeshSize.Y <= KINDA_SMALL_NUMBER || WallMeshSize.Z <= KINDA_SMALL_NUMBER)
	{
		UE_LOG(LogTemp, Error, TEXT("BuildMaze(): WallMesh has invalid bounds size."));
		return;
	}

	// Scale walls so that each cell is exactly CellSize wide/deep, and height = WallHeight
	const float WallScaleX = CellSize / WallMeshSize.X;
	const float WallScaleY = CellSize / WallMeshSize.Y;
	const float WallScaleZ = WallHeight / WallMeshSize.Z;

	// Place walls on the floor even if pivot isn't centered
	const float WallLocalBottomZ = WallBounds.Origin.Z - WallBounds.BoxExtent.Z;
	const float WallZOffset = -WallLocalBottomZ * WallScaleZ;

	// ---- Floor tile mesh bounds -> compute tile scale (each cell gets one tile) ----
	const FBoxSphereBounds TileBounds = FloorTileMesh->GetBounds();
	const FVector TileMeshSize = TileBounds.BoxExtent * 2.0f;

	if (TileMeshSize.X <= KINDA_SMALL_NUMBER || TileMeshSize.Y <= KINDA_SMALL_NUMBER)
	{
		UE_LOG(LogTemp, Error, TEXT("BuildMaze(): FloorTileMesh has invalid bounds size."));
		return;
	}

	// We want 1 tile per cell => scale tile so it covers exactly one CellSize x CellSize
	const float TileScaleX = CellSize / TileMeshSize.X;
	const float TileScaleY = CellSize / TileMeshSize.Y;
	const float TileScaleZ = 1.0f; // keep thickness as in mesh (or expose a parameter if you want)

	// If the tile mesh pivot is not centered, compensate so instances align with cell centers
	const float TileOffsetX = -TileBounds.Origin.X * TileScaleX;
	const float TileOffsetY = -TileBounds.Origin.Y * TileScaleY;

	// If you want tile to sit exactly on Z = LayerBaseZ + FloorZ, and tile pivot is not at bottom:
	const float TileLocalBottomZ = TileBounds.Origin.Z - TileBounds.BoxExtent.Z;
	const float TileZOffset = -TileLocalBottomZ * TileScaleZ;

	// ---- Build each layer ----
	const float LayerHeight = WallHeight + FloorThickness + LayerGap;

	for (int32 k = 0; k < LayersToBuild; ++k)
	{
		const std::vector<uint8_t>& Layer = MultiGrid[k];

		// Base Z for this layer
		const float LayerBaseZ = static_cast<float>(k) * LayerHeight;

		// Z for floors and walls in this layer
		const float FloorZWorld = LayerBaseZ + FloorZ + TileZOffset;
		const float WallZWorld = LayerBaseZ + WallZOffset;

		// ---- Spawn instances cell by cell ----
		for (int32 i = 0; i < Height; ++i)
		{
			for (int32 j = 0; j < Width; ++j)
			{
				const int32 idx = i * Width + j;

				// Cell center position (same coordinate system as your walls)
				const float X = (static_cast<float>(i) * CellSize) - HalfX;
				const float Y = (static_cast<float>(j) * CellSize) - HalfY;

				const uint8 Cell = Layer[idx];

				// 1 = wall
				if (Cell == 1)
				{
					const FTransform WallT(
						FRotator::ZeroRotator,
						FVector(X, Y, WallZWorld),
						FVector(WallScaleX, WallScaleY, WallScaleZ)
					);
					WallInstances->AddInstance(WallT);

					// If you want floors also under walls, you can still place tiles here.
					// (Optional) place tiles under everything except holes:
					const FTransform TileT(
						FRotator::ZeroRotator,
						FVector(X + TileOffsetX, Y + TileOffsetY, FloorZWorld),
						FVector(TileScaleX, TileScaleY, TileScaleZ)
					);
					FloorTileInstances->AddInstance(TileT);

					continue;
				}

				// 2 = hole => do NOT place a floor tile
				if (Cell == 2)
				{
					if (bUseHoleMarkers)
					{
						const FTransform MarkerT(
							FRotator::ZeroRotator,
							FVector(X, Y, FloorZWorld + HoleMarkerZOffset),
							FVector(1.0f, 1.0f, 1.0f)
						);
						HoleMarkerInstances->AddInstance(MarkerT);
					}
					continue;
				}

				// 0 = walkable => place floor tile
				if (Cell == 0)
				{
					const FTransform TileT(
						FRotator::ZeroRotator,
						FVector(X + TileOffsetX, Y + TileOffsetY, FloorZWorld),
						FVector(TileScaleX, TileScaleY, TileScaleZ)
					);
					FloorTileInstances->AddInstance(TileT);
				}
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("BuildMaze(): Layers=%d | WallInstances=%d | FloorTiles=%d | HoleMarkers=%d"),
		static_cast<int32>(MultiGrid.size()),
		WallInstances->GetInstanceCount(),
		FloorTileInstances->GetInstanceCount(),
		bUseHoleMarkers ? HoleMarkerInstances->GetInstanceCount() : 0
	);
}


#if WITH_EDITOR
void AAMazeGenerator::PostLoad()
{
	Super::PostLoad();
	BuildMaze(); // When the level is re-charged
}
#endif

#if WITH_EDITOR
void AAMazeGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// When dragging sliders or rapidly changing properties, Unreal fires many events.
	// Avoid rebuilding on every intermediate step (can cause crashes / lag).
	if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive)
	{
		return;
	}

	// Also avoid rebuild if we are in a bad state.
	if (HasAnyFlags(RF_ClassDefaultObject) || !GetWorld())
	{
		return;
	}

	const FName PropName = PropertyChangedEvent.Property
		? PropertyChangedEvent.Property->GetFName()
		: NAME_None;

	if (PropName == GET_MEMBER_NAME_CHECKED(AAMazeGenerator, Seed) ||
		PropName == GET_MEMBER_NAME_CHECKED(AAMazeGenerator, MazeHeight) ||
		PropName == GET_MEMBER_NAME_CHECKED(AAMazeGenerator, MazeWidth) ||
		PropName == GET_MEMBER_NAME_CHECKED(AAMazeGenerator, CellSize) ||
		PropName == GET_MEMBER_NAME_CHECKED(AAMazeGenerator, WallHeight) ||
		PropName == GET_MEMBER_NAME_CHECKED(AAMazeGenerator, WallMesh) ||
		PropName == GET_MEMBER_NAME_CHECKED(AAMazeGenerator, FloorTileMesh) ||
		PropName == GET_MEMBER_NAME_CHECKED(AAMazeGenerator, FloorZ) ||
		PropName == GET_MEMBER_NAME_CHECKED(AAMazeGenerator, LayerGap) ||
		PropName == GET_MEMBER_NAME_CHECKED(AAMazeGenerator, FloorThickness) ||
		PropName == GET_MEMBER_NAME_CHECKED(AAMazeGenerator, HoleMarkerMesh) ||
		PropName == GET_MEMBER_NAME_CHECKED(AAMazeGenerator, HoleMarkerZOffset))
	{
		BuildMaze();
	}
}
#endif

