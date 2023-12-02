// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilityDemo.h"

void UUtilityDemo::RegexDemo()
{
	const FString TextStr("ABCDEFGHIJKLMN");
	const FRegexPattern TestPattern(TEXT("C.+H"));
	if (FRegexMatcher TestMatcher(TestPattern, TextStr); TestMatcher.FindNext())
	{
		UE_LOG(LogTemp, Warning, TEXT("找到匹配内容 [%d, %d)"),
		       TestMatcher.GetMatchBeginning(), TestMatcher.GetMatchEnding());
	}
}
