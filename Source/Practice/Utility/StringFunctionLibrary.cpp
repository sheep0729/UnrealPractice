// Fill out your copyright notice in the Description page of Project Settings.


#include "StringFunctionLibrary.h"
#include <string>

inline bool UStringFunctionLibrary::LessEqual_StringString(FString A, FString B)
{
	// return std::basic_string(*A) <= std::basic_string(*B);
	return A.Compare(B, ESearchCase::CaseSensitive) <= 0;
}

bool UStringFunctionLibrary::Equal_StringString(FString A, FString B)
{
	return A.Compare(B, ESearchCase::CaseSensitive) == 0;
}
