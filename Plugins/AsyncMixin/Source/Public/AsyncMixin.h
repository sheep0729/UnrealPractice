// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Ticker.h"
#include "UObject/SoftObjectPtr.h"

class FAsyncCondition;
class FName;
class UPrimaryDataAsset;
struct FPrimaryAssetId;
struct FStreamableHandle;
template <class TClass>
class TSubclassOf;

DECLARE_DELEGATE_OneParam(FStreamableHandleDelegate, TSharedPtr<FStreamableHandle>)

//TODO I think we need to introduce a retention policy, preloads automatically stay in memory until canceled
//     but what if you want to preload individual items just using the AsyncLoad functions?  I don't want to
//     introduce individual policies per call, or introduce a whole set of preload vs asyncloads, so would
//     would rather have a retention policy.  Should it be a member and actually create real memory when
//     you inherit from AsyncMixin, or should it be a template argument?
//enum class EAsyncMixinRetentionPolicy : uint8
//{
//	Default,
//	KeepResidentUntilComplete,
//	KeepResidentUntilCancel
//};
//TODO 我认为我们需要引入一个保留策略，预加载会自动保留在内存中，直到取消
//     但是，如果您只想使用 AsyncLoad 函数预加载单个项目怎么办？我不想
//     对每个调用引入单独的策略，也不想引入一整套预加载与异步加载，因此
//     我宁愿有一个保留策略。它应该是一个成员并在继承 AsyncMixin 时实际创建真实内存，
//     还是应该是一个模板参数？
//enum class EAsyncMixinRetentionPolicy : uint8
//{
//	Default,
//	KeepResidentUntilComplete,
//	KeepResidentUntilCancel
//};


/**
 * The FAsyncMixin allows easier management of async loading requests, to ensure linear request handling, to make 
 * writing code much easier.  The usage pattern is as follows,
 *
 * First - inherit from FAsyncMixin, even if you're a UObject, you can also inherit from FAsyncMixin.
 *
 * Then - you can make your async loads as follows.
 * 
 * CancelAsyncLoading();			// Some objects get reused like in lists, so it's important to cancel anything you had pending doesn't complete.
 * AsyncLoad(ItemOne, CallbackOne);
 * AsyncLoad(ItemTwo, CallbackTwo);
 * StartAsyncLoading();
 * 
 * You can also include the 'this' scope safely, one of the benefits of the mix-in, is that none of the callbacks
 * are ever out of scope of the host AsyncMixin derived object.
 * e.g.
 * AsyncLoad(SomeSoftObjectPtr, [this, ...]() {
 *    
 * });
 * 
 *
 * What will happen is first we cancel any existing one(s), e.g. perhaps we are a widget that just got told to represent
 * some new thing.  What will happen is we'll Load ItemOne and ItemTwo, *THEN* we'll call the callbacks in the order you
 * requested the async loads - even if ItemOne or ItemTwo was already loaded when you request it.
 *
 * When all the async loading requests complete, OnFinishedLoading will be called.
 * 
 * If you forget to call StartAsyncLoading(), we'll call it next frame, but you should remember to call it
 * when you're done with your setup, as maybe everything is already loaded, and it will avoid a single frame
 * of a loading indicator flash, which is annoying.
 * 
 * NOTE: The FAsyncMixin also makes it safe to pass [this] as a captured input into your lambda, because it handles 
 * unhooking everything if either your owner class is destroyed, or you cancel everything.
 *
 * NOTE: FAsyncMixin doesn't add any additional memory to your class.  Several classes currently handling async loading 
 * internally allocate TSharedPtr<FStreamableHandle> members and tend to hold onto SoftObjectPaths temporary state.  The 
 * FAsyncMixin does all of this internally with a static TMap so that all of the async request memory is stored temporarily
 * and sparsely.
 * 
 * NOTE: For debugging and understanding what's going on, you should add -LogCmds="LogAsyncMixin Verbose" to the command line.
 */
