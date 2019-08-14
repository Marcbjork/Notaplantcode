#include "TNTongueAbility.h"
#include "Engine/World.h"
#include "TNCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"

UTNTongueAbility::UTNTongueAbility()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

bool UTNTongueAbility::StartAbility()
{
	// Fixes a bug where a player holds the ability and tabs in and out, leaving them stuck
	if (Tongue != nullptr)
		return false;

	if (CanAttachTongue())
	{
		FActorSpawnParameters Params;
		Params.Owner = GetOwner();
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		Tongue = GetWorld()->SpawnActor<ATNTongue>(TongueActor, Params);

		if (Tongue != nullptr)
		{
			ATNCharacter* Character = GetCharacter();

			if (Character != nullptr)
				if (UPrimitiveComponent* OwnerMesh = Character->GetMesh())
					Tongue->AttachBase(OwnerMesh, OwnerMesh->GetSocketLocation(TongueSocket), TongueSocket);
			
			Tongue->AttachTip(Trace.GetComponent(), Trace.ImpactPoint);
			Tongue->ResetMeshPose();

			if (Tongues.Num() >= MaxTongues)
			{
				ATNTongue* Last = Tongues.Pop();
				Last->Destroy();
			}

			OnAttach.Broadcast(Trace.GetActor(), Trace.GetComponent());

			return true;
		}
	}

	return false;
}

void UTNTongueAbility::EndAbility()
{
	if (Tongue == nullptr)
		return;

	if (CanAttachTongue())
	{
		Tongue->AttachBase(Trace.GetComponent(), Trace.ImpactPoint);
		Tongues.Insert(Tongue, 0);

		OnAttach.Broadcast(Trace.GetActor(), Trace.GetComponent());
	}
	else
	{
		Tongue->Destroy();
	}

	Tongue = nullptr;
}

void UTNTongueAbility::ClearTongues()
{
	for (int i = 0; i < Tongues.Num(); i++)
		Tongues[i]->Destroy();

	Tongues.Empty();

	if (Tongue != nullptr)
	{
		Tongue->Destroy();
		Tongue = nullptr;
	}
}

void UTNTongueAbility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector Location;
	FVector Direction;
	GetCharacter()->GetCameraView(Location, Direction);
	FVector End = Location + Direction * Range;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	TraceHit = GetWorld()->SweepSingleByChannel(Trace, Location, End, FQuat(), ECC_Visibility, FCollisionShape::MakeSphere((TraceRadius)));
	//TraceHit = GetWorld()->LineTraceSingleByChannel(Trace, Location, End, ECC_Visibility, Params);
}
