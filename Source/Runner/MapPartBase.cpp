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
	
	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("AttachPoint"));
	ArrowComp->SetupAttachment(FloorMesh);

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
	OccupiedLanes.SetNum(3);
	Super::BeginPlay();
	for (int8 i = 0; i < OccupiedLanes.Num() - 1; i++)
	{
		OccupiedLanes[i] = false;
	}	
	Spawn();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AMapPartBase::CollisionBoxBeginOverlap);
	SetLifeSpan(LifeTime);
}

// Called every frame
void AMapPartBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMapPartBase::CollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARunnerGameMode* Gamemode = Cast<ARunnerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (Gamemode)
	{
		Gamemode->ChangeScores(10);
	}
}

void AMapPartBase::Spawn()
{	
	int8 SpawnChance = FMath::RandRange(0, 100);
	
	if (SpawnChance <= PercentSpawnChance)
	{			
		int8 Lanestooccupy = FMath::RandRange(0, 100);
		if (Lanestooccupy <= 50)
		{			
			SpawnObstacle(false);
		}
		else
		{			
			SpawnObstacle(true);			
		}			
	}	
	if(SpawnChance<= 90)
	SpawnPickUp();
	
}

void AMapPartBase::SpawnPickUp()
{
	int8 RandPickUpType = FMath::RandRange(0, PickUps.Num() - 1);
	
	if (PickUps[RandPickUpType].GetDefaultObject()->ObjectName == "Coin")
	{
		int8 Random = FMath::RandRange(1, 9);

		FVector Offcet(400.0f, 0.0f, 0.0f);
		FVector Loc = SpawnRules() - Offcet;
		FVector StepBetweenCoins(80.0f, 0.0f, 0.0f);
		for (int8 i = 0; i < Random; i++)
		{
			Loc += StepBetweenCoins;
			APickUpBase* Coin = Cast<APickUpBase>(GetWorld()->SpawnActor(PickUps[RandPickUpType], &Loc));
			if (Coin)
			{
				Coin->SetLifeSpan(LifeTime);
			}
		}
	}	
}

void AMapPartBase::SpawnObstacle(bool TwoObjects)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	FRotator Rot = GetActorRotation();

	int8 RandObstacleType = FMath::RandRange(0, Obstacles.Num()-1);
	FVector SpawnLocation = SpawnRules();
	

		AObstacleBase* Obstacle = Cast<AObstacleBase>(GetWorld()->SpawnActor(Obstacles[RandObstacleType], &SpawnLocation, &Rot, SpawnParams));
		if (Obstacle)
		{
			Obstacle->SetLifeSpan(LifeTime);

		}
		if (TwoObjects)
		{
			SpawnObstacle(false);
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

