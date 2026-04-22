// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CustomMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ClimbingSystem/ClimbingSystemCharacter.h"

#include "ClimbingSystem/DebugHelper.h"
void UCustomMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*TraceClimbableSurfaces();
	TraceFormEyeHeight(100.f);*/
}

#pragma region ClimbTraces

TArray<FHitResult> UCustomMovementComponent::DoCapsuleTraceMultiByObject(const FVector& Start, const FVector& End, bool bShowDebugShape, bool bDrawPersistantShapes)
{
	TArray<FHitResult> OutCapsuleTraceHitResult;

	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;

	if (bShowDebugShape)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;

		if (bDrawPersistantShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}

	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		this,
		Start,
		End,
		ClimbCapsuleTraceRadius,
		ClimbCapsuleTarceHalfHeight,
		ClimbableSurfaceTraceTypes,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		OutCapsuleTraceHitResult,
		false
	);

	return OutCapsuleTraceHitResult;
}

FHitResult UCustomMovementComponent::DoLineTraceSingleByObject(const FVector& Start, const FVector& End, bool bShowDebugShape,bool bDrawPersistantShapes)
{
	FHitResult OutHit;

	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;

	if(bShowDebugShape)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;

		if(bDrawPersistantShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}

	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		ClimbableSurfaceTraceTypes,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		OutHit,
		false
	);
		return OutHit;
}

#pragma endregion


#pragma region ClimbCore

void UCustomMovementComponent::ToggleClimbing(bool bEnableClimb)
{
	if(bEnableClimb)
	{
		if(CanStartClimbing())
		{
			Debug::Print(TEXT("Can start climbing"));
		}
		else
		{
			Debug::Print(TEXT("Can NOT start climbing"));
		}
	}else
	{
		
	}
}

bool UCustomMovementComponent::CanStartClimbing()
{
	if (IsFalling()) return false;
	if (!TraceClimbableSurfaces()) return false;
	if (!TraceFormEyeHeight(100.f).bBlockingHit) return false;

	return true;
}

bool UCustomMovementComponent::IsClimbing() const
{

	return MovementMode == MOVE_Custom && CustomMovementMode == ECustomMovementMode::MOVE_Climb;
	
}

//Trace for climbable surfaces,return true if there are indeed valid surfaces,false otherwis
bool UCustomMovementComponent::TraceClimbableSurfaces()
{
	const FVector StartOff = UpdatedComponent->GetForwardVector() * 30.f;
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOff;
	const FVector End = Start + UpdatedComponent->GetForwardVector();
		
	ClimbableSurfacesTracedResults = DoCapsuleTraceMultiByObject(Start,End,true,true);

	return !ClimbableSurfacesTracedResults.IsEmpty();
}

FHitResult UCustomMovementComponent::TraceFormEyeHeight(float TraceDistance, float TraceStartOffset)
{
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
	const FVector EyeHeightOffset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight+ TraceStartOffset);

	const FVector Start = ComponentLocation + EyeHeightOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector() * TraceDistance;


	return DoLineTraceSingleByObject(Start, End, true,true);
}

#pragma endregion