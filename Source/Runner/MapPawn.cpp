// Fill out your copyright notice in the Description page of Project Settings.

#include "MapPawn.h"
#include "RunnerGameInstance.h"
#include "RunnerGameMode.h"
#include "Kismet/GameplayStatics.h"

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
	CreateNewTile(true); // start without obstalces
	for (int8 i = 1; i < MapStartTileNum; ++i) 
	{
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
	{
		NewLocation = MapTail->Tile->ArrowEndLocComp->GetComponentTransform().GetLocation();
		UE_LOG(LogTemp, Warning, TEXT("NewLocation is %s"), *NewLocation.ToString());
	}
		

	FTransform NewTransform(NewLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;

	// determine tile type
	TSubclassOf<AMapPartBase> TileClass;
	ETileType Type = ETileType::None;

	if (bOnlyBasic)
	{
		int32 Rand = FMath::RandRange(0, MapBasicTiles.Num()-1);
		TileClass = MapBasicTiles[Rand].TileClass;
		Type = ETileType::Default;
	}
	else
	{
		std::tie(Type, TileClass) = GetTileType();
		UE_LOG(LogTemp, Warning, TEXT("CreateNewTile: Type is %s"), *UEnum::GetValueAsString(Type));
	}
		
	// spawn
	if (Type != ETileType::None)
	{
		AMapPartBase* NewPart = Cast<AMapPartBase>(GetWorld()->SpawnActor(TileClass, &NewTransform, SpawnParams));
		AddTileToMap(NewPart);
		CurrentMapLength++;

		if (!bOnlyBasic && (Type == ETileType::Default))
			SpawnObstacle(false, NewPart);

	}		
}

// TODO: expand logic for tile generator
std::pair<ETileType, TSubclassOf<AMapPartBase>> AMapPawn::GetTileType()
{
	//int8 SeedForSpawn = FMath::RandRange(0, 100);
	TSubclassOf<AMapPartBase> NewTileClass;
	ETileType Type = ETileType::None;

	//
	if (FMath::RandRange(0, 100) < TileQTESpawnChance)
	{
		// test
		int32 Rand = FMath::RandRange(0, MapQTETiles.Num() - 1);
		NewTileClass = MapQTETiles[Rand].TileClass;
		Type = ETileType::QTE;
		TileQTESpawnChance = 0;
	}
	else
	{
		// test
		TileQTESpawnChance = (TileQTESpawnChance >= QTESpawnChance) ? 30 : (TileQTESpawnChance + 5);
		
		int32 Rand = FMath::RandRange(0, MapBasicTiles.Num() - 1);
		NewTileClass = MapBasicTiles[Rand].TileClass;
		Type = ETileType::Default;
	}
	UE_LOG(LogTemp, Warning, TEXT("AMapPawn::GetTileType: [INfo] NewTyle - %s"), *UEnum::GetValueAsString(Type));
	return std::make_pair(Type, NewTileClass);
}


void AMapPawn::SpawnObstacle(bool TwoObjects, AMapPartBase* MapTile)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;
	FRotator Rot = MapTile->GetActorRotation();

	int8 RandObstacleType = FMath::RandRange(0, Obstacles.Num() - 1);
	FVector SpawnLocation = MapTile->SpawnRules();

	AObstacleBase* Obstacle = Cast<AObstacleBase>(GetWorld()->SpawnActor(Obstacles[RandObstacleType], &SpawnLocation, &Rot, SpawnParams));
	if (Obstacle)
	{
		MapTile->Children.Add(Obstacle);
	}
	if (TwoObjects)
	{
		SpawnObstacle(false, MapTile);
	}
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