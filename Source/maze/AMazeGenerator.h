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
	int32 MazeHeight = 101;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Settings", meta = (ClampMin = "5"))
	int32 MazeWidth = 101;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Settings", meta = (ClampMin = "10.0"))
	float WallHeight = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Settings", meta = (ClampMin = "1.0"))
	float WallThickness = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Maze|Components")
	UStaticMeshComponent* Floor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Assets")
	UStaticMesh* FloorMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Settings", meta = (ClampMin = "0.0"))
	float FloorZ = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Settings", meta = (ClampMin = "0"))
	int32 Seed = 42;
	
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

};


