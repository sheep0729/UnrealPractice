// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilityDemo.h"

void UUtilityDemo::RegexDemo()
{
	const FString TextStr("ABCDEFGHIJKLMN");
	const FRegexPattern TestPattern(TEXT("C.+H"));
	if (FRegexMatcher TestMatcher(TestPattern, TextStr); TestMatcher.FindNext())
	{
		UE_LOG(LogTemp, Display, TEXT("找到匹配内容 [%d, %d)"), TestMatcher.GetMatchBeginning(), TestMatcher.GetMatchEnding());
	}
}

void UUtilityDemo::PathDemo()
{
	UE_LOG(LogTemp, Display, TEXT("Root Dir = %s"), *FPaths::RootDir());
	UE_LOG(LogTemp, Display, TEXT("Project Dir = %s"), *FPaths::ProjectDir());
	UE_LOG(LogTemp, Display, TEXT("Full Project Dir = %s"), *FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()));
}
	}
}
