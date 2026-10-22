// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyFuncActor.generated.h"

UCLASS()
class MYPROJECT02_API AMyFuncActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyFuncActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, Category = "MyFuncActor")
	void MyOpenLevel();
	UFUNCTION(BlueprintCallable, Category = "MyFuncActor")
	void MyCurrentLevel();
	UFUNCTION(BlueprintCallable, Category = "MyFuncActor")
	void MyQuitLevel();
};
