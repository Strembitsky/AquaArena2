// Fill out your copyright notice in the Description page of Project Settings.

#include "PowerManager.h"
#include "ScoreManager.h"
#include "VRPawnMechanics.h"
#include "MyPlayerController.h"
#include "Components/AudioComponent.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
APowerManager::APowerManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APowerManager::BeginPlay()
{
    Super::BeginPlay();

    Moonlight->SetEnabled(false);
    
    for (AActor* Actor : MilkActors)
    {
        if (Actor)
        {
            UInstancedStaticMeshComponent* InstancedMeshComponent = NewObject<UInstancedStaticMeshComponent>(this);
            TArray<AActor*> ChildActors;
            Actor->GetAllChildActors(ChildActors);
            for (AActor* Child : ChildActors)
            {
                AStaticMeshActor* MeshActor = Cast<AStaticMeshActor>(Child);
                if (MeshActor)
                {
                    // Get the actor's transform
                    FTransform ActorTransform = MeshActor->GetActorTransform();
                    // Add an instance to the InstancedMeshComponent with the actor's transform
                    InstancedMeshComponent->AddInstance(ActorTransform);
                    // Destroy the mesh actor
                    MeshActor->Destroy();
                }
            }
            // Only destroy the actor after all its children have been processed
            Actor->Destroy();
        }
    }
    for (AActor* Actor : BuildingActors)
    {
        if (Actor)
        {
            UInstancedStaticMeshComponent* InstancedMeshComponent = NewObject<UInstancedStaticMeshComponent>(this);
            TArray<AActor*> ChildActors;
            Actor->GetAllChildActors(ChildActors);
            for (AActor* Child : ChildActors)
            {
                AStaticMeshActor* MeshActor = Cast<AStaticMeshActor>(Child);
                if (MeshActor)
                {
                    // Get the actor's transform
                    FTransform ActorTransform = MeshActor->GetActorTransform();
                    // Add an instance to the InstancedMeshComponent with the actor's transform
                    InstancedMeshComponent->AddInstance(ActorTransform);
                    // Destroy the mesh actor
                    MeshActor->Destroy();
                }
            }
            // Only destroy the actor after all its children have been processed
            Actor->Destroy();
        }
    }
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVRPawnMechanics::StaticClass(), FoundActors);
    BeginTransition = false;
    MusicPlayed = false;
    PlayMusic = false;
    ElevatorOpened = false;
    GravityEnabled = false;
    ArenaCloseWall->SetActorEnableCollision(false);
    LivingRoomSealWall->SetActorEnableCollision(false);
    SlowDownMusic = false;
    SlowDownMusic2 = false;
    SpeedMusicUp1 = false;
    Splatted = false;
    paintedDecal = true;
    SplatMoved = false;
    EnableGravityTrigger->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnOverlapEnableGravity);
    SplatTrigger->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnOverlapBeginSplat);
    ArenaResetTrigger->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnOverlapResetGame);
    BloodSplatter->SetActorHiddenInGame(true);
    GravityDisabled = false;
    GoDisableGravity = false;

    if (FoundActors.Num() > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("FOUND VRPAWN IN POWERMANAGER"));
        AVRPawnMechanics* FoundVRPawn = Cast<AVRPawnMechanics>(FoundActors[0]);

        if (FoundVRPawn != nullptr)
        {
            // Now you can use FoundVRPawn as a reference to your VRPawn class.
            VRPawn = FoundVRPawn;
            PawnRoot = VRPawn->GetRootComponent();
        }
    }

    for (AStaticMeshActor* Actor : SecondPhaseHallwayAdditions)
    {
        Actor->SetActorHiddenInGame(true);
    }

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyPlayerController::StaticClass(), FoundActors);

    if (FoundActors.Num() > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("FOUND VRController IN POWERMANAGER"));
        AMyPlayerController* FoundVRController = Cast<AMyPlayerController>(FoundActors[0]);

        if (FoundVRController != nullptr)
        {
            // Now you can use FoundVRPawn as a reference to your VRPawn class.
            VRPlayerController = FoundVRController;
            VRRoot = VRPlayerController->GetRootComponent();
        }
    }
    
    PowerIsOn = true;
    GoalsSwapped = false;
    InitBGoalPos = BlueGoalRim->GetActorTransform();
    InitOGoalPos = OrangeGoalRim->GetActorTransform();
    InitOLightPos = OrangeLight->GetActorTransform();
    InitBLightPos = BlueLight->GetActorTransform();
    InitBallPos = Ball->GetActorTransform();
    InitGenPos = GeneratorActor->GetActorTransform();

    for (AStaticMeshActor* Button : ButtonArray)
    {
        InitialButtonPositions.Add(Button, Button->GetActorLocation());
        OldInitialButtonPositions.Add(Button, Button->GetActorLocation());
        Button->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnButtonOverlapBegin);
        
    }
    for (int32 i = 0; i < ButtonArray.Num(); ++i)
    {
        ButtonsReachedMax.Add(false);
        overlappedButtons.Add(false);
    }
}

