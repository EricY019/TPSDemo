// Fill out your copyright notice in the Description page of Project Settings.


#include "Demo/Public/DemoGameMode.h"
#include "UObject/ConstructorHelpers.h"

ADemoGameMode::ADemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Character/BP_DemoCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
