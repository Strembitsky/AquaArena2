// MyPlayerController.cpp

#include "MyPlayerController.h"
#include "MyPlayerCameraManager.h"

AMyPlayerController::AMyPlayerController()
{
    // Initialize the component objects
    vrRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
    vrOffset = CreateDefaultSubobject<USceneComponent>(TEXT("VROffset"));
    vrCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VR Camera"));
    vrOffset->SetupAttachment(vrRoot);
    vrCamera->SetupAttachment(vrOffset);
    // Attach the components to the root
    LastBoostTime = 0.0f;
    bIsBoosting = false;
    PlayerCameraManagerClass = AMyPlayerCameraManager::StaticClass();
}

void AMyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (bpRef)
    {
        for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
        {
            if (ActorItr->IsA(bpRef))
            {
                VRPawnInstance = Cast<AVRPawnMechanics>(*ActorItr);
                // Now you have a reference to the VRPawn instance in your world.
            }
        }
    }
    
    if (VRPawnInstance)
    {
        rootComp = VRPawnInstance->GetRootComponent();
        controllerOrigin = nullptr;
        TArray<USceneComponent*> SceneComponents;
        VRPawnInstance->GetComponents<USceneComponent>(SceneComponents);
        for (USceneComponent* Component : SceneComponents)
        {
            if (Component->GetName() == "controllerOrigin")
            {
                controllerOrigin = Component;
            }

            if (Component->GetName() == "Head")
            {
                head = Component;
            }

            // if (Component->GetName() == "Camera1")
            // {
            //     vrCamera = Cast<UCameraComponent>(Component);
            // }

            // UCameraComponent* CameraComponent = VRPawnInstance->FindComponentByClass<UCameraComponent>();
            // if (CameraComponent)
            // {
            //     vrCamera = CameraComponent;
            // }
            
        }
        VRPawnMovement = Cast<UFloatingPawnMovement>(VRPawnInstance->GetMovementComponent());
        if (VRPawnMovement)
        {
            UE_LOG(LogTemp, Warning, TEXT("FOUND FLOATING PAWN MOVEMENT"));
        }
        TArray<UMotionControllerComponent*> MotionControllerComponents;
        VRPawnInstance->GetComponents<UMotionControllerComponent>(MotionControllerComponents);
        for (UMotionControllerComponent* MotionControllerComponent : MotionControllerComponents)
        {
            if (MotionControllerComponent->MotionSource == "LeftAim")
            {
                LHand = MotionControllerComponent;
            }
            else if (MotionControllerComponent->MotionSource == "RightAim")
            {
                RHand = MotionControllerComponent;
            }
        }
    }
    
    if (rootComp && controllerOrigin) //&& vrCamera)
    {
        //vrOffset->AttachToComponent(vrCamera, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
        vrRoot->AttachToComponent(rootComp, FAttachmentTransformRules::KeepRelativeTransform);
        controllerOrigin->AttachToComponent(vrRoot, FAttachmentTransformRules::KeepRelativeTransform);
        //vrOffset->AttachToComponent(vrCamera, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
    }

    rootComp->SetWorldLocation(vrCamera->GetRelativeLocation(), false, nullptr, ETeleportType::TeleportPhysics);
    //this->AttachToActor(VRPawnInstance, FAttachmentTransformRules::KeepWorldTransform);
    
}

void AMyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
    if (Input)
    {
        Input->BindAction(RWristThrustAction, ETriggerEvent::Triggered, this, &AMyPlayerController::HandleRWristThrust);
        Input->BindAction(LWristThrustAction, ETriggerEvent::Triggered, this, &AMyPlayerController::HandleLWristThrust);
        Input->BindAction(RWristThrustAction, ETriggerEvent::Completed, this, &AMyPlayerController::HandleRWristRelease);
        Input->BindAction(LWristThrustAction, ETriggerEvent::Completed, this, &AMyPlayerController::HandleLWristRelease);
        Input->BindAction(BoostAction, ETriggerEvent::Started, this, &AMyPlayerController::HandleBoost);
        // Input->BindAction(LGrabAction, ETriggerEvent::Triggered, this, &AVRPawnMechanics::HandleLClimb);
    }
}

void AMyPlayerController::HandleBoost(const FInputActionInstance& Instance)
{
    if (BoostCooldown >= 51.0f)
    {
        // Get the forward vector of the camera
        FVector Forward = vrCamera->GetForwardVector();
        // Calculate the thrust force
        FVector Thrust = Forward * BoostForce;
        // Apply the thrust
        VRPawnMovement->Velocity = ClampVelocity(VRPawnMovement->Velocity, Thrust, BoostMaxSpeed);

        BoostCooldown -= 51.0f;
    }
}

