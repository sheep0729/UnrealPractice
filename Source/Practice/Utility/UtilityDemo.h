// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UtilityDemo.generated.h"


class IImageWrapper;

UCLASS()
class PRACTICE_API UUtilityDemo : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Utility Demo")
	static void RegexDemo();

	UFUNCTION(BlueprintCallable, Category = "Utility Demo")
	static void PathDemo();

	UFUNCTION(BlueprintCallable, Category = "Utility Demo")
	static void XmlDemo();

	UFUNCTION(BlueprintCallable, Category = "Utility Demo")
	static void JsonDemo();

	UFUNCTION(BlueprintCallable, Category = "Utility Demo")
	static void GConfigDemo();

	UFUNCTION(BlueprintCallable, Category = "Utility Demo")
	static void ConvertImageDemo();

	UFUNCTION(BlueprintCallable, Category = "Utility Demo")
	static bool CovertPNG2JPG(const FString& SourceName, const FString& TargetName);

	// 从图片直接加载纹理
	UFUNCTION(BlueprintCallable, Category = "Utility Demo")
	static UTexture2D* LoadTexture2DFromFilePath(FString& ImagePath, int32& OutWidth, int32& OutHeight);

private:
	static UTexture2D* LoadTexture2DFromBytesAndExtension(const FString& ImagePath, const uint8* InCompressedData, int32 InCompressedSize, int32& OutWidth,
	                                                      int32& OutHeight);

	static TSharedPtr<IImageWrapper> GetImageWrapperByExtension(const FString path);
};
