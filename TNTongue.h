#pragma once
#include "GameFramework/Actor.h"
#include "TNTongue.generated.h"

class USceneComponent;
class UPoseableMeshComponent;
class ATNCharacter;

UCLASS()
class TN_API ATNTongue : public AActor
{
	GENERATED_BODY()

public:
	ATNTongue();

	// The length at which the tongue exerts no force.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tongue")
	float TongueLength = 200.f;

	// Constant to be used for the inner friction of the tongue.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tongue")
	float Friction = 0.f;

	// The tightness of the tongue.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tongue")
	float Stiffness = 1000.f;

	// Whether to keep the distance at tongue length, the objects will repel in addition to pull.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tongue")
	bool bKeepDistance = false;

	// The interpolation rate of the tongue mesh, less is more loose.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tongue")
	float MeshInterpRate = 10.f;

	// Attaches the first component to an Actor.
	UFUNCTION(BlueprintCallable)
	void AttachTip(UPrimitiveComponent* Component, const FVector Location, FName Socket = NAME_None);

	// Attaches the second component to an Actor.
	UFUNCTION(BlueprintCallable)
	void AttachBase(UPrimitiveComponent* Component, const FVector Location, FName Socket = NAME_None);

	// Detaches the first component if attached, automatically called by AttachFirst.
	UFUNCTION(BlueprintCallable)
	void DetachTip();

	// Detaches the second component if attached, automatically called by AttachSecond.
	UFUNCTION(BlueprintCallable)
	void DetachBase();

	// Resets the mesh pose.
	void ResetMeshPose();

	// Gets the location of the first attachment point.
	UFUNCTION(BlueprintPure)
	FORCEINLINE FVector GetTipLocation() const { return Tip->GetComponentLocation(); }

	// Gets the location of the second attachment point.
	UFUNCTION(BlueprintPure)
	FORCEINLINE FVector GetBaseLocation() const { return Base->GetComponentLocation(); }

	// Gets the component the first attachment point is attached to, if any.
	UFUNCTION(BlueprintPure)
	FORCEINLINE UPrimitiveComponent* GetTipAttached() const { return TipAttached; }

	// Gets the component the second attachment point is attached to, if any.
	UFUNCTION(BlueprintPure)
	FORCEINLINE UPrimitiveComponent* GetBaseAttached() const { return BaseAttached; }

protected:
	// Acts as the tip of the tongue, last bone of the mesh.
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Tip;

	// Acts as the base of the tongue, first bone of the mesh.
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Base;

	// The mesh to stretch between the points.
	UPROPERTY(VisibleAnywhere)
	UPoseableMeshComponent* Mesh;

	UPROPERTY()
	UPrimitiveComponent* TipAttached;
	
	UPROPERTY()
	UPrimitiveComponent* BaseAttached;
	
	TArray<FName> MeshBones;

	void PoseMesh();
	FVector GetSpringForce(const FVector APos, const FVector BPos, const FVector AVel, const FVector BVel);
	void ApplySpringForce(UPrimitiveComponent* Component, FVector Location, FVector Force);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};