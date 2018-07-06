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
		*GetOwner()->GetName()))
	{
		//	Physics handle is found
	}	//else
		//{
		//	//UE_LOG(LogTemp, Error, TEXT("%s is missing a UPhysicsHandleComponent. Please add this component to continue."),
		//	//	*GetOwner()->GetName()
		//	//);
		//	//	This will crash out the editor, not a good way to check. Instead use ensure
		//	//checkf(Input, TEXT("%s is missing a UPhysicsHandleComponent. Please add this component to continue."),
		//	//	*GetOwner()->GetName());
		//}

}

void UGrabber::SetupInputComponent()
{
	///	Look for attached input component
	Input = GetOwner()->FindComponentByClass<UInputComponent>();
	if (ensureMsgf(Input, TEXT("%s is missing a UInputComponent. Please add this component to continue."),
		*GetOwner()->GetName()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Input component found on %s"), *GetOwner()->GetName());

		/// Bind the input actions
		Input->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		Input->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	///	Get player viewpoint this tick (Triple slash will get rid of documentation)
	FVector PlayerPosition;
	FRotator PlayerRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerPosition, OUT PlayerRotation);

	FVector LineTraceEnd = PlayerPosition + PlayerRotation.Vector() * Reach;

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

	return LineTraceHit;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Grabber reporting for duty!"));

	FindPhysicsHandleComponent();
	SetupInputComponent();

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
	//	If the physics handle is attached
	if (PhysicsHandle->GrabbedComponent)
	{
		//	move the object we are holding
		PhysicsHandle->SetTargetLocationAndRotation(LineTraceEnd, PlayerRotation);
	}
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab key pressed"));

	///	Line tracne and try and erach any actors with physics body collision channels set
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorToGrab = HitResult.GetActor();
	
	///	If we hit something then attach a physics handle
	if (ActorToGrab)
	{
		PhysicsHandle->GrabComponentAtLocationWithRotation(ComponentToGrab, 
			NAME_None, 
			ActorToGrab->GetActorLocation(), 
			ActorToGrab->GetActorRotation()
		);
	}

}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Key has been released"));
	PhysicsHandle->ReleaseComponent();
}

