#include "AsyncLoadingTester.h"

#include "AsyncMixin.h"
#include "AsyncMixin.h"
#include "Kismet/GameplayStatics.h"


void AAsyncLoadingTester::BeginPlay()
{
	Super::BeginPlay();

	FAsyncScope AsyncScope;
	AsyncScope.AsyncLoad(Sound, [this]
	{
		UGameplayStatics::PlaySound2D(this, Sound.Get());
	});
	AsyncScope.StartAsyncLoading();
}
