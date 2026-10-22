// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bCanEverTick = true;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));//创建弹簧臂
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));//创建相机
	MyWidgetHealth = CreateDefaultSubobject<UWidgetComponent>(TEXT("MyWidgetHealth"));//创建小部件组件
	MyWidgetHealth->SetupAttachment(RootComponent);//设置小部件组件附加到根组件
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetAsset(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UMGWidget.UMGWidget_C'"));//查找小部件蓝图
	if (WidgetAsset.Succeeded()) {
		MyWidgetHealth->SetWidgetClass(WidgetAsset.Class);//设置小部件类
	}
	MyWidgetHealth->SetRelativeLocation(FVector(0, 0, 100));//设置小部件位置
	MyWidgetHealth->SetWidgetSpace(EWidgetSpace::Screen);//设置小部件空间
	MyWidgetHealth->SetDrawSize(FVector2D(400, 20));//设置小部件大小
	SpringArm->TargetArmLength = 400;//设置相机距离
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);//设置相机附加到弹簧臂
	SpringArm->SetupAttachment(RootComponent);//设置弹簧臂附加到根组件
	SpringArm->bUsePawnControlRotation = true;//设置弹簧臂使用Pawn控制旋转

	bUseControllerRotationPitch = false;//设置控制器俯仰
	bUseControllerRotationRoll = false;//设置控制器滚动
	bUseControllerRotationYaw = false;//设置控制器偏航

	GetCharacterMovement()->bOrientRotationToMovement = true;//设置角色移动组件旋转到移动方向
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();//调用父类的BeginPlay函数
	if (Controller != nullptr)
	{
		if (APlayerController* pcp = Cast<APlayerController>(Controller))
		{
			if (UEnhancedInputLocalPlayerSubsystem* localsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pcp->GetLocalPlayer()))
			{
				localsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
	Attack();//攻击
	CalculateBleed();//计算流血
	//设置定时器 (句柄 ,对象 ,引用绑定的函数,循环时间,是否打印)
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AMyCharacter::TimerFunction, 1.0f, true);
	//消除定时器
	if (TimerHandle.IsValid()) {
		GetWorldTimerManager().ClearTimer(TimerHandle);
	}
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	StartLocation = Camera->GetComponentLocation();//获取相机位置
	ForwardVector = Camera->GetForwardVector();//获取相机前向量
	EndLocation = StartLocation + ForwardVector * 10000;//获取结束位置
	//根据通道检测返回的结果
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility);//射线检测
	if(bHit)
	{
		AActor* HitActor = HitResult.GetActor();//获取碰撞的actor
		FVector hitpoint = HitResult.ImpactPoint;//获取碰撞位置
		FVector HitLocation = HitResult.ImpactPoint;//获取碰撞位置
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("LineTraceSingleByChannel------*HitActor->GetName : %s"),*HitActor->GetName()));

	}

	//根据对象检测返回的结果
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	bool bHit2 = GetWorld()->LineTraceSingleByObjectType(HitResult, StartLocation, EndLocation, ObjectQueryParams);//射线检测
	if (bHit2)
	{
		AActor* HitActor2 = HitResult.GetActor();//获取碰撞的actor
		FVector hitpoint2 = HitResult.ImpactPoint;//获取碰撞位置
		FVector HitLocation2 = HitResult.ImpactPoint;//获取碰撞位置
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("LineTraceSingleByObjectType------*HitActor->GetName : %s"), *HitActor2->GetName()));
	}

	bool HitRetMulti = GetWorld()->LineTraceMultiByChannel(HitResults, StartLocation, EndLocation, ECC_Visibility);//射线检测
	if (HitRetMulti)
	{
		for (int32 i = 0; i < HitResults.Num(); i++)
		{
			AActor* HitActor3 = HitResults[i].GetActor();//获取碰撞的actor
			FVector hitpoint3 = HitResults[i].ImpactPoint;//获取碰撞位置
			FVector HitLocation3 = HitResults[i].ImpactPoint;//获取碰撞位置
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("LineTraceMultiByChannel------*HitActor->GetName : %s"), *HitActor3->GetName()));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("LineTraceMultiByChannel------No Hit")));
	}

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))//强制转换
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);//绑定移动
		//EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);//绑定移动
		//EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);//绑定查看
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);//绑定查看
	}
}

void AMyCharacter::Move(const FInputActionValue& value)
{
	FVector2D MoveMent = value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		const FRotator ControlRot = Controller->GetControlRotation();//获取控制器的旋转
		const FRotator ControlYawRot(0, ControlRot.Yaw, 0);//获取控制器的偏航旋转
		const FVector ForwardDirection = FRotationMatrix(ControlYawRot).GetUnitAxis(EAxis::X);//获取前后
		const FVector RightForward = FRotationMatrix(ControlYawRot).GetUnitAxis(EAxis::Y);//获取左右

		//AddMovementInput(Direction, MoveMent.X);
		//AddMovementInput(Direction, MoveMent.Y);
		AddMovementInput(RightForward, MoveMent.X);//添加前后移动输入
		AddMovementInput(ForwardDirection, MoveMent.Y);//添加左右移动输入

	}
}

void AMyCharacter::Look(const FInputActionValue& value)
{

	FVector2D LookAxisVector = value.Get<FVector2D>();
	if (Controller != nullptr) {
		AddControllerYawInput(LookAxisVector.X);//添加控制器x轴输入
		AddControllerPitchInput(LookAxisVector.Y);//添加控制器y轴输入
	}
}

void AMyCharacter::Attack()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT(" void AMyCharacter::Attack() ")));//添加屏幕调试信息
}

void AMyCharacter::CalculateBleed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT(" void AMyCharacter::CalculateBleed() ")));//添加屏幕调试信息
}

void AMyCharacter::TimerFunction()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT(" void AMyCharacter::TimerFunction() ")));//添加屏幕调试信息
}

