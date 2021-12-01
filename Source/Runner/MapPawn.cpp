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
		if (!Head)
		{
			// create head node
			Head = new Node();
			Head->Tile = Tile;
			Head->Prev = Head;
			Head->Next = Head;
		}
		else
		{
			Node* NewNode = new Node();

			// create new Node
			NewNode->Tile = Tile;
			NewNode->Prev = Head->Prev;
			NewNode->Next = Head;

			// modify chain
			Head->Prev->Next = NewNode;
			Head->Prev = NewNode;
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

	FVector NewLocation = FVector(0.0f, 0.0f, 0.0f);
	if (!Head)
		NewLocation = GetActorLocation();
	else
	{
		NewLocation = Head->Prev->Tile->ArrowEndLocComp->GetComponentTransform().GetLocation();
	}

	FTransform NewTransform(NewLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;

	CurrentMapLength++;
	//UE_LOG(LogTemp, Warning, TEXT("AMapPawn::CreateNewTile ArrowLoc -  %s, TileNum - %i"), *NewLocation.ToString(), CurrentMapLength);

	AMapPartBase* NewPart = Cast<AMapPartBase>(GetWorld()->SpawnActor(MapElementsTypes[0], &NewTransform, SpawnParams));
	AddTileToMap(NewPart);
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
	if (Head->Prev != Head)
	{
		Node* NewHead = Head->Next;
		NewHead->Prev = Head->Prev;
		Head->Prev->Next = NewHead;

		Head->Tile->DestroyTile();
		Head = NewHead;

		CurrentMapLength--;
	}
	//UE_LOG(LogTemp, Warning, TEXT("AMapPawn::DeleteLastTile: TileNum - %i"), CurrentMapLength);
}