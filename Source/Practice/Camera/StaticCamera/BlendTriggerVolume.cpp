#include "BlendTriggerVolume.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraActor.h"
#include "Template/TP_FirstPerson/TP_FirstPersonCharacter.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Template/TP_ThirdPerson/TP_ThirdPersonCharacter.h"

// Sets default values
ABlendTriggerVolume::ABlendTriggerVolume()
{
	//Create box component default components
	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("CameraProximityVolume"));

	//Set the box component attachment to the root component.
	OverlapVolume->SetupAttachment(RootComponent);

	//Set the CameraBlendTime
	CameraBlendTime = 1.0f;
}

void ABlendTriggerVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// Cast check to see if overlapped Actor is Third Person Player Character
	if (const ATP_ThirdPersonCharacter* PlayerCharacterCheck = Cast<ATP_ThirdPersonCharacter>(OtherActor))
	{
		// Cast to Player Character's PlayerController
		if (APlayerController* PlayerCharacterController = Cast<APlayerController>(PlayerCharacterCheck->GetController()))
		{
			// Array to contain found Camera Actors
			TArray<AActor*> FoundActors;

			// Utility function to populate array with all Camera Actors in the level
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), CameraToFind, FoundActors);

			// Sets Player Controller view to the first CameraActor found
			PlayerCharacterController->SetViewTargetWithBlend(FoundActors[0], CameraBlendTime, EViewTargetBlendFunction::VTBlend_Linear);

			// 调整 PlayerController 的 Yaw
			auto ControlRotation = PlayerCharacterController->GetControlRotation();
			ControlRotation.Yaw = FoundActors[0]->GetActorRotation().Yaw;
			PlayerCharacterController->SetControlRotation(ControlRotation);
		}
	}
}

void ABlendTriggerVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	if (const ATP_ThirdPersonCharacter* PlayerCharacterCheck = Cast<ATP_ThirdPersonCharacter>(OtherActor))
	{
		if (APlayerController* PlayerCharacterController = Cast<APlayerController>(PlayerCharacterCheck->GetController()))
		{
			// Blend to Player Character's Camera Component.
			PlayerCharacterController->SetViewTargetWithBlend(PlayerCharacterController->GetPawn(), CameraBlendTime,
			                                                  EViewTargetBlendFunction::VTBlend_Linear);
		}
	}
}