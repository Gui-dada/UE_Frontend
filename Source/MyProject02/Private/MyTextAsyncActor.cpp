// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTextAsyncActor.h"
#include "FMyAsyncClass.h"
#include "Async/Async.h"
// Sets default values
AMyTextAsyncActor::AMyTextAsyncActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyTextAsyncActor::BeginPlay()
{
	Super::BeginPlay();
	// 创建一个异步任务
	//FAutoDeleteAsyncTask<FMyAsyncClass>* MyAsyncTask = new FAutoDeleteAsyncTask<FMyAsyncClass>();
	// 启动异步任务 线程池
	//MyAsyncTask->StartBackgroundTask();
	//启动异步任务 同步线程
	//MyAsyncTask->StartSynchronousTask();
	// 你可以在这里执行一些其他的操作，比如更新UI等


	Async(EAsyncExecution::LargeThreadPool,[]()
	{// 这里是异步线程执行的代码
		// 你可以在这里执行一些耗时的操作，比如加载资源、计算数据等
		// 注意：这个函数是在异步线程中执行的，所以不能直接访问主线程的UI元素
		// 你可以使用UE4提供的线程安全的方法来更新UI，比如使用AsyncTask或者FGraphEventRef等
		UE_LOG(LogTemp, Warning, TEXT("Async task is running......"));
	});
}
	

// Called every frame
void AMyTextAsyncActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

