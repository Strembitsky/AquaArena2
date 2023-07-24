// VRPawnMechanics.cpp

#include "VRPawnMechanics.h"

#include "GenericPlatform/GenericPlatformCrashContext.h"

AVRPawnMechanics::AVRPawnMechanics()
{

    PrimaryActorTick.bCanEverTick = true;

    rootCollision = CreateDefaultSubobject<USphereComponent>("SphereComp");
    rootCollision->InitSphereRadius(12.5f);
    rootCollision->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

    rootCollision->CanCharacterStepUpOn = ECB_No;
    rootCollision->SetShouldUpdatePhysicsVolume(true);
    rootCollision->SetCanEverAffectNavigation(false);

    RootComponent = rootCollision;

    head = CreateDefaultSubobject<USceneComponent>("Head");
    head->SetupAttachment(rootCollision);

    leftHand = CreateDefaultSubobject<USceneComponent>("LeftHand");
    leftHand->SetupAttachment(rootCollision);

    rightHand = CreateDefaultSubobject<USceneComponent>("RightHand");
    rightHand->SetupAttachment(rootCollision);
    
    velocities.SetNumZeroed(FrameCount);
    rVelocities.SetNumZeroed(FrameCount);
    lVelocities.SetNumZeroed(FrameCount);
    
}

void AVRPawnMechanics::BeginPlay()
{
    Super::BeginPlay();
    //Camera = FindComponentByClass<UCameraComponent>();
    VROrigin = FindComponentByClass<USceneComponent>();
    //rootCollision->SetHiddenInGame(false);
    FloatingPawn = Cast<UFloatingPawnMovement>(GetMovementComponent());
    leftController = nullptr;
    rightController = nullptr;
    TArray<UMotionControllerComponent*> controllers;
    GetComponents<UMotionControllerComponent>(controllers);
    for (UMotionControllerComponent* controller : controllers)
    {
        if (controller->MotionSource == "Left")
        {
            leftController = controller;
        }
        else if (controller->MotionSource == "Right")
        {
            rightController = controller;
        }
    }

    if (leftController == nullptr || rightController == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find left or right controller"));
    }
    previousLPosition = leftController->GetRelativeLocation();
    previousRPosition = rightController->GetRelativeLocation();
    previousPosition = rootCollision->GetRelativeLocation();
}

// void AVRPawnMechanics::HandleLClimb(const FInputActionInstance& Instance)
// {
//     if (HeldComponentLeft.IsValid())
//     {
//         // Already holding something, we might want to just return
//         return;
//     }
// }

FVector AVRPawnMechanics::AverageVelocity(const TArray<FVector>& velocityArray) const
{
    FVector totalVelocity(0.f, 0.f, 0.f);

    for (const FVector& tempVelocity : velocityArray)
    {
        totalVelocity += tempVelocity;
    }

    return totalVelocity / static_cast<float>(FrameCount);
}

void AVRPawnMechanics::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    currentPosition = rootCollision->GetComponentLocation();
    displacement = currentPosition - previousPosition;
    FVector newVelocity = displacement / DeltaTime;
    velocities.RemoveAt(0);
    velocities.Add(newVelocity);
    velocity = AverageVelocity(velocities);
    previousPosition = currentPosition;

    currentRPosition = rightController->GetRelativeLocation();
    rDisplacement = currentRPosition - previousRPosition;
    FVector newRVelocity = rDisplacement / DeltaTime;
    rVelocities.RemoveAt(0);
    rVelocities.Add(newRVelocity);
    rVelocity = AverageVelocity(rVelocities);
    
    previousRPosition = currentRPosition;

    currentLPosition = leftController->GetRelativeLocation();
    lDisplacement = currentLPosition - previousLPosition;
    FVector newLVelocity = lDisplacement / DeltaTime;
    lVelocities.RemoveAt(0);
    lVelocities.Add(newLVelocity);
    lVelocity = AverageVelocity(lVelocities);
    previousLPosition = currentLPosition;
}
