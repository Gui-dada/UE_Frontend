// Fill out your copyright notice in the Description page of Project Settings.

#include "MyTimelineActor.h"

// Sets default values
AMyTimelineActor::AMyTimelineActor()
{
		// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MyTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("MyTimelineComponent"));//初始化时间轴组件
	MySceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MySceneComponent"));//初始化场景组件
	MyStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyStaticMeshComponent"));//初始化静态网格组件
	MyBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("MyBoxComponent1"));//初始化盒子组件
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MyStaticMeshObject(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Architecture/Wall_400x400.Wall_400x400'"));//初始化浮点曲线
	if (MyStaticMeshObject.Succeeded()) {
		MyStaticMeshComponent->SetStaticMesh(MyStaticMeshObject.Object);//设置静态网格
	}
	RootComponent = MySceneComponent;//设置根组件
	MyStaticMeshComponent->SetupAttachment(RootComponent);//设置静态网格组件
	MyBoxComponent->SetupAttachment(RootComponent);//设置盒子组件
	MyBoxComponent->SetBoxExtent(FVector(200,100,100));//设置盒子大小
	MyBoxComponent->SetRelativeLocation(FVector(200, 0, 0));//设置盒子位置
}

// Called when the game starts or when spawned
void AMyTimelineActor::BeginPlay()
{
	Super::BeginPlay();
	MyTimelineStartDelegate.BindUFunction(this, FName("TimelineStart"));//绑定开始
	MyTimelineFinishDelegate.BindUFunction(this, FName("TimelineFinish"));//绑定结束	
	MyTimelineComponent->AddInterpFloat(MyCurveFloat, MyTimelineStartDelegate);//添加插值浮点
	//MyTimelineComponent->SetLooping(false);//设置循环
	//MyTimelineComponent->PlayFromStart();//从头开始
	//MyTimelineComponent->Play();
	MyTimelineComponent->SetTimelineFinishedFunc(MyTimelineFinishDelegate);//设置时间轴结束函数
	//设置碰撞绑定

	MyBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AMyTimelineActor::BeginOverlapFunc);//开始碰撞
	MyBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AMyTimelineActor::EndOverlapFunc);//结束碰撞


}

// Called every frame
void AMyTimelineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyTimelineActor::TimelineStart(float value)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT(" void AMyTimelineActor::TimelineStart() ")));//添加屏幕调试信息)
	//设置旋转
	float YawRotation = FMath::Lerp(0.0f, 90.0f, value);
	FRotator NewRotation = FRotator(0, YawRotation, 0);//设置旋转
	MyStaticMeshComponent->SetRelativeRotation(NewRotation);//设置旋转
}

void AMyTimelineActor::TimelineFinish()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("void AMyTimelineActor::TimelineFinish()")));//添加屏幕调试信息
}

void AMyTimelineActor::BeginOverlapFunc(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	AMyCharacter* TempMyCharacter = Cast<AMyCharacter>(OtherActor);
	if (TempMyCharacter != nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("void AMyTimelineActor::BeginOverlapFunc()")));//添加屏幕调试信息
		MyTimelineComponent->PlayFromStart();//从头开始
	}
}
void AMyTimelineActor::EndOverlapFunc(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMyCharacter* TempMyCharacter = Cast<AMyCharacter>(OtherActor);
	if (TempMyCharacter != nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("void AMyTimelineActor::BeginOverlapFunc()")));//添加屏幕调试信息
		MyTimelineComponent->ReverseFromEnd();//反向播放
	}
}
