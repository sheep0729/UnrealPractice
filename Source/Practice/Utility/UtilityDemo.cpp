// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilityDemo.h"

#include "XmlFile.h"
#include "XmlNode.h"

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

void UUtilityDemo::XmlDemo()
{
	const FString XMLFilePath = FPaths::ProjectDir() / TEXT("Asset/Test.xml");
	if (!FPaths::FileExists(XMLFilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("File Not Exist: %s"), *XMLFilePath);
		return;
	}

	FXmlFile* XML = new FXmlFile();
	XML->LoadFile(XMLFilePath);
	FXmlNode* RootNode = XML->GetRootNode();

	const FString FromContent = RootNode->FindChildNode("from")->GetContent();
	UE_LOG(LogTemp, Display, TEXT("from = %s"), *FromContent);

	const FString NoteName = RootNode->GetAttribute("name");
	UE_LOG(LogTemp, Display, TEXT("note @name = %s"), *NoteName);

	for (TArray<FXmlNode*> ListNode = RootNode->FindChildNode("list")->GetChildrenNodes(); const FXmlNode* Node : ListNode)
	{
		UE_LOG(LogTemp, Display, TEXT("list: %s "), *(Node-> GetContent()));
	}
}
	}
}
