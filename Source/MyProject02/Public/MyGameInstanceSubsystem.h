// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MyMulThreadClass.h"
#include "MyGameInstanceSubsystem.generated.h"

/**
 *
 */
UCLASS()
class MYPROJECT02_API UMyGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//创建单个实例时，使用static变量来保存实例的指针
	static UMyGameInstanceSubsystem* MyGameInstance; //全局调用需要static
	UFUNCTION(BlueprintCallable, Category = "MyGameInstanceSubsystem")
	static UMyGameInstanceSubsystem* GetMyGameInstance();
	UFUNCTION(BlueprintCallable, Category = "MyGameInstanceSubsystem")
	void startThread();
	UFUNCTION(BlueprintCallable, Category = "MyGameInstanceSubsystem")
	void stopThread();
private:
	//共享指针
	TSharedPtr<FMyMulThreadClass> MyThreadPtr; //线程类指针
	//FMyMulThreadClass* MyThread; //线程类
	FRunnableThread* Thread; //线程
	//FThreadSafeBool bRunThread; //线程安全布尔值

};