// Called every frame
void APowerManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (Cast<USphereComponent>(VRPlayerController->rootComp)->IsGravityEnabled())
    {
        VRPawn->FloatingPawn->Velocity.Z = 0.f;
    }

    if (GoEnableGravity)
    {
        UE_LOG(LogTemp, Warning, TEXT("ENABLING GRAVITY NOW"));
        Cast<USphereComponent>(VRPlayerController->rootComp)->SetSimulatePhysics(true);
        Cast<USphereComponent>(VRPlayerController->rootComp)->SetEnableGravity(true);
        Flashlight->GetStaticMeshComponent()->SetEnableGravity(true);
        Cast<USphereComponent>(VRPlayerController->rootComp)->SetMassOverrideInKg(NAME_None, 0.1f);
        Cast<USphereComponent>(VRPlayerController->rootComp)->SetSphereRadius(17.5f);
        VRPlayerController->BoostingAllowed = false;
        VRPlayerController->ThrustingAllowed = false;
        VRPawn->FloatingPawn->Deceleration = 1000.f;
        GoEnableGravity = false;
        TArray<UAudioComponent*> AudioComponents;
        VRPawn->GetComponents<UAudioComponent>(AudioComponents);
        for(UAudioComponent* AudioComp : AudioComponents)
        {
            if(AudioComp->GetName().Equals("WindFall"))
            {
                WindFall = AudioComp;
                WindFall->Play();
            }
            else if (AudioComp->GetName().Equals("Splat"))
            {
                Splat = AudioComp;
            }
        }
    }

    if (GoDisableGravity)
    {
        UE_LOG(LogTemp, Warning, TEXT("DISABLING GRAVITY NOW"));
        Cast<USphereComponent>(VRPlayerController->rootComp)->SetSimulatePhysics(false);
        Cast<USphereComponent>(VRPlayerController->rootComp)->SetEnableGravity(false);
        Flashlight->GetStaticMeshComponent()->SetEnableGravity(false);
        Cast<USphereComponent>(VRPlayerController->rootComp)->SetMassOverrideInKg(NAME_None, 0.1f);
        Cast<USphereComponent>(VRPlayerController->rootComp)->SetSphereRadius(12.5f);
        VRPlayerController->BoostingAllowed = true;
        VRPlayerController->ThrustingAllowed = true;
        VRPawn->FloatingPawn->Deceleration = 0.f;
        GoDisableGravity = false;
        Moonlight->SetEnabled(false);
        BeginTransition = false;
        MusicPlayed = false;
        PlayMusic = false;
        ElevatorOpened = false;
        GravityEnabled = false;
        ArenaCloseWall->SetActorEnableCollision(false);
        LivingRoomSealWall->SetActorEnableCollision(false);
        SlowDownMusic = false;
        SlowDownMusic2 = false;
        SpeedMusicUp1 = false;
        Splatted = false;
        paintedDecal = true;
        SplatMoved = false;
        BloodSplatter->SetActorHiddenInGame(true);
        ScoreManager->PowerResetOnce = false;
        ScoreManager->turnPowerBackOn1 = false;
        ScoreManager->SecondPhaseBegun = false;
        ScoreManager->TimeSecondPhaseBegun = 0.f;
        ScoreManager->TurnOffPowerSection1 = false;
        ScoreManager->TurnOffPowerSection2 = false;
        ScoreManager->TurnOffPowerSection3 = false;
        ScoreManager->TurnOffPowerSection4 = false;
        ScoreManager->SecondPhaseComplete = false;
        ScoreManager->PowerOffOnce = false;
        ScoreManager->OScoringAllowed = true;
        ScoreManager->BScoringAllowed = true;
        ArenaResetDoor->SetActorHiddenInGame(false);
        ArenaResetDoor->SetActorEnableCollision(true);
        ScoreManager->Door1->SetActorLocation(ScoreManager->InitDoor1Pos);
        ScoreManager->Door2->SetActorHiddenInGame(false);
        ScoreManager->Door2->SetActorEnableCollision(true);
        VRPawn->CanPlayDragSound = false;
        VRPawn->FloatingPawn->Velocity = VRPawn->velocity;
    }

    if (Splatted)
    {
        ArenaResetDoor->SetActorHiddenInGame(true);
        ArenaResetDoor->SetActorEnableCollision(false);
        if (!SplatMoved)
        {
            FVector CurrentDifference = VRPawn->GetActorLocation() - BloodSplatter->GetActorLocation();
            TArray<AActor*> OverlappingActors;
            SplatTeleportTrigger->GetOverlappingActors(OverlappingActors);
            FVector SplatPosition = BloodSplatter->GetActorLocation();
    
            for (AActor* ActorToTeleport : OverlappingActors)
            {
                if (ActorToTeleport->ActorHasTag("ToTeleport"))
                {
                    ActorToTeleport->SetActorLocation(BloodSplatter2->GetActorLocation() + (ActorToTeleport->GetActorLocation() - SplatPosition), false, nullptr, ETeleportType::TeleportPhysics);
                }
            }
            FVector UpdatedSplatPosition = BloodSplatter2->GetActorLocation();
            // Then compute the new location for the VRPawn
            FVector NewLocation = UpdatedSplatPosition + CurrentDifference;
            VRPawn->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
            SplatMoved = true;
        }
        if (paintedDecal)
        {
            playerPosition = VRPlayerController->rootComp->GetComponentLocation();
            paintedDecal = false;
        }
        else if ((VRPlayerController->rootComp->GetComponentLocation() - playerPosition).Size() > 100.f)
        {
            FRotator rotator = (VRPlayerController->rootComp->GetComponentLocation() - playerPosition).GetSafeNormal().Rotation();
            // Add a decal at the specified location and rotation
            UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BloodDragDecal, FVector(90.0f), ((VRPlayerController->rootComp->GetComponentLocation() + playerPosition)/2) + FVector(-30.f, 0.f, 0.f), rotator);
            paintedDecal = true;
        }
    }

    if (PlayMusic)
    {
        TurnPowerBackOn2();
        PlayMusic = false;
        PowerIsOn = true;
    }

    if (SlowDownMusic2)
    {
        if (Music2->GetAudioComponent()->VolumeMultiplier > 0.f)
        {
            //Music2->GetAudioComponent()->SetPitchMultiplier(Music2->GetAudioComponent()->PitchMultiplier - 0.0025f);
            Music2->GetAudioComponent()->SetVolumeMultiplier(Music2->GetAudioComponent()->VolumeMultiplier - 0.0025f);
        }
        else
        {
            Music2->Stop();
            SlowDownMusic2 = false;
            Music2->GetAudioComponent()->SetPitchMultiplier(1.0f);
            Music2->GetAudioComponent()->SetVolumeMultiplier(1.0f);
        }
    }

    if (SlowDownMusic)
    {
        if (Music->GetAudioComponent()->VolumeMultiplier > 0.f)
        {
            Music->GetAudioComponent()->SetPitchMultiplier(Music->GetAudioComponent()->PitchMultiplier - 0.0025f);
            Music->GetAudioComponent()->SetVolumeMultiplier(Music->GetAudioComponent()->VolumeMultiplier - 0.0025f);

        }
        else
        {
            Music->Stop();
            SlowDownMusic = false;
            Music->GetAudioComponent()->SetPitchMultiplier(1.0f);
            Music->GetAudioComponent()->SetVolumeMultiplier(1.0f);
        }
    }

    if (SpeedMusicUp1)
    {
        Music->Play(30.f);
        if (Music->GetAudioComponent()->VolumeMultiplier < 1.f)
        {
            Music->GetAudioComponent()->SetPitchMultiplier(Music->GetAudioComponent()->PitchMultiplier + 0.0025f);
            Music->GetAudioComponent()->SetVolumeMultiplier(Music->GetAudioComponent()->VolumeMultiplier + 0.0025f);

        }
        else
        {
            SpeedMusicUp1 = false;
            Music->GetAudioComponent()->SetPitchMultiplier(1.0f);
            Music->GetAudioComponent()->SetVolumeMultiplier(1.0f);
        }
    }
    
    if (BeginTransition)
    {
        MilkDoor->SetActorHiddenInGame(true);
        MilkDoor->SetActorEnableCollision(false);
        Music2->SetActorTransform(Music->GetActorTransform());
        FVector CurrentDifference = VRPawn->GetActorLocation() - GeneratorActor->GetActorLocation();

        TArray<AActor*> OverlappingActors;
        WhatToTeleportTrigger->GetOverlappingActors(OverlappingActors);
        FVector GeneratorPosition = GeneratorActor->GetActorLocation();
    
        for (AActor* ActorToTeleport : OverlappingActors)
        {
            if (ActorToTeleport->ActorHasTag("ToTeleport"))
            {
                ActorToTeleport->SetActorLocation(InitGenPos.GetLocation() + (ActorToTeleport->GetActorLocation() - GeneratorPosition), false, nullptr, ETeleportType::TeleportPhysics);
            }
        }
        
        // Update the generator actor position first
        GeneratorActor->SetActorTransform(InitGenPos);
        FVector UpdatedGenPosition = GeneratorActor->GetActorLocation();
        // Then compute the new location for the VRPawn
        FVector NewLocation = UpdatedGenPosition + CurrentDifference;
        VRPawn->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
        //VRPlayerController->VRPawnMovement->Velocity = FVector(0.f);
        // Hide the second phase hallway addition only after moving the actors
        for (AStaticMeshActor* Actor : SecondPhaseHallwayAdditions)
        {
            Actor->SetActorHiddenInGame(true);
        }
        InitialButtonPositions.Empty();
        for (int32 i = 0; i < ButtonArray.Num(); ++i)
        {
            AStaticMeshActor* Button = ButtonArray[i];
            GenHumSoundArray[i]->Stop();
            GenLightArray[i]->SetLightColor(FColor::Red);

            // Disable physics simulation and detach constraints
            Button->GetStaticMeshComponent()->SetSimulatePhysics(false);

            // Teleport button and constraint
            Button->SetActorLocation(OldInitialButtonPositions[Button], false, nullptr, ETeleportType::TeleportPhysics);
    
            // Reset velocity
            Button->GetStaticMeshComponent()->SetPhysicsLinearVelocity(FVector::ZeroVector);
            Button->GetStaticMeshComponent()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
            Button->GetStaticMeshComponent()->SetSimulatePhysics(true);
    
            // Add button to initial positions list
            InitialButtonPositions.Add(Button, Button->GetActorLocation());
        }
        ScoreManager->OpenDoor3();
        BeginTransition = false;
    }
    
    if (!PowerIsOn)
    {
        for (int32 Index = 0; Index < ButtonArray.Num(); ++Index)
        {
            // Only process this button if it hasn't yet reached MaxX
            if (!ButtonsReachedMax[Index])
            {
                AStaticMeshActor* Button = ButtonArray[Index];

                if (overlappedButtons[Index])
                {
                    Button->GetStaticMeshComponent()->AddForce(FVector(500.0f, 0.f, 0.f));
                }
                // The maximum X position this object should be able to reach is its initial position plus the constraint limit.
                FVector InitialPosition = InitialButtonPositions[Button];
                float MaxX = InitialPosition.X + 15.f; // This should match your constraint limit

                if (Button->GetActorLocation().X >= MaxX)
                {
                    Button->GetStaticMeshComponent()->SetSimulatePhysics(false);
                    Button->GetStaticMeshComponent()->ComponentTags.Remove("Button");
                    // Pass the index to the new function
                    PowerGenerator(Index);
                }
            }
        }
        if (Algo::AllOf(ButtonsReachedMax, [](bool Value){ return Value; }))
        {
            if (!GoalsSwapped)
            {
                TurnPowerBackOn1();
            }
            else
            {
                //TurnPowerBackOn2();
            }
            ButtonsReachedMax.Empty();
            overlappedButtons.Empty();

            for (int32 i = 0; i < ButtonArray.Num(); ++i)
            {
                // AStaticMeshActor* Button = ButtonArray[i];
                // Button->GetStaticMeshComponent()->SetSimulatePhysics(true);
                // Button->GetStaticMeshComponent()->ComponentTags.Add("Button");
                ButtonsReachedMax.Add(false);
                overlappedButtons.Add(false);
            }
        }
    }

}

