#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DemoPlayerState.generated.h"


USTRUCT()
struct FPositionHistoryEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Position;

	UPROPERTY()
	float Time;

	FPositionHistoryEntry() : Position(FVector::ZeroVector), Time(0.0f) {}
	FPositionHistoryEntry(const FVector& InPosition, float InTime) : Position(InPosition), Time(InTime) {}
};

/**
 * 
 */
UCLASS()
class DEMO_API ADemoPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ADemoPlayerState();
	virtual void Tick(float DeltaTime) override;

	// RPC, update position on server
	UFUNCTION(Server, Reliable)
	void ServerUpdatePosition(const FVector& NewPosition, float TimeStamp);

	// Retrieves the position of the player at a specific time, called on server
	FVector GetPositionAtTime(float ServerTime) const;

	// Set replicates
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// Circular list to store past positions with timestamp index, updated on server
	UPROPERTY()
	TArray<FPositionHistoryEntry> PositionHistory;

	// Maximum history duration in seconds, default 1s
	UPROPERTY(EditDefaultsOnly, Category = "Position History")
	float MaxHistoryDuration = 1.0f;
};
