// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilityDemo.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "XmlFile.h"
#include "XmlNode.h"
#include "HAL/FileManagerGeneric.h"

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

void UUtilityDemo::JsonDemo()
{
	const FString JsonStr = "[{\"author\":\"Tim\"},{\"age\":\"100\"}]";
	const TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonStr);
	if (TArray<TSharedPtr<FJsonValue>> JsonParsed; FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		UE_LOG(LogTemp, Display, TEXT("Json 解析成功"));
		const FString Author = JsonParsed[0]->AsObject()->GetStringField("author");
		UE_LOG(LogTemp, Display, TEXT("author = %s"), *Author);
	}
}

void UUtilityDemo::GConfigDemo()
{
	const FString ConfigFile = FPaths::ProjectDir() / "Asset/MyConfig.ini";
	GConfig->SetString(
		/* Section */ TEXT("MySection"),
		              /* Key */ TEXT("Name"),
		              /* Value */ TEXT("李白"),
		              /* Filename */ ConfigFile);

	GConfig->SetInt(
		/* Section */ TEXT("MySection"),
		              /* Key */ TEXT("Age"),
		              /* Value */ 18,
		              /* Filename */ ConfigFile);

	// 通常在运行过程中，进行写入配置操作值并不会马上写入到文件。
	GConfig->Flush(false, ConfigFile);

	FString Result;
	GConfig->GetString(
		TEXT("MySection"),
		TEXT("Name"),
		Result,
		ConfigFile);

	UE_LOG(LogTemp, Display, TEXT("Name = %s"), *Result);
}

void UUtilityDemo::ConvertImageDemo()
{
	const FString ImageFilePathNoExt = FPaths::ProjectDir() / "Asset" / "ewe_1f4112";
	CovertPNG2JPG(ImageFilePathNoExt + ".png", ImageFilePathNoExt + ".jpg");
}

bool UUtilityDemo::CovertPNG2JPG(const FString& SourceName, const FString& TargetName)
{
	check(SourceName.EndsWith(TEXT(".png")) && TargetName.EndsWith(TEXT(".jpg")));

	// TODO 这个函数的实现方式不是 IImageWrapperModule 注释中推荐的，是比较底层的方式
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	const TSharedPtr<IImageWrapper> SourceImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	const TSharedPtr<IImageWrapper> TargetImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

	TArray<uint8> SourceImageData;
	TArray<uint8> TargetImageData;

	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*SourceName))
	{
		// 文件不存在
		return false;
	}

	if (!FFileHelper::LoadFileToArray(SourceImageData, *SourceName))
	{
		// 文件读取失败
		return false;
	}

	if (SourceImageWrapper.IsValid() && SourceImageWrapper->SetCompressed(SourceImageData.GetData(), SourceImageData.Num()))
	{
		if (TArray<uint8> UncompressedRGBA; SourceImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedRGBA))
		{
			const int32 Height = SourceImageWrapper->GetHeight();
			const int32 Width = SourceImageWrapper->GetWidth();

			if (TargetImageWrapper->SetRaw(UncompressedRGBA.GetData(), UncompressedRGBA.Num(), Width, Height, ERGBFormat::RGBA, 8))
			{
				TargetImageData = TargetImageWrapper->GetCompressed();
				if (!FFileManagerGeneric::Get().DirectoryExists(*TargetName))
				{
					FFileManagerGeneric::Get().MakeDirectory(*FPaths::GetPath(TargetName), true);
				}
				return FFileHelper::SaveArrayToFile(TargetImageData, *TargetName);
			}
		}
	}
	return false;
}


TSharedPtr<IImageWrapper> UUtilityDemo::GetImageWrapperByExtension(const FString path)
{
	IImageWrapperModule& module = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	if (path.EndsWith(".png"))
	{
		return module.CreateImageWrapper(EImageFormat::PNG);
	}
	if (path.EndsWith(".jpg"))
	{
		return module.CreateImageWrapper(EImageFormat::JPEG);
	}

	return nullptr;
}


UTexture2D* UUtilityDemo::LoadTexture2DFromBytesAndExtension(
	const FString& ImagePath,
	const uint8* InCompressedData,
	const int32 InCompressedSize,
	int32& OutWidth,
	int32& OutHeight
)
{
	UTexture2D* Texture = nullptr;
	const TSharedPtr<IImageWrapper> ImageWrapper = GetImageWrapperByExtension(ImagePath);
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(InCompressedData, InCompressedSize)) // 读取压缩后的图片数据
	{
		if (TArray<uint8> UncompressedRGBA; ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedRGBA)) // 获取原始图片数据
		{
			Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_R8G8B8A8);
			if (Texture != nullptr)
			{
				// 通过内存复制，填充原始 RGB 数据到贴图的数据中
				OutWidth = ImageWrapper->GetWidth();
				OutHeight = ImageWrapper->GetHeight();

				void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedRGBA.GetData(), UncompressedRGBA.Num());
				Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
				Texture->UpdateResource();
			}
		}
	}
	return Texture;
}

UTexture2D* UUtilityDemo::LoadTexture2DFromFilePath(FString& ImagePath, int32& OutWidth, int32& OutHeight)
{
	// 文件是否存在
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*ImagePath))
	{
		return nullptr;
	}

	// 读取文件资源
	TArray<uint8> CompressedData;
	if (!FFileHelper::LoadFileToArray(CompressedData, *ImagePath))
	{
		return nullptr;
	}

	return LoadTexture2DFromBytesAndExtension(ImagePath, CompressedData.GetData(), CompressedData.Num(), OutWidth, OutHeight);
}
