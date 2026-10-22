// Fill out your copyright notice in the Description page of Project Settings.


#include "FMyAsyncClass.h"

FMyAsyncClass::FMyAsyncClass()
{
}

FMyAsyncClass::~FMyAsyncClass()
{
	UE_LOG(LogTemp, Warning, TEXT("FMyAsyncTask End......"));
}

void FMyAsyncClass::DoWork()
{
	// 这里是异步线程执行的代码
	// 你可以在这里执行一些耗时的操作，比如加载资源、计算数据等
	// 注意：这个函数是在异步线程中执行的，所以不能直接访问主线程的UI元素
	// 你可以使用UE4提供的线程安全的方法来更新UI，比如使用AsyncTask或者FGraphEventRef等
	UE_LOG(LogTemp, Warning, TEXT("FMyAsyncClass::DoWork() is running......"));

}