/**
 * FAsyncMixin 使异步加载请求的管理更加容易，以确保线性请求处理，使编写代码更加容易。使用模式如下：
 *
 * 首先，即使您是 UObject，也应从 FAsyncMixin 继承，您也可以从 FAsyncMixin 继承。
 *
 * 然后，您可以按以下方式进行异步加载。
 * 
 * CancelAsyncLoading();			// 有些对象会像在列表中一样被重复使用，因此取消任何未完成的挂起操作非常重要。
 * AsyncLoad(ItemOne, CallbackOne);
 * AsyncLoad(ItemTwo, CallbackTwo);
 * StartAsyncLoading();
 * 
 * 您还可以安全地包含“this”作用域，混合的一个好处是，没有一个回调函数会超出主 AsyncMixin 派生对象的范围。
 * 例如：
 * AsyncLoad(SomeSoftObjectPtr, this, ... {
 *    
 * });
 * 
 *
 * 首先，我们将取消任何现有的操作，例如，可能是一个小部件，刚刚被告知要表示某个新事物。然后，我们将加载 ItemOne 和 ItemTwo，然后按您请求异步加载的顺序调用回调函数，即使在请求它时 ItemOne 或 ItemTwo 已经加载。
 *
 * 当所有异步加载请求完成时，将调用 OnFinishedLoading。
 * 
 * 如果您忘记调用 StartAsyncLoading()，我们将在下一帧调用它，但您应该在完成设置时记得调用它，因为可能已经加载了所有内容，这将避免加载指示器闪烁一帧，这很烦人。
 * 
 * 注意：FAsyncMixin 还使得将 [this] 作为捕获输入传递到 lambda 中变得安全，因为它处理了所有取消操作。
 *
 * 注意： FAsyncMixin 不会向您的类添加任何额外的内存。目前，几个类在内部处理异步加载时会分配 TSharedPtr<FStreamableHandle> 成员，并倾向于保留 SoftObjectPaths 的临时状态。FAsyncMixin 使用静态 TMap 在内部完成所有这些操作，因此所有异步请求内存都是临时和稀疏存储的。
 * 
 * 请注意，为了调试和了解正在发生的情况，您应该在命令行中添加 -LogCmds="LogAsyncMixin Verbose"。
 */
class ASYNCMIXIN_API FAsyncMixin : public FNoncopyable
{
protected:
	FAsyncMixin();

public:
	virtual ~FAsyncMixin();

protected:
	/** Called when loading starts. */
	virtual void OnStartedLoading()
	{
	}

	/** Called when all loading has finished. */
	virtual void OnFinishedLoading()
	{
	}

protected:
	/** Async load a TSoftClassPtr<T>, call the Callback when complete. */
	/** 异步加载 TSoftClassPtr<T>，在完成时调用 Callback。 */
	template <typename T = UObject>
	void AsyncLoad(TSoftClassPtr<T> SoftClass, TFunction<void()>&& Callback)
	{
		AsyncLoad(SoftClass.ToSoftObjectPath(), FSimpleDelegate::CreateLambda(MoveTemp(Callback)));
	}

	/** Async load a TSoftClassPtr<T>, call the Callback when complete. */
	/** 异步加载 TSoftClassPtr<T>，在完成时调用 Callback。 */
	template <typename T = UObject>
	void AsyncLoad(TSoftClassPtr<T> SoftClass, TFunction<void(TSubclassOf<T>)>&& Callback)
	{
		AsyncLoad(SoftClass.ToSoftObjectPath(),
		          FSimpleDelegate::CreateLambda([SoftClass, UserCallback = MoveTemp(Callback)]() mutable
		          {
			          UserCallback(SoftClass.Get());
		          })
		);
	}

	/** Async load a TSoftClassPtr<T>, call the Callback when complete. */
	/** 异步加载 TSoftClassPtr<T>，在完成时调用 Callback。 */
	template <typename T = UObject>
	void AsyncLoad(TSoftClassPtr<T> SoftClass, const FSimpleDelegate& Callback = FSimpleDelegate())
	{
		AsyncLoad(SoftClass.ToSoftObjectPath(), Callback);
	}

	/** Async load a TSoftObjectPtr<T>, call the Callback when complete. */
	/** 异步加载 TSoftObjectPtr<T>，在完成时调用 Callback。 */
	template <typename T = UObject>
	void AsyncLoad(TSoftObjectPtr<T> SoftObject, TFunction<void()>&& Callback)
	{
		AsyncLoad(SoftObject.ToSoftObjectPath(), FSimpleDelegate::CreateLambda(MoveTemp(Callback)));
	}

