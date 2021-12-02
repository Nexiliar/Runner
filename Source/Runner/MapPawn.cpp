// Fill out your copyright notice in the Description page of Project Settings.

#include "MapPawn.h"
#include "RunnerGameInstance.h"

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

	// get tiles
	TArray<FTileInfo> AllTiles;
	URunnerGameInstance* GameInst = Cast<URunnerGameInstance>(GetGameInstance());
	if (GameInst && GameInst->GetTiles(AllTiles))
	{
		for (const auto& tile : AllTiles)
		{
			if (tile.Type == ETileType::Default)
				MapBasicTiles.Add(tile);
			else if (tile.Type == ETileType::QTE)
				MapQTETiles.Add(tile);
		}
	}

	// create tiles at start
	for (int8 i = 0; i < MapStartTileNum; ++i) {
		CreateNewTile(true);
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

void AMapPawn::CreateNewTile(bool bOnlyBasic)
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

	// determine tile type
	TSubclassOf<AMapPartBase> TileClass;
	if (bOnlyBasic)
		TileClass = MapBasicTiles[0].TileClass;
	else 
		TileClass = GetTileType();

	// spawn
	AMapPartBase* NewPart = Cast<AMapPartBase>(GetWorld()->SpawnActor(TileClass, &NewTransform, SpawnParams));
	if (NewPart)
	{
		AddTileToMap(NewPart);
		CurrentMapLength++;
	}
}

// TODO: expand logic for tile generator
TSubclassOf<AMapPartBase> AMapPawn::GetTileType()
{
	int8 SeedForSpawn = FMath::RandRange(0, 100);
	TSubclassOf<AMapPartBase> NewTileClass;
	UE_LOG(LogTemp, Warning, TEXT("AMapPawn::GetTileType: [INfo] SeedForSpawn - %i"), SeedForSpawn);
	if (SeedForSpawn < 30)
	{
		// test
		NewTileClass = MapQTETiles[0].TileClass;
	}
	else
	{
		// test
		NewTileClass = MapBasicTiles[0].TileClass;
	}
	return NewTileClass;
}

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