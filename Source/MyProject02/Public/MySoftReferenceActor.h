// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MySoftReferenceActor.generated.h"

UCLASS()
class MYPROJECT02_API AMySoftReferenceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	//…Ë÷√ƒ¨»œ÷µ
	AMySoftReferenceActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	FSoftObjectPath MySoftObjectPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	FSoftClassPath MySoftClassPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	TSoftObjectPtr<AActor> MyTSoftObjectPtr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	TSoftClassPtr<AActor> MyTSoftClassPtr;
	
};
