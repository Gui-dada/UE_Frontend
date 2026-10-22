// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstanceSubsystem.h"

UMyGameInstanceSubsystem* UMyGameInstanceSubsystem::MyGameInstance = nullptr;
bool UMyGameInstanceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UMyGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	MyGameInstance = this;
	UE_LOG(LogTemp, Warning, TEXT("MyGameInstanceSubsystem Initialized"));
}

void UMyGameInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Warning, TEXT("MyGameInstanceSubsystem Deinitialized"));
}
UMyGameInstanceSubsystem* UMyGameInstanceSubsystem::GetMyGameInstance()
{
	return MyGameInstance;
}

void UMyGameInstanceSubsystem::startThread()
{
	// 确保字符串转换为 TCHAR 类型  
	MyThreadPtr = MakeShared<FMyMulThreadClass>(TEXT("MyThreadPtr"));//text中的内容会被转换为TCHAR类型

	FString ThreadName = MyThreadPtr->ThreadName;
	Thread = FRunnableThread::Create(MyThreadPtr.Get(), *ThreadName);
	UE_LOG(LogTemp, Warning, TEXT("UMyGameInstanceSubsystem::startThread()"));

}

void UMyGameInstanceSubsystem::stopThread()
{
	if (MyThreadPtr.IsValid()) {
		MyThreadPtr->Stop();
	}
	if (Thread) {
		Thread->WaitForCompletion();//等待线程完成
		delete Thread;//删除线程
		Thread = nullptr;//置空线程指针
	}
	UE_LOG(LogTemp, Warning, TEXT("UMyGameInstanceSubsystem::stopThread() "));
}
