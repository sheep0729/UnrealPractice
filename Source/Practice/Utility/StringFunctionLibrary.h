#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StringFunctionLibrary.generated.h"

UCLASS()
class PRACTICE_API UStringFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, meta=(DisplayName = "string <= string", CompactNodeTitle = "<=", Keywords = "<= less"), Category="Math|String")
	static bool LessEqual_StringString(FString A, FString B);

	UFUNCTION(BlueprintPure, meta=(DisplayName = "string == string", CompactNodeTitle = "==", Keywords = "== equal"), Category="Math|String")
	static bool Equal_StringString(FString A, FString B);
};
