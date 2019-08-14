#pragma once
#include "Components/ActorComponent.h"
#include "TNTongue.h"
#include "TNCharacter.h"
#include "TNAbilityInterface.h"
#include "TNTongueAbility.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttach, AActor*, Actor, UPrimitiveComponent*, Component);

UCLASS(ClassGroup = (Custom), BlueprintType, meta = (BlueprintSpawnableComponent))
class TN_API UTNTongueAbility : public UActorComponent/*, public ITNAbilityInterface*/
{
	GENERATED_BODY()

public:
	UTNTongueAbility();

	/** Called when a tongue is attached on either end. */
	UPROPERTY(BlueprintAssignable)
	FOnAttach OnAttach;

	/** Position from which the tongue will be attached. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tongue")
	FName TongueSocket;

	/** From how far the tongue can be attached. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tongue")
	float Range = 1000.f;
	
	/** The radius of the sphere trace, allows less accurate aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tongue")
	float TraceRadius = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tongue")
	int MaxTongues = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tongue")
	TSubclassOf<ATNTongue> TongueActor;

	UFUNCTION(BlueprintCallable, Category = "Tongue")
	bool StartAbility();

	UFUNCTION(BlueprintCallable, Category = "Tongue")
	void EndAbility();

	UFUNCTION(BlueprintCallable, Category = "Tongue")
	void ClearTongues();

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool CanAttachTongue() const { return TraceHit; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE ATNTongue* GetTongue() const { return Tongue; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE ATNCharacter* GetCharacter() const { return Cast<ATNCharacter>(GetOwner()); }

private:
	FHitResult Trace;
	bool TraceHit;

	ATNTongue* Tongue;
	TArray<ATNTongue*> Tongues;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};