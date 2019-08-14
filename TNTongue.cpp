#include "TNTongue.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TNAIActivationActor.h"
#include "Components/PoseableMeshComponent.h"

ATNTongue::ATNTongue()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Mesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Base = CreateDefaultSubobject<USceneComponent>(TEXT("Base"));
	Base->SetupAttachment(RootComponent);

	Tip = CreateDefaultSubobject<USceneComponent>(TEXT("Tip"));
	Tip->SetupAttachment(RootComponent);
}

void ATNTongue::AttachBase(UPrimitiveComponent* Component, const FVector Location, FName Socket)
{
	// Detach first, so we're not trying to attach multiple objects
	DetachBase();

	// Move the component to the location
	Base->SetWorldLocation(Location);

	// Can't attach to nothing, BSP and such require no attachment, we'll simply move to the location
	if (Component == nullptr)
		return;

	// Attach to the actor's root component
	FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, true);
	Base->AttachToComponent(Component, Rules, Socket);

	BaseAttached = Component;
}

void ATNTongue::AttachTip(UPrimitiveComponent* Component, const FVector Location, FName Socket)
{
	// Detach first, so we're not trying to attach multiple objects
	DetachTip();

	// Move the component to the location
	Tip->SetWorldLocation(Location);

	// Can't attach to nothing, BSP and such require no attachment, we'll simply move to the location
	if (Component == nullptr)
		return;

	// Attach to the actor's root component
	FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, true);
	Tip->AttachToComponent(Component, Rules, Socket);

	TipAttached = Component;
}

void ATNTongue::DetachBase()
{
	FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, true);
	Base->DetachFromComponent(Rules);
}

void ATNTongue::DetachTip()
{
	FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, true);
	Tip->DetachFromComponent(Rules);
}

void ATNTongue::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PoseMesh();

	if (Mesh->bHiddenInGame)
		Mesh->SetHiddenInGame(false);

	if (Tip == nullptr || Base == nullptr)
		return;

	UPrimitiveComponent* TipComponent = GetTipAttached();
	UPrimitiveComponent* BaseComponent = GetBaseAttached();

	// It's attached to the same two objects or if none of them are simulating physics, we'll stop here
	//if (BaseComponent != nullptr && TipComponent != nullptr && BaseComponent->GetOwner() != GetOwner())
	//	if (TipComponent == BaseComponent || (!TipComponent->IsSimulatingPhysics() && !BaseComponent->IsSimulatingPhysics()))
	//		return;

	FVector TipVelocity = IsValid(TipComponent) ? TipComponent->GetComponentVelocity() : FVector::ZeroVector;
	FVector BaseVelocity = IsValid(BaseComponent) ? BaseComponent->GetComponentVelocity() : FVector::ZeroVector;

	// Get the force between the two points, we only apply half of the force to each object, as it should be split between them
	FVector Force = GetSpringForce(GetTipLocation(), GetBaseLocation(), TipVelocity, BaseVelocity);

	ApplySpringForce(TipComponent, GetTipLocation(), Force);
	ApplySpringForce(BaseComponent, GetBaseLocation(), -Force);
}

void ATNTongue::PoseMesh()
{
	// Center the location of the actor itself between tongue points, helps avoid issues with rendering
	SetActorLocation((Tip->GetComponentLocation() + Base->GetComponentLocation()) / 2.f);

	int NumBones = MeshBones.Num();
	for (int i = 0; i < NumBones; i++)
	{
		FName Bone = MeshBones[i];
		FVector Direction = Tip->GetComponentLocation() - Base->GetComponentLocation();
		float Offset = (Direction / (NumBones - 1)).Size();
		Direction.Normalize();
		FRotator Rotation = FRotationMatrix::MakeFromXY(Direction, -FVector::UpVector).Rotator();

		FVector Location = Base->GetComponentLocation() + Direction * (Offset * i);
		FVector PreviousLocation = Mesh->GetBoneLocationByName(Bone, EBoneSpaces::WorldSpace);

		float Interpolation = (FMath::Abs((NumBones / 2) - i) / (NumBones / 2.f) + 0.5f) / 0.5f;
		if (GetBaseAttached() != nullptr && GetBaseAttached()->GetOwner() == GetOwner() && i == 0)
			Interpolation *= 50.f;
		FVector TargetLocation = FMath::VInterpTo(PreviousLocation, Location, GetWorld()->GetDeltaSeconds(), Interpolation * MeshInterpRate);

		Mesh->SetBoneLocationByName(Bone, TargetLocation, EBoneSpaces::WorldSpace);
		Mesh->SetBoneRotationByName(Bone, Rotation, EBoneSpaces::WorldSpace);

		//DrawDebugSphere(GetWorld(), TargetLocation, 8.f, 12, FColor::Blue, false, -1.f, 0, 1.f);
	}

	Mesh->UpdateBounds();
}

void ATNTongue::ResetMeshPose()
{
	for (int i = 0; i < MeshBones.Num(); i++)
		Mesh->SetBoneLocationByName(MeshBones[i], GetBaseLocation(), EBoneSpaces::WorldSpace);
}

FVector ATNTongue::GetSpringForce(const FVector APos, const FVector BPos, const FVector AVel, const FVector BVel)
{
	FVector Vector = APos - BPos;
	float Distance = Vector.Size();

	FVector Force;

	if (!bKeepDistance && Distance < TongueLength)
		return Force;

	if (Distance != 0)
	{
		Force += -(Vector / Distance) * (Distance - TongueLength) * Stiffness;
		Force += -(AVel - BVel) * Friction;
	}

	return Force;
}

void ATNTongue::ApplySpringForce(UPrimitiveComponent* Component, FVector Location, FVector Force)
{
	if (!IsValid(Component))
		return;

	// For normal physics objects we just apply the force
	if (Component->IsSimulatingPhysics())
	{
		Component->AddForceAtLocation(Force, Location);
		return;
	}
	// Special case for ACharacter, as they do not have "normal" physics, we need to use it's movement component
	else if (Component->GetOwner() != nullptr && Component->GetOwner()->IsA<ACharacter>())
	{
		ACharacter* Player = Cast<ACharacter>(Component->GetOwner());
		Player->GetCharacterMovement()->AddForce(Force / 2.f);
	}
}

void ATNTongue::BeginPlay()
{
	Super::BeginPlay();

	Mesh->SetHiddenInGame(true);
	Mesh->GetBoneNames(MeshBones);
}

void ATNTongue::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DetachBase();
	DetachTip();

	Super::EndPlay(EndPlayReason);
}
