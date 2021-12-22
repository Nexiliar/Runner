// Fill out your copyright notice in the Description page of Project Settings.

#include "MapPawn.h"
#include "RunnerGameInstance.h"
#include "RunnerGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Containers/UnrealString.h"

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
			// push to std::map
			MapsAllTiles.FindOrAdd(tile.Lanscape).Add(tile);
		}
	}

	// create tiles at start
	for (int8 i = 0; i < MapStartTileNum; ++i)
		CreateNewTile();
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

		CurrentMapLength++;
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

void AMapPawn::CreateNewTile()
{
	// delete last before spawn new
	if (CurrentMapLength >= MapMaxTileRuntimeNum)
		DeleteLastTile();

	// determine tile type
	TSubclassOf<AMapPartBase> TileClass;
	ETileType Type = ETileType::None;

	std::tie(Type, TileClass) = GetTileType();

	// get new tile spawn location
	FVector NewLocation = FVector(0.0f, 0.0f, 0.0f);
	if (!MapHead)
		NewLocation = GetActorLocation();
	else
	{
		NewLocation = MapTail->Tile->ArrowEndLocComp->GetComponentTransform().GetLocation();
		UE_LOG(LogTemp, Warning, TEXT("NewLocation is %s"), *NewLocation.ToString());
	}

	// set spawn params
	FTransform NewTransform(NewLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;

	// spawn in game
	if ((Type != ETileType::None) && (CurrentEnv != ELandscapeType::None))
	{
		AMapPartBase* NewPart = Cast<AMapPartBase>(GetWorld()->SpawnActor(TileClass, &NewTransform, SpawnParams));
		//if (!bOnlyBasic && (Type == ETileType::Default))
		//SpawnObstacle(false, NewPart);

		// update map info
		AddTileToMap(NewPart);
		PrevTileType = Type;
	}
}

// determine type
std::pair<ETileType, TSubclassOf<AMapPartBase>> AMapPawn::GetTileType()
{
	TSubclassOf<AMapPartBase> NewTileClass;
	ETileType NewType = ETileType::None;

	// spawn end tile for current location
	if (bShouldChangeLocation)
	{
		NewType = ETileType::EndEnvTile;

		// use current env until new timer tick
		bShouldChangeLocation = false;

		//UE_LOG(LogTemp, Warning, TEXT("AMapPawn::GetTileType:  ChangeLocation"));
	}
	else if ((PrevTileType == ETileType::None) || (PrevTileType == ETileType::EndEnvTile))
	{
		// determine env for new environment
		CurrentEnv = GetNextEnvironment();

		// set new tile as start of new location
		NewType = ETileType::StartEnvTile;

		// create timer for changing environment
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ChangeLoc, FTimerDelegate::CreateLambda([&] { bShouldChangeLocation = true; }), TimeToChangeLocation, false);
	}
	else if ((PrevTileType != ETileType::None) || (PrevTileType == ETileType::BasicEnvTile))
	{
		// use basic
		NewType = ETileType::BasicEnvTile;
	}

	// get tile class
	TArray<FTileInfo> EnvTiles = MapsAllTiles.FindRef(CurrentEnv);
	if (EnvTiles.Num() > 0)
	{
		TArray<FTileInfo> AvailableTiles;
		for (const auto& tile : EnvTiles)
		{
			if (tile.Type == NewType)
				AvailableTiles.Add(tile);
		}

		if (AvailableTiles.Num() > 0)
			NewTileClass = AvailableTiles[FMath::RandRange(0, AvailableTiles.Num() - 1)].TileClass;
	}

	//UE_LOG(LogTemp, Warning, TEXT("AMapPawn::GetTileType: [INfo] NewTyle - %s, Env - %s"), *UEnum::GetValueAsString(NewType), *UEnum::GetValueAsString(CurrentEnv));
	return std::make_pair(NewType, NewTileClass);
}

ELandscapeType AMapPawn::GetNextEnvironment()
{
	TArray<ELandscapeType> MapEnvs;
	MapsAllTiles.GetKeys(MapEnvs);

	if (MapEnvs.Num() == 0)
		return ELandscapeType::None;

	int8 i = 0;
	bool bFind = false;
	ELandscapeType NewEnv = ELandscapeType::None;
	while (i < MapEnvs.Num() && !bFind)
	{
		if ((CurrentEnv == ELandscapeType::None) || (CurrentEnv == ELandscapeType::Village))
			NewEnv = ELandscapeType::Forest;
		else if (CurrentEnv == ELandscapeType::Forest)
			NewEnv = ELandscapeType::Road;
		else if (CurrentEnv == ELandscapeType::Road)
			NewEnv = ELandscapeType::Village;

		if (CheckIfEnvExist(NewEnv))
			bFind = true;

		i++;
	}

	return NewEnv;
}

bool AMapPawn::CheckIfEnvExist(ELandscapeType NewEnv)
{
	return MapsAllTiles.Contains(NewEnv);
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