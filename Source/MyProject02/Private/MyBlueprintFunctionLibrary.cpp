// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"
#include "Runtime/Core/Public/HAL/FileManager.h"
#include "HAL/FileManagerGeneric.h"


bool UMyBlueprintFunctionLibrary::LoadStringFromFile(const FString& FilePath, FString& RetString)
{
	if (!FilePath.IsEmpty())
	{
		if (FFileHelper::LoadFileToString(RetString, *FilePath))
		{
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load file: %s"), *FilePath);
			return false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("File path is empty"));
		return false;
	}
}

bool UMyBlueprintFunctionLibrary::WriteStringToFile(TArray<FString> SaveArray, const FString& FilePath)
{
	if (!FilePath.IsEmpty()) {
		if (FFileHelper::SaveStringArrayToFile(SaveArray, *FilePath)) {
			return true;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Failed to write file: %s"), *FilePath);
			return false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("File path is empty"));
		return false;
	}
}

FString UMyBlueprintFunctionLibrary::GetFilePath(const FString& FileName)
{
	return FPaths::GetPath(*FileName);
}

FString UMyBlueprintFunctionLibrary::GetFileName(const FString& FilePath,bool bRemovePath)
{
	return FPaths::GetBaseFilename(*FilePath, bRemovePath);
}

FString UMyBlueprintFunctionLibrary::GetFileSuffix(const FString& FilePath)
{
	return FString();
}

void UMyBlueprintFunctionLibrary::CreateDirectory(const FString DirectoryPath)
{
	FString Path = FPaths::ProjectContentDir() / *DirectoryPath;//获取路径
	FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*Path);//创建目录
}

void UMyBlueprintFunctionLibrary::DeleteDirectory(const FString& DirectoryPath)
{
	FString DPath = FPaths::ProjectContentDir() / *DirectoryPath;//获取路径
	FPlatformFileManager::Get().GetPlatformFile().DeleteDirectoryRecursively(*DPath);//遍历删除
}

bool UMyBlueprintFunctionLibrary::MoveDirectory(const FString& SourceDirectory, const FString& DestinationDirectory)
{
	return IFileManager::Get().Move( *DestinationDirectory,*SourceDirectory);
}

TArray<FString> UMyBlueprintFunctionLibrary::FindFilesInDirectory(const FString& DirectoryPath, const FString Filter, bool File, bool Directory)
{
	TArray<FString> FileArray;
	FileArray.Empty();
	FFileManagerGeneric::Get().FindFilesRecursive(FileArray, *DirectoryPath, *Filter,File,Directory);
	return FileArray;
}

void UMyBlueprintFunctionLibrary::JsonValue(FString JsonString)
{
	//TSharedPtr<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
}

// 以上代码是一个简单的文件操作库，包含了加载、写入、获取路径、创建、删除和查找文件等功能。