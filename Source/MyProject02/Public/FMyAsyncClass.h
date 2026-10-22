// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Async/AsyncWork.h"

class MYPROJECT02_API FMyAsyncClass : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FMyAsyncClass>;//友元类
public:
	FMyAsyncClass();
	~FMyAsyncClass();
public:
	// 这个函数是异步线程执行的函数
	void DoWork();
	// 这个函数是主线程执行的函数
	FORCEINLINE TStatId GetStatId() const
	{
		//第一个参数是统计ID，第二个参数是统计组
		RETURN_QUICK_DECLARE_CYCLE_STAT(FMyAsyncClass, STATGROUP_ThreadPoolAsyncTasks);
	}
	FORCEINLINE static const TCHAR* GetStatName()
	{
		return TEXT("FMyAsyncClass");
	}
};
