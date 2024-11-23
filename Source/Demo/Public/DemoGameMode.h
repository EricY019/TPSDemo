#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlayerController/DemoPlayerController.h"
#include "DemoGameMode.generated.h"

class ADemoCharacter;
class ADemoPlayerController;

/**
 * 
 */
UCLASS()
class DEMO_API ADemoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADemoGameMode();

	// Called when player is eliminated
	virtual void PlayerEliminated(ADemoCharacter* ElimmedCharacter, ADemoPlayerController* VictimController, ADemoPlayerController* AttackerController);
};
