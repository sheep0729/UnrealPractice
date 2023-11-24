// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameplayMessageTypes2.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/WeakObjectPtr.h"

#include "GameplayMessageSubsystem.generated.h"

class UGameplayMessageSubsystem;
struct FFrame;

GAMEPLAYMESSAGERUNTIME_API DECLARE_LOG_CATEGORY_EXTERN(LogGameplayMessageSubsystem, Log, All);

class UAsyncAction_ListenForGameplayMessage;

/**
 * An opaque handle that can be used to remove a previously registered message listener
 * @see UGameplayMessageSubsystem::RegisterListener and UGameplayMessageSubsystem::UnregisterListener
 */
/**
 * 一个不透明的句柄，可用于删除先前注册的消息侦听器
 * @see UGameplayMessageSubsystem::RegisterListener 和 UGameplayMessageSubsystem::UnregisterListener
 */
USTRUCT(BlueprintType)
struct GAMEPLAYMESSAGERUNTIME_API FGameplayMessageListenerHandle
{
public:
	GENERATED_BODY()

	FGameplayMessageListenerHandle() {}

	void Unregister();

	bool IsValid() const { return ID != 0; }

private:
	UPROPERTY(Transient)
	TWeakObjectPtr<UGameplayMessageSubsystem> Subsystem;

	UPROPERTY(Transient)
	FGameplayTag Channel;

	UPROPERTY(Transient)
	int32 ID = 0;

	FDelegateHandle StateClearedHandle;

	friend UGameplayMessageSubsystem;

	FGameplayMessageListenerHandle(UGameplayMessageSubsystem* InSubsystem, FGameplayTag InChannel, int32 InID) : Subsystem(InSubsystem), Channel(InChannel), ID(InID) {}
};

/** 
 * Entry information for a single registered listener
 */
/**
 * 一个用于单个注册侦听器的条目信息
 */
USTRUCT()
struct FGameplayMessageListenerData
{
	GENERATED_BODY()

	// Callback for when a message has been received
	// 当消息被接收时的回调函数
	TFunction<void(FGameplayTag, const UScriptStruct*, const void*)> ReceivedCallback;

	int32 HandleID;
	EGameplayMessageMatch MatchType;

	// Adding some logging and extra variables around some potential problems with this
	// 围绕一些潜在问题添加一些日志记录和额外变量
	TWeakObjectPtr<const UScriptStruct> ListenerStructType = nullptr;
	bool bHadValidType = false;
};

/**
 * This system allows event raisers and listeners to register for messages without
 * having to know about each other directly, though they must agree on the format
 * of the message (as a USTRUCT() type).
 *
 *
 * You can get to the message router from the game instance:
 *    UGameInstance::GetSubsystem<UGameplayMessageSubsystem>(GameInstance)
 * or directly from anything that has a route to a world:
 *    UGameplayMessageSubsystem::Get(WorldContextObject)
 *
 * Note that call order when there are multiple listeners for the same channel is
 * not guaranteed and can change over time!
 */
/**
 * 这个系统允许事件触发器和监听器注册消息，而无需直接了解彼此，尽管它们必须就消息的格式达成一致（作为USTRUCT()类型）。
 *
 *
 * 你可以从游戏实例中获取消息路由器：
 *    UGameInstance::GetSubsystem<UGameplayMessageSubsystem>(GameInstance)
 * 或者直接从任何具有到世界的路由的东西中获取：
 *    UGameplayMessageSubsystem::Get(WorldContextObject)
 *
 * 请注意，当同一通道有多个监听器时，调用顺序不能保证并且可能随时间而变化！
 */
UCLASS()
class GAMEPLAYMESSAGERUNTIME_API UGameplayMessageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend UAsyncAction_ListenForGameplayMessage;