void APowerManager::PowerGenerator(int32 Index)
{
    ButtonClickSoundArray[Index]->Play();
    GenHumSoundArray[Index]->Play();
    GenLightArray[Index]->SetLightColor(FColor::Green);
    ButtonsReachedMax[Index] = true;
}

void APowerManager::TurnPowerOff1()
{
    LightCage->GetStaticMeshComponent()->SetVisibility(true);
    LightCageSphere->GetStaticMeshComponent()->SetVisibility(true);
    LightCageLight->SetEnabled(true);
    LightCageLightSound->Play();
    OrangeGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(true);
    OrangeGoalRim->GetStaticMeshComponent()->SetEnableGravity(true);
    BlueGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(true);
    BlueGoalRim->GetStaticMeshComponent()->SetEnableGravity(true);
    BlueGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BlueGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    OrangeGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    OrangeGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BlueBubble->SetActorHiddenInGame(true);
    BlueBubbleInterior->SetActorHiddenInGame(true);
    OrangeBubble->SetActorHiddenInGame(true);
    OrangeBubbleInterior->SetActorHiddenInGame(true);
    ScoreManager->OScoringAllowed = false;
    ScoreManager->BScoringAllowed = false;

    for (APointLight* Light : ArenaLightArray)
    {
        Light->SetEnabled(false);
    }

    ArenaEmissions->SetEnabled(false);
    ArenaEmissions2->SetEnabled(false);
    PowerDown->Play();
    SlowDownMusic = true;
    PowerIsOn = false;
}

