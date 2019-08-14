#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "TNAICharacter.generated.h"

class AAIController;
class ATNAIActivationActor;
class USphereComponent;

UCLASS()
class TN_API ATNAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATNAICharacter();

	// Public functionality
	UPROPERTY(VisibleAnywhere)
	USphereComponent* ObjectTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	ATNAIActivationActor* ActorRef;

	UPROPERTY(BlueprintReadWrite, Category = "Data")
	bool bShouldChase = false;;

	// Delegates
	UFUNCTION()
	void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};