public:

	/**
	 * @return the message router for the game instance associated with the world of the specified object
	 */
	/**
	 * @return 返回与指定对象的世界相关联的游戏实例的消息路由器
	 */
	static UGameplayMessageSubsystem& Get(const UObject* WorldContextObject);

	/**
	 * @return true if a valid GameplayMessageRouter subsystem if active in the provided world
	 */
	/**
	 * @return 如果在提供的世界中存在有效的 GameplayMessageRouter 子系统，则返回 true
	 */
	static bool HasInstance(const UObject* WorldContextObject);

	//~USubsystem interface
	virtual void Deinitialize() override;
	//~End of USubsystem interface

	/**
	 * Broadcast a message on the specified channel
	 *
	 * @param Channel			The message channel to broadcast on
	 * @param Message			The message to send (must be the same type of UScriptStruct expected by the listeners for this channel, otherwise an error will be logged)
	 */
	/**
	 * 在指定的通道上广播消息
	 *
	 * @param Channel			要广播的消息通道
	 * @param Message			要发送的消息（必须与此通道的侦听器期望的 UScriptStruct 相同类型，否则将记录错误）
	 */
	template <typename FMessageStructType>
	void BroadcastMessage(FGameplayTag Channel, const FMessageStructType& Message)
	{
		const UScriptStruct* StructType = TBaseStructure<FMessageStructType>::Get();
		BroadcastMessageInternal(Channel, StructType, &Message);
	}

	/**
	 * Register to receive messages on a specified channel
	 *
	 * @param Channel			The message channel to listen to
	 * @param Callback			Function to call with the message when someone broadcasts it (must be the same type of UScriptStruct provided by broadcasters for this channel, otherwise an error will be logged)
	 *
	 * @return a handle that can be used to unregister this listener (either by calling Unregister() on the handle or calling UnregisterListener on the router)
	 */
	/**
	 * 在指定的通道上注册以接收消息
	 *
	 * @param Channel			要监听的消息通道
	 * @param Callback			当有人广播消息时调用的函数（必须与此通道的广播者提供的 UScriptStruct 相同类型，否则将记录错误）
	 *
	 * @return 可用于取消注册此侦听器的句柄（通过在句柄上调用 Unregister() 或在路由器上调用 UnregisterListener）
	 */
	template <typename FMessageStructType>
	FGameplayMessageListenerHandle RegisterListener(FGameplayTag Channel, TFunction<void(FGameplayTag, const FMessageStructType&)>&& Callback, EGameplayMessageMatch MatchType = EGameplayMessageMatch::ExactMatch)
	{
		auto ThunkCallback = [InnerCallback = MoveTemp(Callback)](FGameplayTag ActualTag, const UScriptStruct* SenderStructType, const void* SenderPayload)
		{
			InnerCallback(ActualTag, *reinterpret_cast<const FMessageStructType*>(SenderPayload));
		};

		const UScriptStruct* StructType = TBaseStructure<FMessageStructType>::Get();
		return RegisterListenerInternal(Channel, ThunkCallback, StructType, MatchType);
	}

	/**
	 * Register to receive messages on a specified channel and handle it with a specified member function
	 * Executes a weak object validity check to ensure the object registering the function still exists before triggering the callback
	 *
	 * @param Channel			The message channel to listen to
	 * @param Object			The object instance to call the function on
	 * @param Function			Member function to call with the message when someone broadcasts it (must be the same type of UScriptStruct provided by broadcasters for this channel, otherwise an error will be logged)
	 *
	 * @return a handle that can be used to unregister this listener (either by calling Unregister() on the handle or calling UnregisterListener on the router)
	 */
	/**
	 * 在指定的通道上注册以接收消息，并使用指定的成员函数处理它
	 * 执行弱对象有效性检查，以确保注册函数的对象仍然存在，然后触发回调
	 *
	 * @param Channel			要监听的消息通道
	 * @param Object			要调用函数的对象实例
	 * @param Function			当有人广播消息时调用的成员函数（必须与此通道的广播者提供的 UScriptStruct 相同类型，否则将记录错误）
	 *
	 * @return 可用于取消注册此侦听器的句柄（通过在句柄上调用 Unregister() 或在路由器上调用 UnregisterListener）
	 */
	template <typename FMessageStructType, typename TOwner = UObject>
	FGameplayMessageListenerHandle RegisterListener(FGameplayTag Channel, TOwner* Object, void(TOwner::* Function)(FGameplayTag, const FMessageStructType&))
	{
		TWeakObjectPtr<TOwner> WeakObject(Object);
		return RegisterListener<FMessageStructType>(Channel,
			[WeakObject, Function](FGameplayTag Channel, const FMessageStructType& Payload)
			{
				if (TOwner* StrongObject = WeakObject.Get())
				{
					(StrongObject->*Function)(Channel, Payload);
				}
			});
	}

	/**
	 * Register to receive messages on a specified channel with extra parameters to support advanced behavior
	 * The stateful part of this logic should probably be separated out to a separate system
	 *
	 * @param Channel			The message channel to listen to
	 * @param Params			Structure containing details for advanced behavior
	 *
	 * @return a handle that can be used to unregister this listener (either by calling Unregister() on the handle or calling UnregisterListener on the router)
	 */
	/**
	 * 在指定的通道上注册以接收消息，并使用指定的成员函数处理它
	 * 执行弱对象有效性检查，以确保注册函数的对象仍然存在，然后触发回调
	 *
	 * @param Channel			要监听的消息通道
	 * @param Params			包含高级行为详细信息的结构体
	 *
	 * @return 可用于取消注册此侦听器的句柄（通过在句柄上调用 Unregister() 或在路由器上调用 UnregisterListener）
	 */
	template <typename FMessageStructType>
	FGameplayMessageListenerHandle RegisterListener(FGameplayTag Channel, FGameplayMessageListenerParams<FMessageStructType>& Params)
	{
		FGameplayMessageListenerHandle Handle;

		// Register to receive any future messages broadcast on this channel
		if (Params.OnMessageReceivedCallback)
		{
			auto ThunkCallback = [InnerCallback = Params.OnMessageReceivedCallback](FGameplayTag ActualTag, const UScriptStruct* SenderStructType, const void* SenderPayload)
			{
				InnerCallback(ActualTag, *reinterpret_cast<const FMessageStructType*>(SenderPayload));
			};

			const UScriptStruct* StructType = TBaseStructure<FMessageStructType>::Get();
			Handle = RegisterListenerInternal(Channel, ThunkCallback, StructType, Params.MatchType);
		}

		return Handle;
	}

	/**
	 * Remove a message listener previously registered by RegisterListener
	 *
	 * @param Handle	The handle returned by RegisterListener
	 */
	/**
	 * 通过 RegisterListener 先前注册的消息侦听器
	 *
	 * @param Handle	RegisterListener 返回的句柄
	 */
	void UnregisterListener(FGameplayMessageListenerHandle Handle);

