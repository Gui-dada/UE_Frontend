// Fill out your copyright notice in the Description page of Project Settings.


#include "MyStringActor.h"
#include "Kismet/KismetStringLibrary.h"
// 设置默认值
AMyStringActor::AMyStringActor()
{
	// 设置这个Actor是否可以被“点击”（即，是否可以被鼠标点击）
	PrimaryActorTick.bCanEverTick = true;

}

// 在游戏开始或生成时调用
void AMyStringActor::BeginPlay()
{
	Super::BeginPlay();
	
}

//  在每一帧调用
void AMyStringActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyStringActor::MyStringOperation(FString MyString)
{
	//判断字符串是否包含子字符串
	bool isFind1 = MyString.Contains("b",ESearchCase::IgnoreCase,ESearchDir::FromStart);
	bool isFind2 = UKismetStringLibrary::Contains(MyString, "b", ESearchCase::IgnoreCase, ESearchDir::FromStart);
	//判断字符串是否相等
	bool isEquals1 = MyString.Equals("abcd_1234",ESearchCase::IgnoreCase);
	bool isEquals2 = UKismetStringLibrary::EqualEqual_StrStr(MyString, "abcd_1234");
	//拼接字符串
	FString MyString1 = MyString + TEXT("efgh_5678");
	FString MyString2 = UKismetStringLibrary::Concat_StrStr(MyString, TEXT("efgh_5678"));
	//查看长度
	int32 MyStringLength1 = MyString.Len();
	int32 MyStringLength2 = UKismetStringLibrary::Len(MyString);//字符串函数库方法
	//判断为空
	bool isNull1 = MyString.IsEmpty();
	bool isNull2 = UKismetStringLibrary::IsEmpty(MyString);
	//获取子字符串
	FString MySubString1 = MyString.Mid(0, 4);
	FString MySubString2 = UKismetStringLibrary::GetSubstring(MyString, 0, 4);
	//获取子字符串的长度
	int32 MySubStringLength1 = MySubString1.Len();
	int32 MySubStringLength2 = UKismetStringLibrary::Len(MySubString2);
	//
}

