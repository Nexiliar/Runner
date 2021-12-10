// Fill out your copyright notice in the Description page of Project Settings.

#include "ObstacleBase.h"
#include "Components/BoxComponent.h"
#include "RunnerCharacter.h"

// Sets default values
AObstacleBase::AObstacleBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("MainScene"));
	RootComponent = SceneComp;

	ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OBstacle"));

	ObstacleMesh->OnComponentHit.AddDynamic(this, &AObstacleBase::OnComponentHit);
	ObstacleMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AObstacleBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AObstacleBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AObstacleBase::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ARunnerCharacter* Player = Cast<ARunnerCharacter>(OtherActor);
	if (Player)
	{
		//Try to find a better solution.
		if (Hit.Distance < 1.0f)
		{
			Player->KillChar();
		}
	}
}