void APowerManager::TurnPowerOff2()
{
    LightCage->GetStaticMeshComponent()->SetVisibility(true);
    LightCageSphere->GetStaticMeshComponent()->SetVisibility(true);
    LightCageLight->SetEnabled(true);
    LightCageLightSound->Play();
    
    ArenaEmissions->SetEnabled(false);
    ArenaEmissions2->SetEnabled(false);
    
    PowerDown->Play();
    
    TArray<AActor*> OverlappingActors;
    WhatToTeleportTrigger->GetOverlappingActors(OverlappingActors);
    FVector GeneratorPosition = GeneratorActor->GetActorLocation();
    FVector NewGenPos = NewGeneratorActor->GetActorLocation();
    FVector currentDifference;
    for (AActor* ActorToTeleport : OverlappingActors)
    {
        if (ActorToTeleport->ActorHasTag("ToTeleport"))
        {
            currentDifference = ActorToTeleport->GetActorLocation() - GeneratorPosition;
            ActorToTeleport->SetActorLocation(NewGenPos + currentDifference, false, nullptr, ETeleportType::TeleportPhysics);
        }
    }
    
    GeneratorActor->SetActorTransform(NewGeneratorActor->GetActorTransform(), false, nullptr, ETeleportType::TeleportPhysics);
    InitialButtonPositions.Empty();
    
    for (int32 i = 0; i < ButtonArray.Num(); ++i)
    {
        AStaticMeshActor* Button = ButtonArray[i];
        GenHumSoundArray[i]->Stop();
        GenLightArray[i]->SetLightColor(FColor::Red);

        // Disable physics simulation and detach constraints
        Button->GetStaticMeshComponent()->SetSimulatePhysics(false);

        // Teleport button and constraint
        Button->SetActorLocation(NewButtonPositions[i]->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
    
        // Reset velocity
        Button->GetStaticMeshComponent()->SetPhysicsLinearVelocity(FVector::ZeroVector);
        Button->GetStaticMeshComponent()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
        Button->GetStaticMeshComponent()->SetSimulatePhysics(true);
    
        // Add button to initial positions list
        InitialButtonPositions.Add(Button, Button->GetActorLocation());
    }

    for (AStaticMeshActor* Element : ElementsToMakeInvisible)
    {
        Element->SetActorHiddenInGame(true);
        Element->GetStaticMeshComponent()->SetSimulatePhysics(false);
        Element->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    HallwayShrinkTrigger->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnOverlapBeginHallShrink);
    MusicTrigger->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnOverlapBeginMusic);
    CloseArenaOpenElevatorTrigger->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnOverlapBeginCloseArenaOpenElevator);
    
    for (AStaticMeshActor* Actor : SecondPhaseHallwayAdditions)
    {
        Actor->SetActorHiddenInGame(false);
    }

    PowerIsOn = false;
    
}

