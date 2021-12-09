// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerGameInstance.h"

bool URunnerGameInstance::GetTiles(TArray<FTileInfo>& OutTiles)
{	
	bool res = true;
	if (MapTilesTable) {
		TArray<FName> RowNames = MapTilesTable->GetRowNames();

		int8 i = 0;
		while (i < RowNames.Num()) {
			
			FTileInfo* TileItem = MapTilesTable->FindRow<FTileInfo>(RowNames[i], "");
			if (TileItem->Type != ETileType::None)
			{
				OutTiles.Add(*TileItem);
				UE_LOG(LogTemp, Warning, TEXT("URunnerGameInstance::GetTiles: [INfo] Tile - %s"), *RowNames[i].ToString());
			}
			++i;
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("URunnerGameInstance::GetTiles: [Error] TilesTable is NULL"));
		res = false;
	}

	return res;
}