	/** Async load a TSoftObjectPtr<T>, call the Callback when complete. */
	/** 异步加载 TSoftObjectPtr<T>，在完成时调用 Callback。 */
	template <typename T = UObject>
	void AsyncLoad(TSoftObjectPtr<T> SoftObject, TFunction<void(T*)>&& Callback)
	{
		AsyncLoad(SoftObject.ToSoftObjectPath(),
		          FSimpleDelegate::CreateLambda([SoftObject, UserCallback = MoveTemp(Callback)]() mutable
		          {
			          UserCallback(SoftObject.Get());
		          })
		);
	}

	/** Async load a TSoftObjectPtr<T>, call the Callback when complete. */
	/** 异步加载 TSoftObjectPtr<T>，在完成时调用 Callback。 */
	template <typename T = UObject>
	void AsyncLoad(TSoftObjectPtr<T> SoftObject, const FSimpleDelegate& Callback = FSimpleDelegate())
	{
		AsyncLoad(SoftObject.ToSoftObjectPath(), Callback);
	}

	/** Async load a FSoftObjectPath, call the Callback when complete. */
	/** 异步加载 FSoftObjectPath，在完成时调用 Callback。 */
	void AsyncLoad(FSoftObjectPath SoftObjectPath, const FSimpleDelegate& Callback = FSimpleDelegate());

	/** Async load an array of FSoftObjectPath, call the Callback when complete. */
	/** 异步加载 FSoftObjectPath 数组，在完成时调用 Callback。 */
	void AsyncLoad(const TArray<FSoftObjectPath>& SoftObjectPaths, TFunction<void()>&& Callback)
	{
		AsyncLoad(SoftObjectPaths, FSimpleDelegate::CreateLambda(MoveTemp(Callback)));
	}

	/** Async load an array of FSoftObjectPath, call the Callback when complete. */
	/** 异步加载 FSoftObjectPath 数组，在完成时调用 Callback。 */
	void AsyncLoad(const TArray<FSoftObjectPath>& SoftObjectPaths, const FSimpleDelegate& Callback = FSimpleDelegate());

	/** Given an array of primary assets, it loads all of the bundles referenced by properties of these assets specified in the LoadBundles array. */
	/** 给定一个主资产数组，它会加载在 LoadBundles 数组中指定的这些资产的属性引用的所有捆绑包。 */
	template <typename T = UPrimaryDataAsset>
	void AsyncPreloadPrimaryAssetsAndBundles(const TArray<T*>& Assets, const TArray<FName>& LoadBundles,
	                                         const FSimpleDelegate& Callback = FSimpleDelegate())
	{
		TArray<FPrimaryAssetId> PrimaryAssetIds;
		for (const T* Item : Assets)
		{
			PrimaryAssetIds.Add(Item);
		}

		AsyncPreloadPrimaryAssetsAndBundles(PrimaryAssetIds, LoadBundles, Callback);
	}

	/** Given an array of primary asset ids, it loads all of the bundles referenced by properties of these assets specified in the LoadBundles array. */
	/** 给定一个主资产 ID 数组，它会加载在 LoadBundles 数组中指定的这些资产的属性引用的所有捆绑包。 */
	void AsyncPreloadPrimaryAssetsAndBundles(const TArray<FPrimaryAssetId>& AssetIds, const TArray<FName>& LoadBundles, TFunction<void()>&& Callback)
	{
		AsyncPreloadPrimaryAssetsAndBundles(AssetIds, LoadBundles, FSimpleDelegate::CreateLambda(MoveTemp(Callback)));
	}

	/** Given an array of primary asset ids, it loads all of the bundles referenced by properties of these assets specified in the LoadBundles array. */
	/** 给定一个主资产 ID 数组，它会加载在 LoadBundles 数组中指定的这些资产的属性引用的所有捆绑包。 */
	void AsyncPreloadPrimaryAssetsAndBundles(const TArray<FPrimaryAssetId>& AssetIds, const TArray<FName>& LoadBundles,
	                                         const FSimpleDelegate& Callback = FSimpleDelegate());

	/** Add a future condition that must be true before we move forward. */
	/** 添加一个必须为真的未来条件，然后我们才能继续。 */
	void AsyncCondition(TSharedRef<FAsyncCondition> Condition, const FSimpleDelegate& Callback = FSimpleDelegate());

