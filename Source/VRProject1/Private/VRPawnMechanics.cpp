// VRPawnMechanics.cpp

#include "VRPawnMechanics.h"
#include "Components/AudioComponent.h"
#include "PowerManager.h"
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
    ResetGame = false;
    
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
    InitiateFall = false;

    TArray<AActor*> FoundPower;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerManager::StaticClass(), FoundPower);
    PowerManager = Cast<APowerManager>(FoundPower[0]);
    
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
    
    TArray<AActor*> FoundZipFall;  // Holds the actors found
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("FallFromZiplineTrigger"), FoundZipFall);
    ZiplineFall = Cast<ATriggerBox>(FoundZipFall[0]);
    ZiplineFall->OnActorBeginOverlap.AddDynamic(this, &AVRPawnMechanics::OnOverlapBeginZiplineFall);
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

    if (rootCollision)
    {
           currentPosition = rootCollision->GetComponentLocation();
           displacement = currentPosition - previousPosition;
           FVector newVelocity = displacement / DeltaTime;
           velocities.RemoveAt(0);
           velocities.Add(newVelocity);
           velocity = AverageVelocity(velocities);
           previousPosition = currentPosition; 
    }

    if (rightController)
    {
        currentRPosition = rightController->GetRelativeLocation();
        rDisplacement = currentRPosition - previousRPosition;
        FVector newRVelocity = rDisplacement / DeltaTime;
        rVelocities.RemoveAt(0);
        rVelocities.Add(newRVelocity);
        rVelocity = AverageVelocity(rVelocities);
        
        previousRPosition = currentRPosition;
    }

    if (leftController)
    {
        currentLPosition = leftController->GetRelativeLocation();
        lDisplacement = currentLPosition - previousLPosition;
        FVector newLVelocity = lDisplacement / DeltaTime;
        lVelocities.RemoveAt(0);
        lVelocities.Add(newLVelocity);
        lVelocity = AverageVelocity(lVelocities);
        previousLPosition = currentLPosition;
    }


    if (UpdatePictureFrame)
    {
        if (WrenchItemGrabbed)
        {
            if (WrenchPictureMesh)
            {
               WrenchPictureMesh->GetStaticMeshComponent()->SetMaterial(0, WrenchMat); 
            }
            
        }
        if (DiscItemGrabbed)
        {
            if (DiscPictureMesh)
            {
              DiscPictureMesh->GetStaticMeshComponent()->SetMaterial(0, DiscMat);  
            }
            
        }
        if (SlenderItemGrabbed)
        {
            if (SlenderPictureMesh)
            {
                SlenderPictureMesh->GetStaticMeshComponent()->SetMaterial(0, SlenderMat);
            }
        }
        if (MilkItemGrabbed)
        {
            if (MilkPictureMesh)
            {
                            MilkPictureMesh->GetStaticMeshComponent()->SetMaterial(0, MilkMat);

            }
        }
        if (BearItemGrabbed)
        {
            if (BearPictureMesh)
            {
                            BearPictureMesh->GetStaticMeshComponent()->SetMaterial(0, BearMat);

            }
        }
        if (MooseItemGrabbed)
        {
            if (MoosePictureMesh)
            {
                            MoosePictureMesh->GetStaticMeshComponent()->SetMaterial(0, MooseMat);

            }
        }
        if (BoxItemGrabbed)
        {
            if (BoxPictureMesh)
            {
                            BoxPictureMesh->GetStaticMeshComponent()->SetMaterial(0, BoxMat);

            }
        }
        if (FlashlightItemGrabbed)
        {
            if (FlashlightPictureMesh)
            {
                            FlashlightPictureMesh->GetStaticMeshComponent()->SetMaterial(0, FlashlightMat);

            }
        }
        UpdatePictureFrame = false;
    }
    
}

void AVRPawnMechanics::OnOverlapBeginZiplineFall(AActor* OverlappedActor, AActor* OtherActor)
{
    if (OtherActor->GetName().Contains("Pawn"))
    {
        if (!(WrenchItemGrabbed && DiscItemGrabbed && SlenderItemGrabbed && MilkItemGrabbed && BearItemGrabbed && MooseItemGrabbed && BoxItemGrabbed && FlashlightItemGrabbed))
        {
            InitiateFall = true;
            if (rootCollision)
            {
                            Cast<USphereComponent>(rootCollision)->SetSimulatePhysics(true);
                            Cast<USphereComponent>(rootCollision)->SetEnableGravity(true);
            }

            TArray<UAudioComponent*> AudioComponents;
            GetComponents<UAudioComponent>(AudioComponents);
            for(UAudioComponent* AudioComp : AudioComponents)
            {
                if(AudioComp->GetName().Equals("WindFall"))
                {
                    AudioComp->Play();
                }
            }
        }
    }
}