protected:
	/**
	 * Broadcast a message on the specified channel
	 *
	 * @param Channel			The message channel to broadcast on
	 * @param Message			The message to send (must be the same type of UScriptStruct expected by the listeners for this channel, otherwise an error will be logged)
	 */
	/**
	 * 在指定的通道上广播消息
	 *
	 * @param Channel			要广播的消息通道
	 * @param Message			要发送的消息（必须与此通道的侦听器期望的 UScriptStruct 相同类型，否则将记录错误）
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category=Messaging, meta=(CustomStructureParam="Message", AllowAbstract="false", DisplayName="Broadcast Message"))
	void K2_BroadcastMessage(FGameplayTag Channel, const int32& Message);

	DECLARE_FUNCTION(execK2_BroadcastMessage);

private:
	// Internal helper for broadcasting a message
	// 用于广播消息的内部帮助程序
	void BroadcastMessageInternal(FGameplayTag Channel, const UScriptStruct* StructType, const void* MessageBytes);

	// Internal helper for registering a message listener
	// 用于注册消息监听器的内部辅助函数
	FGameplayMessageListenerHandle RegisterListenerInternal(
		FGameplayTag Channel, 
		TFunction<void(FGameplayTag, const UScriptStruct*, const void*)>&& Callback,
		const UScriptStruct* StructType,
		EGameplayMessageMatch MatchType);

	void UnregisterListenerInternal(FGameplayTag Channel, int32 HandleID);

private:
	// List of all entries for a given channel
	// 给定通道的所有条目列表
	struct FChannelListenerList
	{
		TArray<FGameplayMessageListenerData> Listeners;
		int32 HandleID = 0;
	};

private:
	TMap<FGameplayTag, FChannelListenerList> ListenerMap;
};
