// Fill out your copyright notice in the Description page of Project Settings.

#include "MapPawn.h"

// Sets default values
AMapPawn::AMapPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMapPawn::BeginPlay()
{
	Super::BeginPlay();

	// create tiles at start
	for (int8 i = 0; i < MapStartTileNum; ++i) {
		CreateNewTile();
	}
}

// Called every frame
void AMapPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMapPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMapPawn::AddTileToMap(AMapPartBase* Tile)
{
	if (Tile)
	{
		if (!MapHead)
		{
			// create head node
			MapHead = new Node();
			MapHead->Tile = Tile;
			MapTail = MapHead;
			MapHead->Next = MapTail;
		}
		else
		{
			// create new Node
			Node* NewNode = new Node();
			NewNode->Tile = Tile;

			// modify chain
			MapTail->Next = NewNode;
			MapTail = NewNode;
		}
	}
}

void AMapPawn::CreateNewTile()
{
	// delete last before spawn new
	if (CurrentMapLength >= MapMaxTileNum)
	{
		DeleteLastTile();
	}

	// get new tile spawn location
	FVector NewLocation = FVector(0.0f, 0.0f, 0.0f);
	if (!MapHead)
		NewLocation = GetActorLocation();
	else
		NewLocation = MapTail->Tile->ArrowEndLocComp->GetComponentTransform().GetLocation();

	FTransform NewTransform(NewLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;

	// spawn
	AMapPartBase* NewPart = Cast<AMapPartBase>(GetWorld()->SpawnActor(MapElementsTypes[0], &NewTransform, SpawnParams));
	AddTileToMap(NewPart);

	CurrentMapLength++;
}

// TODO
//void GetTileType()
//{
//	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
//	int8 TileForSpawn = FMath::RandRange(0, 100);
//	if (TileForSpawn <= 10)
//	{
//		AMapPartBase* NewPart = Cast<AMapPartBase>(GetWorld()->SpawnActor(MapParts[1], &SpawnPoint, SpawnParams));
//		if (NewPart)
//		{
//			FVector Location = NewPart->ArrowComp->GetComponentLocation();
//			SpawnPoint.SetLocation(Location);
//			SpawnEvent(NewPart);
//		}
//	}
//	else
//	{
//		AMapPartBase* NewPart = Cast<AMapPartBase>(GetWorld()->SpawnActor(MapParts[0], &SpawnPoint, SpawnParams));
//		if (NewPart)
//		{
//			FVector Location = NewPart->ArrowComp->GetComponentLocation();
//			SpawnPoint.SetLocation(Location);
//			Spawn(NewPart);
//		}
//	}
//}

void AMapPawn::DeleteLastTile()
{
	if (MapHead->Next != MapHead)
	{
		Node* NewHead = MapHead->Next;
		
		MapHead->Tile->DestroyTile();
		MapHead = NewHead;

		CurrentMapLength--;
	}
}