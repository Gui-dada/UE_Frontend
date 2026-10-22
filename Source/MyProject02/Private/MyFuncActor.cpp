// Fill out your copyright notice in the Description page of Project Settings.


#include "MyFuncActor.h"
#include <Kismet/GameplayStatics.h>
// Sets default values
AMyFuncActor::AMyFuncActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyFuncActor::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), OutActors);//获取所有的Actor
	for (AActor* Actor : OutActors)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor Name : %s"), *Actor->GetName());//打印Actor的名字
	}
}

// Called every frame
void AMyFuncActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyFuncActor::MyOpenLevel()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("ThirdPersonMap"));
}

void AMyFuncActor::MyCurrentLevel()
{
	FString MyGameCurrentName= UGameplayStatics::GetCurrentLevelName(GetWorld());
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Current Level Name : %s"), *MyGameCurrentName));

}

void AMyFuncActor::MyQuitLevel()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->ConsoleCommand("quit");
}

