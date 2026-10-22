// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyMaterialActor.generated.h"

UCLASS()
class MYPROJECT02_API AMyMaterialActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyMaterialActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="MySceneComponent")
	class USceneComponent* MyScene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MySceneComponent")
	class UStaticMeshComponent * MyMesh;
	UMaterialInstanceDynamic* MyMaterialInstance;//动态材质实例
	UFUNCTION()
	void DelayFunction(float DelayTime);//延迟函数

};
