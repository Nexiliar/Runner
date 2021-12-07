// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "MapPartBase.h"
#include "Types.generated.h"


UENUM(BlueprintType)
enum class EMovementLine : uint8
{
	LINE_1,
	LINE_2,
	LINE_3
};

UENUM(BlueprintType)
enum class ESpeedChangeTypes : uint8
{
	FirstType UMETA(DisplayName = "Increase Speed OverTime"),
	SecondType UMETA(DisplayName = "Increase Speed Over Scores"),
	ThirdType UMETA(DisplayName = "Increase Speed Over Certain Map Progress")
};

UENUM(BlueprintType)
enum class ETileType : uint8
{
	None UMETA(DisplayName = "None"),
	Default UMETA(DisplayName = "BasicTile"),
	QTE UMETA(DisplayName = "QTE"),
};

UENUM(BlueprintType)
enum class ELandscapeType : uint8
{
	None UMETA(DisplayName = "None"),
	Forest UMETA(DisplayName = "Forest"),
	Village UMETA(DisplayName = "Village"),
	Road UMETA(DisplayName = "Road"),
};

USTRUCT(BlueprintType)
struct FTileInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapTile")
		ETileType Type = ETileType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapTile")
		ELandscapeType Lanscape = ELandscapeType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapTile")
		FName TileName = FName("Tile");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapTile")
		TSubclassOf<AMapPartBase> TileClass = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapTile")
		FTransform Transform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapTile")
		float PassTileScore = 10.0f;
};

/**
 * 
 */
UCLASS()
class RUNNER_API UTypes : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};
