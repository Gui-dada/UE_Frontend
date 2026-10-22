// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"

/**
 *
 */
UCLASS()
class MYPROJECT02_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "FileLibrary")//加载文件资源
		static bool LoadStringFromFile(const FString& FilePath, FString& RetString);
	UFUNCTION(BlueprintCallable, Category = "FileLibrary")//写入文件资源
		static bool WriteStringToFile(TArray<FString> SaveArray, const FString& FilePath);
	UFUNCTION(BlueprintCallable, Category = "FileLibrary")//获取文件路径
		static FString GetFilePath(const FString& FileName);
	UFUNCTION(BlueprintCallable, Category = "FileLibrary")//获取文件名
		static FString GetFileName(const FString& FilePath, bool bRemovePath);
	UFUNCTION(BlueprintCallable, Category = "FileLibrary")//获取文件后缀
		static FString GetFileSuffix(const FString& FilePath);
	UFUNCTION(BlueprintCallable, Category = "FileLibrary")//创建文件夹
		static void CreateDirectory(const FString DirectoryPath);
	UFUNCTION(BlueprintCallable, Category = "FileLibrary")//删除文件夹
		static void DeleteDirectory(const FString& DirectoryPath);
	UFUNCTION(BlueprintCallable, Category = "FileLibrary")//移动文件夹
		static bool MoveDirectory(const FString& SourceDirectory, const FString& DestinationDirectory);
	UFUNCTION(BlueprintCallable, Category = "FileLibrary")//查找文件目录下所有文件
		static TArray<FString> FindFilesInDirectory(const FString& DirectoryPath, const FString Filter,bool File,bool Directory);
	UFUNCTION(BlueprintCallable, Category = "Json")//
		static void JsonValue(FString JsonString);
		


};