void APowerManager::TurnOffPowerSection1()
{
    PowerSectionOff->Play();
    for (int32 i = 0; i < ArenaLightArray.Num(); ++i)
    {
        if (i <= (ArenaLightArray.Num()/4))
        {
            APointLight* Light = ArenaLightArray[i];
            Light->SetEnabled(false);
        }
    }
}

void APowerManager::TurnOffPowerSection2()
{
    PowerSectionOff->Play();
    OrangeGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(true);
    OrangeGoalRim->GetStaticMeshComponent()->SetEnableGravity(true);
    OrangeGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    OrangeGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BlueBubble->SetActorHiddenInGame(true);
    BlueBubbleInterior->SetActorHiddenInGame(true);
    ScoreManager->OScoringAllowed = false;
    for (int32 i = 0; i < ArenaLightArray.Num(); ++i)
    {
        if ((i > (ArenaLightArray.Num()/4)) &&  (i <= (ArenaLightArray.Num()/4)*2))
        {
            APointLight* Light = ArenaLightArray[i];
            Light->SetEnabled(false);
        }
    }
}

void APowerManager::TurnOffPowerSection3()
{
    PowerSectionOff->Play();
    BlueGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(true);
    BlueGoalRim->GetStaticMeshComponent()->SetEnableGravity(true);
    BlueGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BlueGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    OrangeBubble->SetActorHiddenInGame(true);
    OrangeBubbleInterior->SetActorHiddenInGame(true);
    ScoreManager->BScoringAllowed = false;
    SlowDownMusic = true;
    ScaryLaugh->Play();
    
    for (int32 i = 0; i < ArenaLightArray.Num(); ++i)
    {
        if ((i > (ArenaLightArray.Num()/4)*2) &&  (i <= (ArenaLightArray.Num()/4)*3))
        {
            APointLight* Light = ArenaLightArray[i];
            Light->SetEnabled(false);
        }
    }
}

