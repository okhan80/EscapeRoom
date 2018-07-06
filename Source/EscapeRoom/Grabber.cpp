// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "Engine/World.h"

#define OUT	//	Annotation to remind us that values are being modified through reference modification

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UGrabber::FindPhysicsHandleComponent()
{
	///	Look for attached physics handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (ensureMsgf(PhysicsHandle, TEXT("%s is missing a UPhysicsHandleComponent. Please add this component to continue."),
		*GetOwner()->GetName())) { }

}

void UGrabber::SetupInputComponent()
{
	///	Look for attached input component
	Input = GetOwner()->FindComponentByClass<UInputComponent>();
	if (ensureMsgf(Input, TEXT("%s is missing a UInputComponent. Please add this component to continue."),
		*GetOwner()->GetName()))
	{
		/// Bind the input actions
		Input->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		Input->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	///	Setup query parameters
	FHitResult HitResult;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	FRotator CurrentRotation;
	FTwoVectors TracePoints = GetLineTracePoints(CurrentRotation);

	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		TracePoints.v1,
		TracePoints.v2,
		FCollisionObjectQueryParams(ECC_PhysicsBody),
		TraceParameters
	);

	return HitResult;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandleComponent();
	SetupInputComponent();

}

FTwoVectors UGrabber::GetLineTracePoints(FRotator& out_Rotation) const
{
	FVector StartLocation;
	FRotator PlayerRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT StartLocation, OUT PlayerRotation);
	FVector EndLocation = StartLocation + PlayerRotation.Vector() * Reach;
	out_Rotation = PlayerRotation;
	return FTwoVectors(StartLocation, EndLocation);
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) { return; }
	if (PhysicsHandle->GrabbedComponent)
	{
		//	move the object we are holding
		FRotator CurrentRotation;
		FVector TargetLocation = GetLineTracePoints(OUT CurrentRotation).v2;
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, CurrentRotation);
	}
}

void UGrabber::Grab()
{
	///	Line trace and try and reach any actors with physics body collision channels set
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorToGrab = HitResult.GetActor();
	
	///	If we hit something then attach a physics handle
	if (ActorToGrab)
	{
		if (!PhysicsHandle) { return; }
		PhysicsHandle->GrabComponentAtLocationWithRotation(ComponentToGrab, 
			NAME_None, 
			ActorToGrab->GetActorLocation(), 
			ActorToGrab->GetActorRotation()
		);
	}

}

void UGrabber::Release()
{
	if (!PhysicsHandle) { return; }
	PhysicsHandle->ReleaseComponent();
}

