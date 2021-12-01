// Fill out your copyright notice in the Description page of Project Settings.

#include "MapPartBase.h"

#include "RunnerGameMode.h"

// Sets default values
AMapPartBase::AMapPartBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("MainScene"));
	RootComponent = SceneComp;

	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Floor"));
	FloorMesh->SetupAttachment(RootComponent);

	ArrowEndLocComp = CreateDefaultSubobject<UArrowComponent>(TEXT("EndLocation"));
	ArrowEndLocComp->SetupAttachment(FloorMesh);

	TileLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("WorldLocation"));
	TileLocation->SetupAttachment(FloorMesh);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("EventBox"));
	BoxComponent->SetupAttachment(FloorMesh);

	Left = CreateDefaultSubobject<UArrowComponent>(TEXT("Left"));
	Left->SetupAttachment(FloorMesh);
	Mid = CreateDefaultSubobject<UArrowComponent>(TEXT("Mid"));
	Mid->SetupAttachment(FloorMesh);
	Right = CreateDefaultSubobject<UArrowComponent>(TEXT("Right"));
	Right->SetupAttachment(FloorMesh);
}

// Called when the game starts or when spawned
void AMapPartBase::BeginPlay()
{
	Super::BeginPlay();
	OccupiedLanes.SetNum(3);

	for (int8 i = 0; i < OccupiedLanes.Num() - 1; i++)
	{
		OccupiedLanes[i] = false;
	}
	//Spawn();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AMapPartBase::CollisionBoxBeginOverlap);
}

// Called every frame
void AMapPartBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMapPartBase::CollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCollidePersonOnce)
	{
		ARunnerGameMode* Gamemode = Cast<ARunnerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (Gamemode)
		{
			Gamemode->ChangeScores(10);
			Gamemode->SpawnMapPart();
		}
		//UE_LOG(LogTemp, Warning, TEXT("AMapPartBase::CollisionBoxBeginOverlap -  %s"), *this->GetName());

		// prevent multiple overlap
		bCollidePersonOnce = false;
	}
	
}

FVector AMapPartBase::SpawnRules()
{
	int8 RandSpawnLane = FMath::RandRange(1, 3);
	if (RandSpawnLane == 1)
	{
		if (OccupiedLanes[0] == false)
		{
			LocactionForSpawn = Left->GetComponentLocation();
			OccupiedLanes[0] = true;
		}
		else
		{
			LocactionForSpawn = SpawnRules();
		}
	}
	else if (RandSpawnLane == 2)
	{
		if (OccupiedLanes[1] == false)
		{
			LocactionForSpawn = Mid->GetComponentLocation();
			OccupiedLanes[1] = true;
		}
		else
		{
			LocactionForSpawn = SpawnRules();
		}
	}
	else if (RandSpawnLane == 3)
	{
		if (OccupiedLanes[2] == false)
		{
			LocactionForSpawn = Right->GetComponentLocation();
			OccupiedLanes[2] = true;
		}
		else
		{
			LocactionForSpawn = SpawnRules();
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Location %s"), *LocactionForSpawn.ToString());
	return LocactionForSpawn;
}

void AMapPartBase::DestroyTile()
{
	this->GetAllChildActors(Children);
	for (int8 i = 0; i < Children.Num(); i++)
	{
		Children[i]->Destroy();
	}
	this->Destroy();
}