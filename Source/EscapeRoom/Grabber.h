// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/InputComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESCAPEROOM_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	//	How far from the player we can reach in cm
	float Reach = 100.0f;
	


	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	UInputComponent* Input = nullptr;

	//	Ray-cast and grab what's in reach
	void Grab();
	void Release();

	//	Find attached physics component
	void FindPhysicsHandleComponent();

	//	Setup input component
	void SetupInputComponent();

	//	Return hit for first physics body in reach
	FHitResult GetFirstPhysicsBodyInReach() const;

	//	Retrieve the start and end points for the line trace being cast
	FTwoVectors GetLineTracePoints(FRotator& out_Rotation) const;

};
