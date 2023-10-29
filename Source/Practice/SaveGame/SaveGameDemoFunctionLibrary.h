// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SaveGameDemoFunctionLibrary.generated.h"

DECLARE_LOG_CATEGORY_CLASS(LogSaveGameDemo, Log, All);

UCLASS()
class PRACTICE_API USaveGameDemoFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Demo|Save Game Demo")
	static void RunAsynchronousSaveLoad();

	UFUNCTION()
	static void OnAsynchronousSaveGameCompleted(const FString& SlotName, const int32 UserIndex, bool bSuccess);

	UFUNCTION()
	static void AsynchronousLoadGame(const FString& SlotName, const int32 UserIndex);

	UFUNCTION()
	static void OnAsynchronousLoadGameCompleted(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);

	UFUNCTION(BlueprintCallable, Category = "Demo|Save Game Demo")
	static void RunSynchronousSaveLoad();
};

