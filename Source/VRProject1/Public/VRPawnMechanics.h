// VRPawnMechanics.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h" 
#include "GameFramework/FloatingPawnMovement.h"
#include "MotionControllerComponent.h" 
#include "EngineUtils.h"
#include "GameFramework/InputSettings.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "VRPawnMechanics.generated.h"

UCLASS()
class VRPROJECT1_API AVRPawnMechanics : public APawn
{
    GENERATED_BODY()

public:
    AVRPawnMechanics();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    USphereComponent* HeadCollision;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    UCameraComponent* Camera;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    USceneComponent* VROrigin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    USceneComponent* head;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    USceneComponent* leftHand;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    USceneComponent* rightHand;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    UMotionControllerComponent* leftController;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    UMotionControllerComponent* rightController;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    UFloatingPawnMovement* FloatingPawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector previousPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector currentPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector displacement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector velocity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector previousRPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector currentRPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector rDisplacement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector rVelocity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector previousLPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector currentLPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector lDisplacement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector lVelocity;

    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grabbing")
    // UGrabComponent* HeldComponentLeft;
    //
    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grabbing")
    // UGrabComponent* HeldComponentRight;

    //The sphere component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        USphereComponent* rootCollision;

    // void AVRPawnMechanics::HandleLClimb(const FInputActionInstance& Instance);

    // Tick function declaration
    virtual void Tick(float DeltaTime) override;

private:
    static const int FrameCount = 10;
    TArray<FVector> velocities;
    TArray<FVector> rVelocities;
    TArray<FVector> lVelocities;

    FVector AverageVelocity(const TArray<FVector>& velocityArray) const;


protected:
    virtual void BeginPlay() override;
};