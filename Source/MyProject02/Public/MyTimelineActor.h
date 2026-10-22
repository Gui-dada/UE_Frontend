// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/TimelineComponent.h"//timeline 头文件
#include "Components/BoxComponent.h"
#include "MyCharacter.h"
#include "GameFramework/Actor.h"
#include "MyTimelineActor.generated.h"

UCLASS()
class MYPROJECT02_API AMyTimelineActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyTimelineActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCurve")
	UCurveFloat* MyCurveFloat;//浮点曲线	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySceneComponent")
	UTimelineComponent* MyTimelineComponent;//时间轴组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySceneComponent")
	USceneComponent* MySceneComponent;//场景组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySceneComponent")
	UStaticMeshComponent* MyStaticMeshComponent;//静态网格组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySceneComponent")
	UBoxComponent* MyBoxComponent;//盒子组件

	FOnTimelineFloat MyTimelineStartDelegate;//时间轴浮点返回代理
	FOnTimelineEvent MyTimelineFinishDelegate;//时间轴事件返回代理

	UFUNCTION()
	void TimelineStart(float value);//时间轴开始
	//UFUNCTION()
	//void TimelineUpdate(float value);//时间轴更新
	UFUNCTION()
	void TimelineFinish();//时间轴结束
	//碰撞绑定函数
	UFUNCTION()
	void BeginOverlapFunc(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,const FHitResult& SweepResult);
	UFUNCTION()
	void EndOverlapFunc(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