void APowerManager::TurnOffPowerSection4()
{
    PowerSectionOff->Play();
    for (int32 i = 0; i < ArenaLightArray.Num(); ++i)
    {
        if ((i > (ArenaLightArray.Num()/4)*3) &&  (i <= (ArenaLightArray.Num()/4)*4))
        {
            APointLight* Light = ArenaLightArray[i];
            Light->SetEnabled(false);
        }
    }

    TurnPowerOff2();
    
}

void APowerManager::TurnPowerBackOn1()
{
    Solution->Play();
    LightCage->GetStaticMeshComponent()->SetVisibility(false);
    LightCageSphere->GetStaticMeshComponent()->SetVisibility(false);
    LightCageLight->SetEnabled(false);
    LightCageLightSound->Stop();
    OrangeGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(false);
    OrangeGoalRim->GetStaticMeshComponent()->SetEnableGravity(false);
    BlueGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(false);
    BlueGoalRim->GetStaticMeshComponent()->SetEnableGravity(false);
    BlueGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BlueGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    OrangeGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    OrangeGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BlueBubble->SetActorHiddenInGame(false);
    BlueBubbleInterior->SetActorHiddenInGame(false);
    OrangeBubble->SetActorHiddenInGame(false);
    OrangeBubbleInterior->SetActorHiddenInGame(false);
    ScoreManager->BScoringAllowed = true;
    ScoreManager->OScoringAllowed = true;

    for (APointLight* Light : ArenaLightArray)
    {
        Light->SetEnabled(true);
    }
    ArenaEmissions->SetEnabled(true);
    ArenaEmissions2->SetEnabled(true);

    SpeedMusicUp1 = true;

    BlueGoalRim->SetActorTransform(InitBGoalPos);
    BlueLight->SetActorTransform(InitBLightPos);
    OrangeGoalRim->SetActorTransform(InitOGoalPos);
    OrangeLight->SetActorTransform(InitOLightPos);
    
    GoalSwapTrigger->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnOverlapBegin);

    ScoreManager->ResetScore();
    
    PowerIsOn = true;
}

