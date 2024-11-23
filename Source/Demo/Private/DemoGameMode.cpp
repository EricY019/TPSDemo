#include "Demo/Public/DemoGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "DemoCharacter.h"
#include "PlayerController/DemoPlayerController.h"

ADemoGameMode::ADemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Character/BP_DemoCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ADemoGameMode::PlayerEliminated(ADemoCharacter* ElimmedCharacter, ADemoPlayerController* VictimController, ADemoPlayerController* AttackerController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}
