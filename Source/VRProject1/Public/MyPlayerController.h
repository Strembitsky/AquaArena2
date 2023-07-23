// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "VRPawnMechanics.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class VRPROJECT1_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPlayerController();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float WristSpeed = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float WristMaxSpeed = 300.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* LWristThrustAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* RWristThrustAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* BoostAction;
    
    UPROPERTY(EditAnywhere, Category = "Motion Controller")
    UMotionControllerComponent* LHand;

    UPROPERTY(EditAnywhere, Category = "Motion Controller")
    UMotionControllerComponent* RHand;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    UFloatingPawnMovement* VRPawnMovement;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* vrRoot;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* head;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
        UCameraComponent* vrCamera;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
        USceneComponent* vrOffset;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
        AVRPawnMechanics* VRPawnInstance;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        TSubclassOf<AVRPawnMechanics> bpRef;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        USceneComponent* rootComp;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    USceneComponent* controllerOrigin;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool bIsLeftWristThrusting;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool bIsRightWristThrusting;

    virtual void SetupInputComponent() override;

    void HandleRWristThrust(const FInputActionInstance&);
    void HandleLWristThrust(const FInputActionInstance&);
    void HandleRWristRelease(const FInputActionInstance&);
    void HandleLWristRelease(const FInputActionInstance&);
    void HandleBoost(const FInputActionInstance&);
    
    FVector ClampVelocity(FVector CurrentVelocity, FVector Thrust, float MaxSpeed);

	virtual void CalcCamera(float dt, FMinimalViewInfo& outResult) override;
    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

private:
    bool bIsBoosting;
    float BoostCooldown;
    float LastBoostTime;
    const float BoostForce = 350.0f;
    const float BoostMaxSpeed = 400.0f;
    const float BoostCooldownDuration = 3.0f;
};