void APowerManager::TurnPowerBackOn2()
{
    LightCage->GetStaticMeshComponent()->SetVisibility(false);
    LightCageSphere->GetStaticMeshComponent()->SetVisibility(false);
    LightCageLight->SetEnabled(false);
    LightCageLightSound->Stop();
    OrangeGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(false);
    OrangeGoalRim->GetStaticMeshComponent()->SetEnableGravity(false);
    BlueGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(false);
    BlueGoalRim->GetStaticMeshComponent()->SetEnableGravity(false);
    BlueGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BlueGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    OrangeGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    OrangeGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BlueBubble->SetActorHiddenInGame(false);
    BlueBubbleInterior->SetActorHiddenInGame(false);
    OrangeBubble->SetActorHiddenInGame(false);
    OrangeBubbleInterior->SetActorHiddenInGame(false);
    ScoreManager->OScoringAllowed = true;
    ScoreManager->BScoringAllowed = true;

    for (APointLight* Light : ArenaLightArray)
    {
        Light->SetEnabled(true);
    }
    ArenaEmissions->SetEnabled(true);
    ArenaEmissions2->SetEnabled(true);
    Music2->Play(50.f);
    UE_LOG(LogTemp, Warning, TEXT("SHOULD START MUSIC HERE"));
    
    BlueGoalRim->SetActorTransform(InitOGoalPos);
    BlueLight->SetActorTransform(InitOLightPos);
    OrangeGoalRim->SetActorTransform(InitBGoalPos);
    OrangeLight->SetActorTransform(InitBLightPos);

    ScoreManager->ResetScore();
    
    PowerIsOn = true;
}

