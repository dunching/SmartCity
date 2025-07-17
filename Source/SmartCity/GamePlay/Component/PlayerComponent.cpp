
#include "PlayerComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActions.h"
#include "Kismet/KismetMathLibrary.h"

FName UPlayerComponent::ComponentName = TEXT("PlayerComponent");

void UPlayerComponent::PossessedBy(
	APlayerController* NewController
	)
{
	if (auto Subsystem = ULocalPlayer::GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>(NewController->GetLocalPlayer()))
	{
		InputActionsPtr = NewObject<UInputActions>();
		InputActionsPtr->InitialInputMapping();

		Subsystem->AddMappingContext(
									 InputActionsPtr->InputMappingContext,
									 0
									);
	}
}

void UPlayerComponent::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent
	)
{
	return;
	
	// 根据设置绑定
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<
		UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(
										   InputActionsPtr->MoveForwardActionPtr,
										   ETriggerEvent::Triggered,
										   this,
										   &ThisClass::MoveForward
										  );
		EnhancedInputComponent->BindAction(
										   InputActionsPtr->MoveRightActionPtr,
										   ETriggerEvent::Triggered,
										   this,
										   &ThisClass::MoveRight
										  );

		EnhancedInputComponent->BindAction(
										   InputActionsPtr->AddPitchActionPtr,
										   ETriggerEvent::Triggered,
										   this,
										   &ThisClass::AddPitchInput
										  );
		EnhancedInputComponent->BindAction(
										   InputActionsPtr->AddYawActionPtr,
										   ETriggerEvent::Triggered,
										   this,
										   &ThisClass::AddYawInput
										  );
	}
}

void UPlayerComponent::AddYawInput(
	const FInputActionValue& InputActionValue
	)
{
	const auto Value = InputActionValue.Get<float>();

	auto OnwerActorPtr = GetOwner<FOwnerType>();

	OnwerActorPtr->AddControllerYawInput(Value);
}

void UPlayerComponent::AddPitchInput(
	const FInputActionValue& InputActionValue
	)
{
	const auto Value = InputActionValue.Get<float>();

	auto OnwerActorPtr = GetOwner<FOwnerType>();

	OnwerActorPtr->AddControllerPitchInput(Value);
}

void UPlayerComponent::MoveRight(
	const FInputActionValue& InputActionValue
	)
{
	const auto Value = InputActionValue.Get<float>();

	auto OnwerActorPtr = GetOwner<FOwnerType>();

	if (OnwerActorPtr->Controller != nullptr)
	{
		const FRotator Rotation = OnwerActorPtr->Controller->GetControlRotation();

		const FVector RightDirection = Rotation.Quaternion().GetRightVector();

		OnwerActorPtr->AddMovementInput(RightDirection, Value);
	}
}

void UPlayerComponent::MoveForward(
	const FInputActionValue& InputActionValue
	)
{
	const auto Value = InputActionValue.Get<float>();

	auto OnwerActorPtr = GetOwner<FOwnerType>();

	if (OnwerActorPtr->Controller != nullptr)
	{
		const FRotator Rotation = OnwerActorPtr->Controller->GetControlRotation();

		const FVector ForwardDirection =
			UKismetMathLibrary::MakeRotFromZX(
											  -OnwerActorPtr->GetGravityDirection(),
											  Rotation.Quaternion().GetForwardVector()
											 ).Vector();

		OnwerActorPtr->AddMovementInput(ForwardDirection, Value);
	}
}
