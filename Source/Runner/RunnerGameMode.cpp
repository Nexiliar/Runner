// Copyright Epic Games, Inc. All Rights Reserved.

#include "RunnerGameMode.h"
#include "RunnerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARunnerGameMode::ARunnerGameMode()
{
	// set default pawn class to our Blueprinted character	
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Runner/Expeditions/Blueprints/Characters/ThirdPersonCharacter"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}
}



void ARunnerGameMode::BeginPlay()
{
	Super::BeginPlay();	
	
	//Spawning road when game starts
	for (int i = 0; i < StartMapLength; i++)
	{
		SpawnMapPart();
	}	
	//Timer For Spawn More Additional parts 
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnMapBase, this, &ARunnerGameMode::SpawnMapPart, MapSpawnSpeed, true);


}


void ARunnerGameMode::SpawnMapPart()
{	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	

	AMapPartBase* NewPart = Cast<AMapPartBase>(GetWorld()->SpawnActor(MapPart, &SpawnPoint, SpawnParams));
	if (MapPart)
	{		
		FVector Location = NewPart->ArrowComp->GetComponentLocation();
		
		SpawnPoint.SetLocation(Location);
	}
}

void ARunnerGameMode::ChangeScores(int32 Amount)
{
	CurrentPoints += Amount;
	OnScoresChange.Broadcast(CurrentPoints);
}

int32 ARunnerGameMode::GetCurrentScores()
{
	return CurrentPoints;
}
