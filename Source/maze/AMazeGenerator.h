// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "AMazeGenerator.generated.h"

class UHierarchicalInstancedStaticMeshComponent;
class UStaticMesh;



UCLASS()
class MAZE_API AAMazeGenerator : public AActor
{
	GENERATED_BODY()

protected:
	// Component that will hold all wall instances (HISM)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Maze|Components")
	UHierarchicalInstancedStaticMeshComponent* WallInstances = nullptr;

	// Mesh used for walls (assign in the editor)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Assets")
	UStaticMesh* WallMesh = nullptr;

	// Size of one cell in Unreal units (100 = 1 meter)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Settings", meta = (ClampMin = "10.0"))
	float CellSize = 100.0f;

	// Maze dimensions (odd values recommended for your backtracking algo)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Settings", meta = (ClampMin = "5"))
	int32 MazeHeight = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Settings", meta = (ClampMin = "5"))
	int32 MazeWidth = 51;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Editor")
	bool bPreviewReduceResolution = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Editor", meta = (ClampMin = "5"))
	int32 PreviewHeight = 51;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Editor", meta = (ClampMin = "5"))
	int32 PreviewWidth = 51;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Settings", meta = (ClampMin = "10.0"))
	float WallHeight = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Settings", meta = (ClampMin = "1.0"))
	float WallThickness = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Settings", meta = (ClampMin = "0.0"))
	float FloorZ = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Settings", meta = (ClampMin = "0"))
	int32 Seed = 42;

	// Floor tiles (one instance per cell)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Maze|Components")
	UHierarchicalInstancedStaticMeshComponent* FloorTileInstances = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Assets")
	UStaticMesh* FloorTileMesh = nullptr;

	// Optional: marker mesh to visualize holes (debug)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Maze|Components")
	UHierarchicalInstancedStaticMeshComponent* HoleMarkerInstances = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Assets")
	UStaticMesh* HoleMarkerMesh = nullptr;

	// Vertical spacing between layers
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Settings", meta = (ClampMin = "0.0"))
	float LayerGap = 200.0f;

	// Optional: thickness of a floor "slab" (only affects spacing logic if you want)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Settings", meta = (ClampMin = "0.0"))
	float FloorThickness = 20.0f;

	// Marker vertical offset so it doesn't Z-fight with the floor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Settings")
	float HoleMarkerZOffset = 10.0f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Editor")
	bool bFullBuildInEditor = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Editor", meta = (ClampMin = "1"))
	int32 PreviewMaxLayers = 1;

	// Collisions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Collision")
	bool bEnableFloorCollisionInGame = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Collision")
	bool bEnableWallCollisionInGame = true;
	
public:	
	// Sets default values for this actor's properties
	AAMazeGenerator();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	void BuildMaze();  // common function


#if WITH_EDITOR
	virtual void PostLoad() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

};


