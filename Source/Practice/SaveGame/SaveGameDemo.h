#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameDemo.generated.h"

UCLASS()
class PRACTICE_API USaveGameDemo : public USaveGame
{
	GENERATED_BODY()

public:

	USaveGameDemo();
	
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;
};