void AMyPlayerController::HandleRWristThrust(const FInputActionInstance& Instance)
{
    bIsRightWristThrusting = true;
}

void AMyPlayerController::HandleLWristThrust(const FInputActionInstance& Instance)
{
    bIsLeftWristThrusting = true;
}

void AMyPlayerController::HandleLWristRelease(const FInputActionInstance& Instance)
{
    bIsLeftWristThrusting = false;
}

void AMyPlayerController::HandleRWristRelease(const FInputActionInstance& Instance)
{
    bIsRightWristThrusting = false;
}

FVector AMyPlayerController::ClampVelocity(FVector CurrentVelocity, FVector Thrust, float MaxSpeed)
{
    FVector CurrentDirection = CurrentVelocity.GetSafeNormal();
    float CurrentSpeed = CurrentVelocity.Size();

    // Decompose the thrust into components parallel and perpendicular to the current velocity
    float ParallelThrustMagnitude = FVector::DotProduct(Thrust, CurrentDirection);
    FVector ParallelThrust = ParallelThrustMagnitude * CurrentDirection;
    FVector PerpendicularThrust = Thrust - ParallelThrust;

    // If the player is moving faster than MaxSpeed and the thrust is in the same direction as the current velocity,
    // then we don't want to slow the player down, so we ignore the parallel component of the thrust
    if (CurrentSpeed > MaxSpeed && ParallelThrustMagnitude > 0)
    {
        ParallelThrust = FVector::ZeroVector;
    }

    // Apply the parallel and perpendicular components of the thrust
    FVector NewVelocity = CurrentVelocity + ParallelThrust + PerpendicularThrust;

    // If the new speed is greater than the max speed, clamp it
    float NewSpeed = NewVelocity.Size();
    if (NewSpeed > FMath::Max(CurrentSpeed, MaxSpeed))
    {
        NewVelocity = FMath::Max(CurrentSpeed, MaxSpeed) * NewVelocity.GetSafeNormal();
    }

    return NewVelocity;
}

// Called every frame
void AMyPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (vrRoot && vrCamera)
    {
        FVector hmdDelta = vrOffset->GetRelativeLocation() - vrCamera->GetRelativeLocation();
        FVector rootPos = vrRoot->GetRelativeLocation();
        vrRoot->AddRelativeLocation(hmdDelta - rootPos, true, nullptr, ETeleportType::None);
        rootComp->AddRelativeLocation(-(hmdDelta - rootPos), true, nullptr, ETeleportType::None);
    }

    // Update boost cooldown
    if (BoostCooldown < 100.0f)
    {
        BoostCooldown += 0.135f;
    }
    else if (BoostCooldown != 100.f)
    {
        BoostCooldown = 100.0f;  // Ensure it doesn't exceed 100 due to floating point precision errors
    }
    //UE_LOG(LogTemp, Warning, TEXT("Boost Cooldown: %f"), BoostCooldown);
    
    if (bIsLeftWristThrusting || bIsRightWristThrusting) 
    {
        FVector Forward;
        float MaxSpeed;
        FVector Thrust;
        
        if (VRPawnMovement)
        {
            if (bIsLeftWristThrusting && bIsRightWristThrusting)
            {
                Forward = (LHand->GetForwardVector() + RHand->GetForwardVector()).GetSafeNormal();
                MaxSpeed = WristMaxSpeed;
                Thrust = Forward * WristSpeed * 2.0f * GetWorld()->GetDeltaSeconds();
            }
            else if (bIsRightWristThrusting)
            {
                Forward = RHand->GetForwardVector().GetSafeNormal();
                MaxSpeed = WristMaxSpeed * 0.5f;
                Thrust = Forward * WristSpeed * GetWorld()->GetDeltaSeconds();
            }
            else if (bIsLeftWristThrusting)
            {
                Forward = LHand->GetForwardVector().GetSafeNormal();
                MaxSpeed = WristMaxSpeed * 0.5f;
                Thrust = Forward * WristSpeed * GetWorld()->GetDeltaSeconds();
            }
            else
            {
                // Neither thruster is active, so we don't need to do anything
                return;
            }

            VRPawnMovement->Velocity = ClampVelocity(VRPawnMovement->Velocity, Thrust, MaxSpeed);
        }
    }
    
}

void AMyPlayerController::CalcCamera(float dt, FMinimalViewInfo& outResult)
{
    //Super::CalcCamera(dt, outResult);
    if (vrCamera)
    {
        vrCamera->GetCameraView(dt, outResult);
    }
}