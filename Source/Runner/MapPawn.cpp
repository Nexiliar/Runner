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

	// create some tiles test
	CreateNewTile();
	CreateNewTile();
	CreateNewTile();
}

// Called every frame
void AMapPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	FVector NewLocation = FVector(0.0f, 0.0f, 0.0f);
	if (!Head) 
	{
		NewLocation = GetActorLocation();
	}
	else
	{
		NewLocation = Head->Prev->Tile->ArrowComp->GetComponentTransform().GetLocation();
		UE_LOG(LogTemp, Warning, TEXT("AMapPawn::CreateNewTile ArrowLoc -  %s"), *NewLocation.ToString());
	}

	FTransform NewTransform(NewLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;
	AMapPartBase* NewPart = Cast<AMapPartBase>(GetWorld()->SpawnActor(MapElementsTypes[0], &NewTransform, SpawnParams));
	AddTileToMap(NewPart);
}

void AMapPawn::DeleteLastTile()
{
	if (Head->Prev != Head)
	{
		Node* Tail = Head->Prev;
		Head->Prev = Tail->Prev;
		Tail->Next = Head;

		delete Tail;
	}
}

