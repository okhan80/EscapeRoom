// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#define OUT	//	Annotation to remind us that values are being modified through reference modification

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Grabber reporting for duty!"));

	///	Look for attached physics handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (ensureMsgf(PhysicsHandle, TEXT("%s is missing a UPhysicsHandleComponent. Please add this component to continue."),
			*GetOwner()->GetName()))
	{
		//	Physics handle is found
	}
	//else
	//{
	//	//UE_LOG(LogTemp, Error, TEXT("%s is missing a UPhysicsHandleComponent. Please add this component to continue."),
	//	//	*GetOwner()->GetName()
	//	//);
	//	//	This will crash out the editor, not a good way to check. Instead use ensure
	//	//checkf(Input, TEXT("%s is missing a UPhysicsHandleComponent. Please add this component to continue."),
	//	//	*GetOwner()->GetName());
	//}

	///	Look for attached input component
	Input = GetOwner()->FindComponentByClass<UInputComponent>();
	if (ensureMsgf(Input, TEXT("%s is missing a UInputComponent. Please add this component to continue."),
		*GetOwner()->GetName()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Input component found on %s"), *GetOwner()->GetName());

		/// Bind the input axis
		Input->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
	}

}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	///	Get player viewpoint this tick (Triple slash will get rid of documentation)
	FVector PlayerPosition;
	FRotator PlayerRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerPosition, OUT PlayerRotation);

	FVector LineTraceEnd = PlayerPosition + PlayerRotation.Vector() * Reach;

	///	Draw red trace in world to visualize
	DrawDebugLine(GetWorld(), PlayerPosition, LineTraceEnd, FColor::Red, false, -1.0f, 0.0f, 10.0f);

	///	Setup query parameters
	FHitResult LineTraceHit;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT LineTraceHit,
		PlayerPosition,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECC_PhysicsBody),
		TraceParameters
	);
	///	See what we hit
	AActor* ActorHit = LineTraceHit.GetActor();
	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit: %s"), *ActorHit->GetName());
	}
	

}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab key pressed"));
}

