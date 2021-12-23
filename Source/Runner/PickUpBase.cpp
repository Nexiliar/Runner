// Fill out your copyright notice in the Description page of Project Settings.

#include "PickUpBase.h"
#include "RunnerGameMode.h"
#include "RunnerCharacter.h"

// Sets default values
APickUpBase::APickUpBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("MainScene"));
	RootComponent = SceneComp;

	PickUpMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickUp"));

	PickUpMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APickUpBase::BeginPlay()
{
	Super::BeginPlay();
	PickUpMesh->OnComponentBeginOverlap.AddDynamic(this, &APickUpBase::OnOverlap);
}

// Called every frame
void APickUpBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickUpBase::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARunnerCharacter* Char = Cast<ARunnerCharacter>(OtherActor);
	if (Char)
	{
		switch (PickUpType)
		{
		case EDropItem::COIN: default:
		{
			ARunnerGameMode* Gamemode = Cast<ARunnerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			if (Gamemode)
				Gamemode->ChangeScores(AmountOfScoresGainOnPickUp);
			//Char->SetCharSpeed(Char->GetCharSpeed() * 0.99);

			break;
		}
		case EDropItem::BUFF:
		{
			Char->ChangeSpeedByBuff(SpeedMultiply, EffectTime);
			break;
		}
		case EDropItem::DEBUFF:
			Char->ChangeSpeedByBuff(SpeedMultiply, EffectTime);
			break;
		}

		UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
		Destroy();
	}
}

EDropItem APickUpBase::GetType() const
{
	return PickUpType;
}