	/**
	 * Rather than load anything, this callback is just inserted into the callback sequence so that when async loading 
	 * completes this event will be called at the same point in the sequence.  Super useful if you don't want a step to be
	 * tied to a particular asset in case some of the assets are optional.
	 */
	/**
	 * 不是加载任何内容，此回调函数仅插入到回调序列中，以便在异步加载完成时，在序列中的相同点调用此事件。如果您不想将步骤与特定资产绑定在一起，这将非常有用，因为某些资产是可选的。
	 */
	void AsyncEvent(TFunction<void()>&& Callback)
	{
		AsyncEvent(FSimpleDelegate::CreateLambda(MoveTemp(Callback)));
	}

	/**
	 * Rather than load anything, this callback is just inserted into the callback sequence so that when async loading
	 * completes this event will be called at the same point in the sequence.  Super useful if you don't want a step to be
	 * tied to a particular asset in case some of the assets are optional.
	 */
	/**
	 * 这个回调并不加载任何东西，而是插入到回调序列中，这样当异步加载完成时，
	 * 这个事件将在序列中的相同点被调用。如果你不希望一个步骤绑定到特定的资产，
	 * 特别是当一些资产是可选的，这将非常有用。
	 */
	void AsyncEvent(const FSimpleDelegate& Callback);

	/** Flushes any async loading requests. */
	/** 刷新任何异步加载请求。 */
	void StartAsyncLoading();

	/** Cancels any pending async loads. */
	/** 取消任何待处理的异步加载。 */
	void CancelAsyncLoading();

	/** Is async loading current in progress? */
	/** 当前是否正在进行异步加载？ */
	bool IsAsyncLoadingInProgress() const;

private:
	/**
	 * The FLoadingState is what actually is allocated for the FAsyncMixin in a big map so that the FAsyncMixin itself holds no
	 * no memory, and we dynamically create the FLoadingState only if needed, and destroy it when it's unneeded.
	 */
	/**
	 * FLoadingState 实际上是为 FAsyncMixin 在一个大的映射中分配的，
	 * 这样 FAsyncMixin 本身就不会占用任何内存，我们只在需要时动态创建 FLoadingState，
	 * 并在不需要时销毁它。
	 */
	class FLoadingState : public TSharedFromThis<FLoadingState>
	{
	public:
		FLoadingState(FAsyncMixin& InOwner);
		virtual ~FLoadingState();

		/** Starts the async sequence. */
		/** 开始异步序列。 */
		void Start();

		/** Cancels the async sequence. */
		/** 取消异步序列。 */
		void CancelAndDestroy();

		void AsyncLoad(FSoftObjectPath SoftObject, const FSimpleDelegate& DelegateToCall);
		void AsyncLoad(const TArray<FSoftObjectPath>& SoftObjectPaths, const FSimpleDelegate& DelegateToCall);
		void AsyncPreloadPrimaryAssetsAndBundles(const TArray<FPrimaryAssetId>& PrimaryAssetIds, const TArray<FName>& LoadBundles,
		                                         const FSimpleDelegate& DelegateToCall);
		void AsyncCondition(TSharedRef<FAsyncCondition> Condition, const FSimpleDelegate& Callback);
		void AsyncEvent(const FSimpleDelegate& Callback);

		bool IsLoadingComplete() const { return !IsLoadingInProgress(); }
		bool IsLoadingInProgress() const;
		bool IsLoadingInProgressOrPending() const;
		bool IsPendingDestroy() const;

	private:
		void CancelOnly(bool bDestroying);
		void CancelStartTimer();
		void TryScheduleStart();
		void TryCompleteAsyncLoading();
		void CompleteAsyncLoading();

	private:
		void RequestDestroyThisMemory();
		void CancelDestroyThisMemory(bool bDestroying);

		/** Who owns the loading state?  We need this to call back into the owning mix-in object. */
		/** 谁拥有加载状态？我们需要这个来回调拥有的混合对象。 */
		FAsyncMixin& OwnerRef;

		/**
		 * Did we need to pre-load bundles?  If we didn't pre-load bundles (which require you keep the streaming handle 
		 * around or they will be destroyed), then we can safely destroy the FLoadingState when everything is done loading.
		 */
		/**
		 * 我们是否需要预加载包？如果我们没有预加载包（这需要你保持流处理句柄，
		 * 否则它们将被销毁），那么当所有东西都加载完毕时，我们可以安全地销毁 FLoadingState。
		 */
		bool bPreloadedBundles = false;

