// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"

/**
 *
 */
UCLASS()
class MYPROJECT02_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyUserWidget")
	float CurrentHealth = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyUserWidget")
	float MaxHealth = 100.0f;
};
