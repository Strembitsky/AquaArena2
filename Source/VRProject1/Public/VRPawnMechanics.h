// VRPawnMechanics.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "MotionControllerComponent.h" 
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMeshActor.h"
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector ballReleasePosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool CanPlayDragSound;

    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grabbing")
    // UGrabComponent* HeldComponentLeft;
    //
    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grabbing")
    // UGrabComponent* HeldComponentRight;

    //The sphere component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        USphereComponent* rootCollision;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    bool DiscItemGrabbed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    bool WrenchItemGrabbed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    bool SlenderItemGrabbed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    bool MilkItemGrabbed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    bool BearItemGrabbed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    bool MooseItemGrabbed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    bool BoxItemGrabbed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    bool FlashlightItemGrabbed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    bool UpdatePictureFrame;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    AStaticMeshActor* DiscPictureMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    AStaticMeshActor* WrenchPictureMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    AStaticMeshActor* SlenderPictureMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    AStaticMeshActor* MilkPictureMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    AStaticMeshActor* BearPictureMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    AStaticMeshActor* BoxPictureMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    AStaticMeshActor* MoosePictureMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    AStaticMeshActor* FlashlightPictureMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zipline")
    AStaticMeshActor* StartZipline;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zipline")
    AStaticMeshActor* EndZipline;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zipline")
    FVector ZiplineAxis;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zipline")
    FVector ZiplineDirection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zipline")
    float ZiplineLengthSquared;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    UMaterialInterface* DiscMat;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    UMaterialInterface* WrenchMat;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    UMaterialInterface* SlenderMat;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    UMaterialInterface* MilkMat;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    UMaterialInterface* BearMat;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    UMaterialInterface* BoxMat;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    UMaterialInterface* MooseMat;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
    UMaterialInterface* FlashlightMat;
    

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