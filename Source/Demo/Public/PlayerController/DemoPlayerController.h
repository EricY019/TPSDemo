#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DemoPlayerController.generated.h"

class ADemoHUD;

/**
 * 
 */
UCLASS()
class DEMO_API ADemoPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// Set HUD health bar and text
	void SetHUDHealth(float Health, float MaxHealth);

protected:
	virtual void BeginPlay() override;

private:
	ADemoHUD* DemoHUD;
};
