// Fill out your copyright notice in the Description page of Project Settings.


#include "MySoftReferenceActor.h"
#include "Engine/AssetManager.h"

// Sets default values
AMySoftReferenceActor::AMySoftReferenceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AMySoftReferenceActor::BeginPlay()
{
	
	Super::BeginPlay();//�̳и���BeginPlay����
	//ͬ��������Դ
	FSoftObjectPath Path1(TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/T_Brick_Clay_Beveled_D.T_Brick_Clay_Beveled_D'"));
	TSharedPtr<FStreamableHandle> SycStreamHandle1 = UAssetManager::GetStreamableManager().RequestSyncLoad(Path1);//����ͬ��������Դ
	if (SycStreamHandle1.IsValid())
	{
		SycStreamHandle1->WaitUntilComplete();//�ȴ��������
		UTexture2D* Texture = Cast<UTexture2D>(SycStreamHandle1->GetLoadedAsset());//�������Դ
		if (Texture)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("SycStreamHandle1 : Texture->GetName : %s"), *Texture->GetName()));//������Ļ������Ϣ
		}
		//�ͷ���Դ
		SycStreamHandle1->ReleaseHandle();
	}
	//�첽������Դ
	FSoftObjectPath Path2(TEXT("/Script/Engine.Texture2D'/Game/StarterContent/Textures/T_Brick_Clay_Beveled_M.T_Brick_Clay_Beveled_M'"));
	TSharedPtr<FStreamableHandle> AsycStreamHandle1 = UAssetManager::GetStreamableManager().RequestAsyncLoad(Path2);//�����첽������Դ
	if (AsycStreamHandle1.IsValid())
	{
		AsycStreamHandle1->WaitUntilComplete();//�ȴ��������
		UTexture2D* Texture = Cast<UTexture2D>(SycStreamHandle1->GetLoadedAsset());//�������Դ
		if (Texture)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("AsycStreamHandle1 : Texture->GetName : %s"), *Texture->GetName()));//������Ļ������Ϣ
		}
		//�ͷ���Դ
		AsycStreamHandle1->ReleaseHandle();
	}
}

// Called every frame
void AMySoftReferenceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);//�̳и���Tick����

}

