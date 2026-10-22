// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"//添加可运行类头文件
#include "HAL/ThreadSafeBool.h"//添加线程安全布尔值头文件


/**
 *
 */
class MYPROJECT02_API FMyMulThreadClass : public FRunnable
{
public:
	FMyMulThreadClass();
	FMyMulThreadClass(FString threadername)
		:ThreadName(threadername) {}; //构造函数
	~FMyMulThreadClass();
	virtual bool   Init() override; //初始化函数
	virtual uint32 Run()  override; //运行函数
	virtual void   Stop() override; //停止函数
	FString ThreadName; //线程名称
	bool bRunThread ; //线程运行标志
	FCriticalSection ThreadLock; //线程锁
};
