#include "TNAICharacter.h"
#include "TNAIActivationActor.h"
#include "Components/SphereComponent.h"
#include "TNTongue.h"

ATNAICharacter::ATNAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	ObjectTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("Object Trigger"));
	ObjectTrigger->InitSphereRadius(100.f);
	ObjectTrigger->SetupAttachment(RootComponent);

	ObjectTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATNAICharacter::OnComponentBeginOverlap);
}

void ATNAICharacter::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == ActorRef)
	{
		TArray<AActor*> AttachedActors;
		ActorRef->GetAttachedActors(AttachedActors);

		for (auto AttachedActor : AttachedActors)
			if (AttachedActor->IsA<ATNTongue>())
				AttachedActor->Destroy();

		ActorRef->Reset();
	}
}
