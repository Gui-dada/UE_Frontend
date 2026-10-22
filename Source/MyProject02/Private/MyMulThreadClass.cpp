// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMulThreadClass.h"
#include "MyGameInstanceSubsystem.h"
FMyMulThreadClass::FMyMulThreadClass()
{
	UE_LOG(LogTemp, Warning, TEXT("FMyMulThreadClass constructor called"));
}

FMyMulThreadClass::~FMyMulThreadClass()
{
	UE_LOG(LogTemp, Warning, TEXT("FMyMulThreadClass destructor called"));
}

bool FMyMulThreadClass::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("Initiallise Finished "));
	return true;
}

uint32 FMyMulThreadClass::Run()
{
	bRunThread = true;
	while (bRunThread)
	{
		// Do some work here
		
		UMyGameInstanceSubsystem* ins = UMyGameInstanceSubsystem::GetMyGameInstance();
		FPlatformProcess::Sleep(3.0f); // Sleep for 1 second
		if (ins) {
			UE_LOG(LogTemp, Warning, TEXT("Thread is running"));
		}
	}
	return uint32();
}

void FMyMulThreadClass::Stop()
{
	bRunThread = false;
}
