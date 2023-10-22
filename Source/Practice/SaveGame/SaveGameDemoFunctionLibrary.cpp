// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGameDemoFunctionLibrary.h"

#include "SaveGameDemo.h"
#include "Kismet/GameplayStatics.h"

void USaveGameDemoFunctionLibrary::RunSynchronousSaveLoad()
{
	// 创建 SaveGame
	USaveGameDemo* SaveGameInstance = Cast<USaveGameDemo>(UGameplayStatics::CreateSaveGameObject(USaveGameDemo::StaticClass()));
	if (!IsValid(SaveGameInstance))
	{
		return;
	}

	// 填充数据
	SaveGameInstance->PlayerName = TEXT("PlayerTwo");
	SaveGameInstance->SaveSlotName = "SynchronousSaveGameSlot";
	SaveGameInstance->UserIndex = 1;
	
	// 同步保存 SaveGame 到 Slot
	UE_LOG(LogSaveGameDemo, Log, TEXT("Start synchronous saving to slot..."));
	bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
	if (!bSuccess)
	{
		UE_LOG(LogSaveGameDemo, Log, TEXT("The synchronous saving to slot has failed."));
		return;
	}
	UE_LOG(LogSaveGameDemo, Log, TEXT("The synchronous saving to slot has succeeded, SlotName = [%s], User Index = [%d]."),
	       *SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);

	// 从 Slot 中同步加载 SaveGame
	UE_LOG(LogSaveGameDemo, Log, TEXT("Start synchronous loading from slot..."));
	const USaveGameDemo* LoadedGame = Cast<USaveGameDemo>(UGameplayStatics::LoadGameFromSlot(SaveGameInstance->SaveSlotName, 0));
	if (!IsValid(LoadedGame))
	{
		UE_LOG(LogSaveGameDemo, Log, TEXT("The synchronous loading from slot has failed."));
		return;
	}
	UE_LOG(LogSaveGameDemo, Log, TEXT("The synchronous loading from slot has succeeded, Player Name = [%s]."), *LoadedGame->PlayerName);

	// 同步保存 SaveGame 到内存
	UE_LOG(LogSaveGameDemo, Log, TEXT("Start synchronous saving to memory..."));
	TArray<uint8> SaveData;
	bSuccess = UGameplayStatics::SaveGameToMemory(SaveGameInstance, SaveData);// 把 SaveGame 保存在内存中，获得一个二进制的 Buffer
	if (!bSuccess)
	{
		UE_LOG(LogSaveGameDemo, Log, TEXT("The synchronous saving to memory has failed."));
		return;

	}
	UE_LOG(LogSaveGameDemo, Log, TEXT("The synchronous saving to memory has succeeded."));

	// 同步保存 Data 到 Slot
	UE_LOG(LogSaveGameDemo, Log, TEXT("Start synchronous saving data to slot..."));
	bSuccess = UGameplayStatics::SaveDataToSlot(SaveData, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex); // 直接保存二进制 Buffer 到文件中，在开发平台下，保存在 Saved/SaveGames 目录中，文件后缀是 .sav
	if (!bSuccess)
	{
		UE_LOG(LogSaveGameDemo, Log, TEXT("The synchronous saving data to slot has failed."));
		return;

	}
	UE_LOG(LogSaveGameDemo, Log, TEXT("The synchronous saving data to slot has succeeded."));
	
	// 从 Slot 中同步加载 Data
	UE_LOG(LogSaveGameDemo, Log, TEXT("Start synchronous loading data from slot..."));
	TArray<uint8> LoadedData;
	bSuccess = UGameplayStatics::LoadDataFromSlot(LoadedData, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
	if (!bSuccess)
	{
		UE_LOG(LogSaveGameDemo, Log, TEXT("The synchronous loading data from slot has failed."));
		return;
	}
	UE_LOG(LogSaveGameDemo, Log, TEXT("The synchronous loading data from slot has succeeded."));

	// 从内存中同步加载 SaveGame
	UE_LOG(LogSaveGameDemo, Log, TEXT("Start synchronous loading from memory..."));
	const USaveGameDemo* LoadedGame2 = Cast<USaveGameDemo>(UGameplayStatics::LoadGameFromMemory(LoadedData));
	if (!IsValid(LoadedGame2))
	{
		UE_LOG(LogSaveGameDemo, Log, TEXT("The synchronous loading from memory has failed."));
		return;
	}
	UE_LOG(LogSaveGameDemo, Log, TEXT("The synchronous loading from memory has succeeded, Player Name = [%s]."), *LoadedGame2->PlayerName);
}

void USaveGameDemoFunctionLibrary::RunAsynchronousSaveLoad()
{
	// 创建 SaveGame
	USaveGameDemo* SaveGameInstance = Cast<USaveGameDemo>(UGameplayStatics::CreateSaveGameObject(USaveGameDemo::StaticClass()));
	if (!IsValid(SaveGameInstance)) return;

	// 填充数据
	SaveGameInstance->PlayerName = TEXT("PlayerOne");
	SaveGameInstance->SaveSlotName = "AsynchronousSaveGameSlot";
	SaveGameInstance->UserIndex = 0;

	// 异步保存 SaveGame 到 Slot
	UE_LOG(LogSaveGameDemo, Log, TEXT("Start asynchronous saving to slot..."));
	FAsyncSaveGameToSlotDelegate SavedDelegate;
	SavedDelegate.BindStatic(&ThisClass::OnAsynchronousSaveGameCompleted);
	UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex, SavedDelegate);
}

void USaveGameDemoFunctionLibrary::OnAsynchronousSaveGameCompleted(const FString& SlotName, const int32 UserIndex, const bool bSuccess)
{
	if (bSuccess)
	{
		// 异步从 Slot 中加载 SaveGame
		UE_LOG(LogSaveGameDemo, Log, TEXT("The asynchronous saving to slot has succeeded, SlotName = [%s], User Index = [%d], Success = true."),
		       *SlotName, UserIndex);
		AsynchronousLoadGame(SlotName, UserIndex);
	}
	else
	{
		UE_LOG(LogSaveGameDemo, Log, TEXT("The asynchronous saving to slot has failed."));
	}
}

void USaveGameDemoFunctionLibrary::AsynchronousLoadGame(const FString& SlotName, const int32 UserIndex)
{
	UE_LOG(LogSaveGameDemo, Log, TEXT("Start asynchronous loading from slot..."));

	FAsyncLoadGameFromSlotDelegate LoadedDelegate;
	LoadedDelegate.BindStatic(&ThisClass::OnAsynchronousLoadGameCompleted);
	UGameplayStatics::AsyncLoadGameFromSlot(SlotName, UserIndex, LoadedDelegate);
}

void USaveGameDemoFunctionLibrary::OnAsynchronousLoadGameCompleted(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
	UE_LOG(LogSaveGameDemo, Log, TEXT("The asynchronous loading from slot has succeeded, PlayerName = [%s]."),
	       *Cast<USaveGameDemo>(LoadedGameData)->PlayerName);
}