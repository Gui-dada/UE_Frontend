// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMaterialActor.h"
#include "Kismet/KismetSystemLibrary.h"
// Sets default values
AMyMaterialActor::AMyMaterialActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MyScene = CreateDefaultSubobject<USceneComponent>(TEXT("MyScene"));//创建一个场景组件
	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyMesh"));//创建一个静态网格组件
	RootComponent = MyScene;//讲MyScene设置为根组件
	MyMesh->SetupAttachment(MyScene);//将MyMesh附加到MyScene
	//静态加载网格
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MyMeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Shapes/Shape_Wedge_B.Shape_Wedge_B'"));
	if (MyMeshAsset.Succeeded())
	{
		MyMesh->SetStaticMesh(MyMeshAsset.Object);//设置静态网格
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load mesh!"));//加载失败
	}
}

// Called when the game starts or when spawned
void AMyMaterialActor::BeginPlay()
{
	Super::BeginPlay();
	//动态加载材质
	UMaterialInterface* MyMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Script/Engine.Material'/Game/TextMaterial.TextMaterial'"));
	if (MyMaterial)
	{
		MyMaterialInstance = MyMesh->CreateDynamicMaterialInstance(0, MyMaterial);//创建动态材质实例
		MyMaterialInstance->SetVectorParameterValue("BaseColor", FLinearColor::Red);//设置材质参数
		MyMaterialInstance->SetScalarParameterValue("BaseMetallic", 1);//设置材质参数
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load material!"));//加载失败
	}
	//实现delay函数
	const FLatentActionInfo LatentInfo(0,FMath::Rand(),TEXT("DelayFunction"), this);
	UKismetSystemLibrary::Delay(this, 4.0f, LatentInfo);//延迟2秒
}

// Called every frame
void AMyMaterialActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyMaterialActor::DelayFunction(float DelayTime)
{
	//延迟DelayTime秒打印数据
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("DelayFunction called!"));//打印数据
	//延迟DelayTime秒
	//FTimerHandle TimerHandle;
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle, [=]()
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("DelayFunction finished!"));
	//	}, DelayTime, false);
	//延迟完成后调用LatentInfo
	
}

