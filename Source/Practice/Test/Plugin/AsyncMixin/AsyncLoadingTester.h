#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AsyncLoadingTester.generated.h"

UCLASS()
class PRACTICE_API AAsyncLoadingTester : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSoftObjectPtr<USoundWave> Sound;
};