		class FAsyncStep
		{
		public:
			FAsyncStep(const FSimpleDelegate& InUserCallback);
			FAsyncStep(const FSimpleDelegate& InUserCallback, const TSharedPtr<FStreamableHandle>& InStreamingHandle);
			FAsyncStep(const FSimpleDelegate& InUserCallback, const TSharedPtr<FAsyncCondition>& InCondition);

			~FAsyncStep();

			void ExecuteUserCallback();

			bool IsLoadingInProgress() const
			{
				return !IsComplete();
			}

			bool IsComplete() const;
			void Cancel();

			bool BindCompleteDelegate(const FSimpleDelegate& NewDelegate);
			bool IsCompleteDelegateBound() const;

		private:
			FSimpleDelegate UserCallback;
			bool bIsCompletionDelegateBound = false;

			// Possible Async 'thing'
			TSharedPtr<FStreamableHandle> StreamingHandle;
			TSharedPtr<FAsyncCondition> Condition;
		};

		bool bHasStarted = false;

		int32 CurrentAsyncStep = 0;
		TArray<TUniquePtr<FAsyncStep>> AsyncSteps;
		TArray<TUniquePtr<FAsyncStep>> AsyncStepsPendingDestruction;

		FTSTicker::FDelegateHandle StartTimerDelegate;
		FTSTicker::FDelegateHandle DestroyMemoryDelegate;
	};

	const FLoadingState& GetLoadingStateConst() const;

	FLoadingState& GetLoadingState();

	bool HasLoadingState() const;

	bool IsLoadingInProgressOrPending() const;

private:
	static TMap<FAsyncMixin*, TSharedRef<FLoadingState>> Loading;
};

/**
 * Sometimes a mix-in just doesn't make sense.  Perhaps the object has to manage many different jobs
 * that each have their own async dependency chain/scope.  For those situations you can use the FAsyncScope.
 * 
 * This class is a standalone Async dependency handler so that you can fire off several load jobs and always handle them
 * in the proper order, just like with combining FAsyncMixin with your class.
 */
/**
 * 有时候，混入（mix-in）可能并不合适。也许对象必须管理许多不同的工作，
 * 每个工作都有自己的异步依赖链/范围。对于这些情况，您可以使用 FAsyncScope。
 * 
 * 这个类是一个独立的异步依赖处理器，所以你可以发起几个加载任务，并始终以正确的顺序处理它们，
 * 就像将 FAsyncMixin 与你的类结合使用一样。
 */
class ASYNCMIXIN_API FAsyncScope : public FAsyncMixin
{
public:
	using FAsyncMixin::AsyncLoad;

	using FAsyncMixin::AsyncPreloadPrimaryAssetsAndBundles;

	using FAsyncMixin::AsyncCondition;

	using FAsyncMixin::AsyncEvent;

	using FAsyncMixin::CancelAsyncLoading;

	using FAsyncMixin::StartAsyncLoading;

	using FAsyncMixin::IsAsyncLoadingInProgress;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

enum class EAsyncConditionResult : uint8
{
	TryAgain, // 条件不满足
	Complete // 条件满足
};

DECLARE_DELEGATE_RetVal(EAsyncConditionResult, FAsyncConditionDelegate);

/**
 * The async condition allows you to have custom reasons to hault the async loading until some condition is met.
 */
/**
 * 异步条件允许你有自定义的原因来暂停异步加载，直到满足某些条件。
 */
class FAsyncCondition : public TSharedFromThis<FAsyncCondition>
{
public:
	FAsyncCondition(const FAsyncConditionDelegate& Condition);
	FAsyncCondition(TFunction<EAsyncConditionResult()>&& Condition);
	virtual ~FAsyncCondition();

protected:
	bool IsComplete() const;
	bool BindCompleteDelegate(const FSimpleDelegate& NewDelegate);

private:
	bool TryToContinue(float DeltaTime);

	FTSTicker::FDelegateHandle RepeatHandle;
	FAsyncConditionDelegate UserCondition;
	FSimpleDelegate CompletionDelegate;

	friend FAsyncMixin;
};
