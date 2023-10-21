#include "CameraDirector.h"

#include "Kismet/GameplayStatics.h"

ACameraDirector::ACameraDirector()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ACameraDirector::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACameraDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeToNextCameraChange -= DeltaTime;
	if (TimeToNextCameraChange <= 0.0f)
	{
		constexpr float TimeBetweenCameraChanges = 2.0f;
		TimeToNextCameraChange += TimeBetweenCameraChanges;

		// Find the actor that handles control for the local player.
		if (APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0))
		{
			if (OurPlayerController->GetViewTarget() != CameraOne && CameraOne != nullptr)
			{
				// Cut instantly to camera one.
				OurPlayerController->SetViewTarget(CameraOne);
			}
			else if (OurPlayerController->GetViewTarget() != CameraTwo && CameraTwo != nullptr)
			{
				constexpr float SmoothBlendTime = 0.75f;
				// Blend smoothly to camera two.
				OurPlayerController->SetViewTargetWithBlend(CameraTwo, SmoothBlendTime);
			}
		}
	}
}

