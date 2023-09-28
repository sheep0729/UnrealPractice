 #pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlendTriggerVolume.generated.h"

UCLASS()
class ABlendTriggerVolume : public AActor
{
	GENERATED_BODY()

public:
	ABlendTriggerVolume();

protected:
	// Collision Bounds of the Actor Volume
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* OverlapVolume;

	// Camera Actor which the Actor Volume blends to
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ACameraActor> CameraToFind;

	// Blend time for camera transition
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta =(ClampMin = 0.0f ))
	float CameraBlendTime;

	//Begin and End Overlap Actor methods for our OverlapVolume Actor.
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
};