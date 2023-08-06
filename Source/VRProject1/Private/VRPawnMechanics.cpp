// VRPawnMechanics.cpp

#include "VRPawnMechanics.h"

#include "Kismet/GameplayStatics.h"

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

    CanPlayDragSound = false;
    
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

    UpdatePictureFrame = false;
    DiscItemGrabbed = false;
    WrenchItemGrabbed = false;
    SlenderItemGrabbed = false;
    MilkItemGrabbed = false;
    BearItemGrabbed = false;
    MooseItemGrabbed = false;
    BoxItemGrabbed = false;
    FlashlightItemGrabbed = false;
    
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

    TArray<AActor*> FoundStartZip;  // Holds the actors found
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("StartZipline"), FoundStartZip);
    StartZipline = Cast<AStaticMeshActor>(FoundStartZip[0]);
    if (StartZipline)
    {
        UE_LOG(LogTemp, Warning, TEXT("found start"));
    }

    TArray<AActor*> FoundEndZip;  // Holds the actors found
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("EndZipline"), FoundEndZip);
    EndZipline = Cast<AStaticMeshActor>(FoundEndZip[0]);
    if (EndZipline)
    {
        UE_LOG(LogTemp, Warning, TEXT("found end"));
    }

    ZiplineAxis = (EndZipline->GetActorLocation() - StartZipline->GetActorLocation()).GetSafeNormal();
    ZiplineDirection = EndZipline->GetActorLocation() - StartZipline->GetActorLocation();
    ZiplineLengthSquared = ZiplineDirection.SizeSquared();
    
    TArray<AActor*> FoundActors;  // Holds the actors found
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Picture"), FoundActors);
    // You can now cast the actors to AStaticMeshActor* and add them to your Pictures array
    for (AActor* Actor : FoundActors)
    {
        AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(Actor);
        if (StaticMeshActor->ActorHasTag("Disc"))
        {
            DiscPictureMesh = StaticMeshActor;
        }
        else if (StaticMeshActor->ActorHasTag("Wrench"))
        {
            WrenchPictureMesh = StaticMeshActor;
        }
        else if (StaticMeshActor->ActorHasTag("Slenderman"))
        {
            SlenderPictureMesh = StaticMeshActor;
        }
        else if (StaticMeshActor->ActorHasTag("Milk"))
        {
            MilkPictureMesh = StaticMeshActor;
        }
        else if (StaticMeshActor->ActorHasTag("Bear"))
        {
            BearPictureMesh = StaticMeshActor;
        }
        else if (StaticMeshActor->ActorHasTag("Box"))
        {
            BoxPictureMesh = StaticMeshActor;
        }
        else if (StaticMeshActor->ActorHasTag("Moose"))
        {
            MoosePictureMesh = StaticMeshActor;
        }
        else if (StaticMeshActor->ActorHasTag("Flashlight"))
        {
            FlashlightPictureMesh = StaticMeshActor;
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

FVector AVRPawnMechanics::AverageVelocity(const TArray<FVector>& velocityArray) const
{
    FVector totalVelocity(0.f, 0.f, 0.f);
    float totalWeight = 0.f;
    int arrayLength = velocityArray.Num();
    float normalWeight = 1.f; // The weight for the newer velocities
    float extraWeight = 1.5f; // The weight for the oldest velocities is now double

    for (int i = 0; i < arrayLength; i++)
    {
        float weight = normalWeight;
        if (i < 5) // For the oldest 5 velocities
            {
            weight = extraWeight;
            }

        totalVelocity += weight * velocityArray[i];
        totalWeight += weight;
    }

    if (totalWeight == 0) return FVector::ZeroVector; // Avoid division by zero
    return totalVelocity / totalWeight;
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

    if (UpdatePictureFrame)
    {
        if (WrenchItemGrabbed)
        {
            WrenchPictureMesh->GetStaticMeshComponent()->SetMaterial(0, WrenchMat);
        }
        if (DiscItemGrabbed)
        {
            DiscPictureMesh->GetStaticMeshComponent()->SetMaterial(0, DiscMat);
        }
        if (SlenderItemGrabbed)
        {
            SlenderPictureMesh->GetStaticMeshComponent()->SetMaterial(0, SlenderMat);
        }
        if (MilkItemGrabbed)
        {
            MilkPictureMesh->GetStaticMeshComponent()->SetMaterial(0, MilkMat);
        }
        if (BearItemGrabbed)
        {
            BearPictureMesh->GetStaticMeshComponent()->SetMaterial(0, BearMat);
        }
        if (MooseItemGrabbed)
        {
            MoosePictureMesh->GetStaticMeshComponent()->SetMaterial(0, MooseMat);
        }
        if (BoxItemGrabbed)
        {
            BoxPictureMesh->GetStaticMeshComponent()->SetMaterial(0, BoxMat);
        }
        if (FlashlightItemGrabbed)
        {
            FlashlightPictureMesh->GetStaticMeshComponent()->SetMaterial(0, FlashlightMat);
        }
        UpdatePictureFrame = false;
    }
    
}