void APowerManager::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!GoalsSwapped)
    {
        BlueGoalRim->SetActorTransform(InitOGoalPos);
        BlueLight->SetActorTransform(InitOLightPos);
        OrangeGoalRim->SetActorTransform(InitBGoalPos);
        OrangeLight->SetActorTransform(InitBLightPos);
        Ball->GetStaticMeshComponent()->SetPhysicsLinearVelocity(FVector(0.f,0.f,0.f));
        Ball->SetActorTransform(InitBallPos);
        InitialButtonPositions.Empty();
        GoalsSwapped = true;
    }
}

void APowerManager::OnOverlapBeginHallShrink(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!HallShrunk && OtherActor->GetName().Contains("Pawn"))
    {
        BeginTransition = true;
        HallShrunk = true;
    }

}

void APowerManager::OnOverlapBeginMusic(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!MusicPlayed && OtherActor->GetName().Contains("Pawn"))
    {
        PlayMusic = true;
        MusicPlayed = true;
    }
}

void APowerManager::OnOverlapBeginCloseArenaOpenElevator(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!ElevatorOpened && OtherActor->GetName().Contains("Pawn"))
    {
        ElevatorOpenWall->SetActorHiddenInGame(true);
        ElevatorOpenWall->SetActorEnableCollision(false);
        ArenaCloseWall->SetActorHiddenInGame(false);
        ArenaCloseWall->SetActorEnableCollision(true);
        //LivingRoomSealWall->SetActorHiddenInGame(false);
        //LivingRoomSealWall->SetActorEnableCollision(true);
        SlowDownMusic2 = true;
        ElevatorOpened = true;
    }
}

void APowerManager::OnOverlapEnableGravity(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!GravityEnabled && OtherActor->GetName().Contains("Pawn"))
    {
        GoEnableGravity = true;
        GravityEnabled = true;
    }
}

void APowerManager::OnOverlapResetGame(AActor* OverlappedActor, AActor* OtherActor)
{
    if (VRPawn->InitiateFall && OtherActor->GetName().Contains("Pawn"))
    {
        GoEnableGravity = false;
        GravityEnabled = false;
        GoDisableGravity = true;
        GravityDisabled = true;
        VRPawn->InitiateFall = false;
        VRPawn->ResetGame = true;
    }
}

void APowerManager::OnOverlapBeginSplat(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!Splatted && OtherActor->GetName().Contains("Pawn"))
    {
        WindFall->Stop();
        Splat->Play();
        Splatted = true;
        VRPawn->CanPlayDragSound = true;
        BloodSplatter->SetActorHiddenInGame(false);
        Moonlight->SetEnabled(true);
    }
    if (!FlashBroke && OtherActor->ActorHasTag("Flashlight"))
    {
        FlashBreak->Play();
        FlashlightLight->SetEnabled(false);
        FlashlightLightMesh->SetActorHiddenInGame(true);
        Flashlight->Tags.Add("BrokenCollect");
    }
}

void APowerManager::OnButtonOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
    UE_LOG(LogTemp, Warning, TEXT("%s"), *OverlappedActor->GetName());
    UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherActor->GetName());
    if (!PowerIsOn)
    {
        if (OtherActor->GetName().Contains("VRPawn") && OverlappedActor->GetName() == "1M_Cube2_97")
        {
            overlappedButtons[0] = true;
        }
        else if (OtherActor->GetName().Contains("VRPawn") && OverlappedActor->GetName() == "1M_Cube2_105")
        {
            overlappedButtons[1] = true;
        }
        else if (OtherActor->GetName().Contains("VRPawn") && OverlappedActor->GetName() == "1M_Cube2_106")
        {
            overlappedButtons[2] = true;
        }
        else if (OtherActor->GetName().Contains("VRPawn") && OverlappedActor->GetName() == "1M_Cube2_107")
        {
            overlappedButtons[3] = true;
        }
    }
}

