#include "PlayerController/DemoPlayerController.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HUD/DemoHUD.h"
#include "HUD/CharacterOverlay.h"


void ADemoPlayerController::BeginPlay()
{
	Super::BeginPlay();

	DemoHUD = Cast<ADemoHUD>(GetHUD());
}

void ADemoPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	DemoHUD = DemoHUD == nullptr ? Cast<ADemoHUD>(GetHUD()) : DemoHUD;
	if (DemoHUD && DemoHUD->CharacterOverlay && DemoHUD->CharacterOverlay->HealthBar && DemoHUD->CharacterOverlay->HealthText)
	{	// Set progress bar percent
		const float HealthPercent = Health / MaxHealth;
		DemoHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		// Set text
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		DemoHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}