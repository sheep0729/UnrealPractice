// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UtilityDemo.generated.h"



UCLASS()
class PRACTICE_API UUtilityDemo : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Utility Demo")
	static void RegexDemo();

	UFUNCTION(BlueprintCallable, Category = "Utility Demo")
	static void PathDemo();
};
