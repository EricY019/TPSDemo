#include "DemoPlayerState.h"

#include "DemoCharacter.h"
#include "Net/UnrealNetwork.h"

ADemoPlayerState::ADemoPlayerState()
{
	// Tick on both clients and server
	PrimaryActorTick.bCanEverTick = true;
	SetTickGroup(TG_PostUpdateWork);
}

void ADemoPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// Replicate position history from server to clients
	DOREPLIFETIME(ADemoPlayerState, PositionHistory);
}

void ADemoPlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority())
	{	// Update position to server on clients
		if (ADemoCharacter* OwningCharacter = Cast<ADemoCharacter>(GetOwner()))
		{
			FVector CurrentPosition = OwningCharacter->GetActorLocation();
			float CurrentTime = GetWorld()->GetTimeSeconds();
			ServerUpdatePosition(CurrentPosition, CurrentTime);
		}
	}
}

void ADemoPlayerState::ServerUpdatePosition_Implementation(const FVector& NewPosition, float TimeStamp)
{
	if (HasAuthority())
	{
		// Add current position to history
		PositionHistory.Add(FPositionHistoryEntry(NewPosition, TimeStamp));

		// Remove old entries beyond
		float CurrentTime = GetWorld()->GetTimeSeconds();
		while (PositionHistory.Num() > 0 && (CurrentTime - PositionHistory.Last().Time) > MaxHistoryDuration)
		{
			PositionHistory.RemoveAt(0);
		}
	}	
}

FVector ADemoPlayerState::GetPositionAtTime(float ServerTime) const
{
	// Find the two entries between which ServerTime falls
	if (PositionHistory.Num() == 0)
	{
		return FVector::ZeroVector;
	}
	
	if (ServerTime <= PositionHistory[0].Time)
	{
		return PositionHistory[0].Position;
	}

	for (int32 i = 0; i < PositionHistory.Num() - 1; i++)
	{
		if (ServerTime >= PositionHistory[i].Time && ServerTime < PositionHistory[i + 1].Time)
		{
			// Linear Interpolation between PositionHistory[i] and PositionHistory[i+1]
			float Alpha = (ServerTime - PositionHistory[i].Time) / (PositionHistory[i + 1].Time - PositionHistory[i].Time);
			return FMath::Lerp(PositionHistory[i].Position, PositionHistory[i + 1].Position, Alpha);
		}
	}
	// Return last position if servertime is after last recorded time
	return PositionHistory.Last().Position;
}
