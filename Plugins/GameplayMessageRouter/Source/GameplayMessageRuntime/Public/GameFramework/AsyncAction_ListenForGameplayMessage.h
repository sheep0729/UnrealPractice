// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/CancellableAsyncAction.h"
#include "GameplayMessageSubsystem.h"
#include "GameplayMessageTypes2.h"

#include "AsyncAction_ListenForGameplayMessage.generated.h"

class UScriptStruct;
class UWorld;
struct FFrame;

/**
 * Proxy object pin will be hidden in K2Node_GameplayMessageAsyncAction. Is used to get a reference to the object triggering the delegate for the follow up call of 'GetPayload'.
 *
 * @param ActualChannel		The actual message channel that we received Payload from (will always start with Channel, but may be more specific if partial matches were enabled)
 */
/**
 * 代理对象引脚将在 K2Node_GameplayMessageAsyncAction 中隐藏。用于获取触发委托的对象的引用，以进行“GetPayload”的后续调用。
 *
 * @param ActualChannel	我们从中接收到 Payload 的实际消息通道（始终以 Channel 开头，但如果启用了部分匹配，则可能更具体）
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAsyncGameplayMessageDelegate, UAsyncAction_ListenForGameplayMessage*, ProxyObject, FGameplayTag, ActualChannel);

UCLASS(BlueprintType, meta=(HasDedicatedAsyncNode))
class GAMEPLAYMESSAGERUNTIME_API UAsyncAction_ListenForGameplayMessage : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:
	/**
	 * Asynchronously waits for a gameplay message to be broadcast on the specified channel.
	 *
	 * @param Channel			The message channel to listen for
	 * @param PayloadType		The kind of message structure to use (this must match the same type that the sender is broadcasting)
	 * @param MatchType			The rule used for matching the channel with broadcasted messages
	 */
	/**
	 * 异步等待在指定频道上广播的游戏消息。
	 *
	 * @param Channel            要监听的消息通道
	 * @param PayloadType        要使用的消息结构类型（这必须与发送方广播的相同类型匹配）
	 * @param MatchType          用于将通道与广播消息匹配的规则
	 */
	UFUNCTION(BlueprintCallable, Category = Messaging, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"))
	static UAsyncAction_ListenForGameplayMessage* ListenForGameplayMessages(UObject* WorldContextObject, FGameplayTag Channel, UScriptStruct* PayloadType, EGameplayMessageMatch MatchType = EGameplayMessageMatch::ExactMatch);

	/**
	 * Attempt to copy the payload received from the broadcasted gameplay message into the specified wildcard.
	 * The wildcard's type must match the type from the received message.
	 *
	 * @param OutPayload	The wildcard reference the payload should be copied into
	 * @return				If the copy was a success
	 */
	/**
	 * 尝试将从广播的游戏消息中接收到的有效负载复制到指定的通配符中。
	 * 通配符的类型必须与接收到的消息的类型匹配。
	 *
	 * @param OutPayload    应将有效负载复制到的通配符引用
	 * @return              如果复制成功
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Messaging", meta = (CustomStructureParam = "OutPayload"))
	bool GetPayload(UPARAM(ref) int32& OutPayload);

	DECLARE_FUNCTION(execGetPayload);

	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;

public:
	/** Called when a message is broadcast on the specified channel. Use GetPayload() to request the message payload. */
	/** 当在指定频道上广播消息时调用。使用 GetPayload() 请求消息有效负载。*/
	UPROPERTY(BlueprintAssignable)
	FAsyncGameplayMessageDelegate OnMessageReceived;

private:
	void HandleMessageReceived(FGameplayTag Channel, const UScriptStruct* StructType, const void* Payload);

private:
	const void* ReceivedMessagePayloadPtr = nullptr;

	TWeakObjectPtr<UWorld> WorldPtr;
	FGameplayTag ChannelToRegister;
	TWeakObjectPtr<UScriptStruct> MessageStructType = nullptr;
	EGameplayMessageMatch MessageMatchType = EGameplayMessageMatch::ExactMatch;

	FGameplayMessageListenerHandle ListenerHandle;
};
