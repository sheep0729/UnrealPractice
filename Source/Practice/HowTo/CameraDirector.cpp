#include "CameraDirector.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ACameraDirector::ACameraDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACameraDirector::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
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

