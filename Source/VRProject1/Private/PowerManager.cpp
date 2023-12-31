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

    HallwayShrunkAdded = false;
    MusicTriggerAdded = false;
    CloseArenaOpenElevatorAdded = false;
    Moonlight->SetEnabled(false);
    CanResetGame = false;
    FlashlightInitPos = Flashlight->GetActorLocation();
    CanSwapGoals = false;
    GoalSwapTrigger->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnOverlapBegin);
    
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
    SplatTrigger2->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnOverlapBeginSplat);
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
        if (VRPawn)
        {
            VRPawn->FloatingPawn->Velocity.Z = 0.f;
        }
    }

    if (GoEnableGravity)
    {
        UE_LOG(LogTemp, Warning, TEXT("ENABLING GRAVITY NOW"));
        if (VRPlayerController)
        {
            Cast<USphereComponent>(VRPlayerController->rootComp)->SetSimulatePhysics(true);
            Cast<USphereComponent>(VRPlayerController->rootComp)->SetEnableGravity(true);
            if (Flashlight)
            {
                Flashlight->GetStaticMeshComponent()->SetEnableGravity(true);
            }
            Cast<USphereComponent>(VRPlayerController->rootComp)->SetMassOverrideInKg(NAME_None, 0.1f);
            Cast<USphereComponent>(VRPlayerController->rootComp)->SetSphereRadius(17.5f);
            VRPlayerController->BoostingAllowed = false;
            VRPlayerController->ThrustingAllowed = false;
            VRPlayerController->BoostingAllowed = false;
            VRPlayerController->ThrustingAllowed = false;
        }
        if (VRPawn)
        {
            VRPawn->FloatingPawn->Deceleration = 1000.f;
        }
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
            else if (AudioComp->GetName().Contains("Splat"))
            {
                Splat = AudioComp;
            }
        }
    }

    if (GoDisableGravity)
    {
        UE_LOG(LogTemp, Warning, TEXT("DISABLING GRAVITY NOW"));
        if (VRPlayerController)
        {
            Cast<USphereComponent>(VRPlayerController->rootComp)->SetSimulatePhysics(false);
            Cast<USphereComponent>(VRPlayerController->rootComp)->SetEnableGravity(false);
            //Flashlight->GetStaticMeshComponent()->SetEnableGravity(false);
            //Flashlight->SetActorEnableCollision(false);
            Cast<USphereComponent>(VRPlayerController->rootComp)->SetMassOverrideInKg(NAME_None, 0.1f);
            Cast<USphereComponent>(VRPlayerController->rootComp)->SetSphereRadius(12.5f);
            if (BlueGoalRim && BlueLight && OrangeGoalRim && OrangeLight)
            {
                BlueGoalRim->SetActorTransform(InitBGoalPos);
                BlueLight->SetActorTransform(InitBLightPos);
                OrangeGoalRim->SetActorTransform(InitOGoalPos);
                OrangeLight->SetActorTransform(InitOLightPos);
            }
            VRPlayerController->BoostingAllowed = true;
            VRPlayerController->ThrustingAllowed = true;
            if (VRPawn)
            {
                VRPawn->FloatingPawn->Deceleration = 0.f;

            }
            GoDisableGravity = false;
            if (Moonlight)
            {
                Moonlight->SetEnabled(false);
            }
        }
        BeginTransition = false;
        MusicPlayed = false;
        PlayMusic = false;
        HallShrunk = false;
        ElevatorOpened = false;
        GravityEnabled = false;
        if (ArenaCloseWall && LivingRoomSealWall)
        {
            ArenaCloseWall->SetActorEnableCollision(false);
            LivingRoomSealWall->SetActorEnableCollision(false);
        }
        SlowDownMusic = false;
        SlowDownMusic2 = false;
        SpeedMusicUp1 = false;
        Splatted = false;
        paintedDecal = true;
        SplatMoved = false;
        if (BloodSplatter)
        {
            BloodSplatter->SetActorHiddenInGame(true);
        }
        if (ScoreManager)
        {
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
            ScoreManager->Door1->SetActorLocation(ScoreManager->InitDoor1Pos);
            ScoreManager->Door2->SetActorHiddenInGame(false);
            ScoreManager->Door2->SetActorEnableCollision(true);
            ScoreManager->OpeningDoor1 = false;
            ScoreManager->Door2->SetActorLocation(ScoreManager->Door2Closed->GetActorLocation());
            ScoreManager->blueScore = 0;
            ScoreManager->orangeScore = 0;
        }
        if (ArenaResetDoor)
        {
            ArenaResetDoor->SetActorHiddenInGame(false);
            ArenaResetDoor->SetActorEnableCollision(true); 
        }
        if (VRPawn)
        {
            VRPawn->CanPlayDragSound = false;
            VRPawn->FloatingPawn->Velocity = VRPawn->velocity; 
        }

        if (Music && Music2)
        {
            Music->Play();
            Music2->Reset();
        }
        CanSwapGoals = false;
        GoalsSwapped = false;
        PowerIsOn = true;
        for (AStaticMeshActor* Element : ElementsToMakeInvisible)
        {
            Element->SetActorHiddenInGame(false);
            Element->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        }
        for (AStaticMeshActor* Actor : SecondPhaseHallwayAdditions)
        {
            Actor->SetActorHiddenInGame(true);
        }
        InitialButtonPositions.Empty();
        OldInitialButtonPositions.Empty();
        ButtonsReachedMax.Empty();
        overlappedButtons.Empty();
        for (AStaticMeshActor* Button : ButtonArray)
        {
            InitialButtonPositions.Add(Button, Button->GetActorLocation());
            OldInitialButtonPositions.Add(Button, Button->GetActorLocation());
            Button->GetStaticMeshComponent()->SetSimulatePhysics(true);
        }
        for (int32 i = 0; i < ButtonArray.Num(); ++i)
        {
            ButtonsReachedMax.Add(false);
            overlappedButtons.Add(false);
        }
        CanResetGame = false;
        if (VRPawn)
        {
            VRPawn->GravityReleased = false;
        }
    }

    if (Splatted)
    {
        if (ArenaResetDoor)
        {
            ArenaResetDoor->SetActorHiddenInGame(true);
            ArenaResetDoor->SetActorEnableCollision(false);
        }
        CanResetGame = true;
        if (!SplatMoved)
        {
            FVector CurrentDifference = VRPawn->GetActorLocation() - BloodSplatter->GetActorLocation();
            TArray<AActor*> OverlappingActors;
            if (SplatTeleportTrigger)
            {
                SplatTeleportTrigger->GetOverlappingActors(OverlappingActors);
            }
            
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
            if (VRPawn)
            {
                VRPawn->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
            }
            
            SplatMoved = true;
        }
        if (paintedDecal)
        {
            if (VRPlayerController)
            {
                playerPosition = VRPlayerController->rootComp->GetComponentLocation();
            }
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
        if (Music2)
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
        
    }

    if (SlowDownMusic)
    {
        if (Music)
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
        
    }

    if (SpeedMusicUp1)
    {
        if (Music)
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
        
    }
    
    if (BeginTransition)
    {
        if (MilkDoor)
        {
            MilkDoor->SetActorHiddenInGame(true);
                    MilkDoor->SetActorEnableCollision(false);
        }
        if (Music2)
        {
            Music2->SetActorTransform(Music->GetActorTransform());
        }
        FVector CurrentDifference;
        if (VRPawn && GeneratorActor)
        {
                    CurrentDifference = VRPawn->GetActorLocation() - GeneratorActor->GetActorLocation();

        }

        TArray<AActor*> OverlappingActors;
        if (WhatToTeleportTrigger)
        {
                    WhatToTeleportTrigger->GetOverlappingActors(OverlappingActors);

        }
        FVector GeneratorPosition;
        if (GeneratorActor)
        {
            GeneratorPosition = GeneratorActor->GetActorLocation();
        }
        
    
        for (AActor* ActorToTeleport : OverlappingActors)
        {
            if (ActorToTeleport->ActorHasTag("ToTeleport"))
            {
                ActorToTeleport->SetActorLocation(InitGenPos.GetLocation() + (ActorToTeleport->GetActorLocation() - GeneratorPosition), false, nullptr, ETeleportType::TeleportPhysics);
            }
        }
        FVector UpdatedGenPosition;
        if (GeneratorActor)
        {
            // Update the generator actor position first
            GeneratorActor->SetActorTransform(InitGenPos);
            UpdatedGenPosition = GeneratorActor->GetActorLocation();
        }
        // Then compute the new location for the VRPawn
        FVector NewLocation = UpdatedGenPosition + CurrentDifference;
        if (VRPawn)
        {
            VRPawn->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
        }
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
        if (ScoreManager)
        {
            ScoreManager->OpenDoor3();
        }
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
                    //Button->GetStaticMeshComponent()->ComponentTags.Remove("Button");
                    // Pass the index to the new function
                    PowerGenerator(Index);
                }
            }
        }
        if (Algo::AllOf(ButtonsReachedMax, [](bool Value){ return Value; }))
        {
            if (!GoalsSwapped)
            {
                CanSwapGoals = true;
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
    if (LightCage && LightCageSphere && LightCageLight && LightCageLightSound)
    {
        LightCage->GetStaticMeshComponent()->SetVisibility(true);
            LightCageSphere->GetStaticMeshComponent()->SetVisibility(true);
            LightCageLight->SetEnabled(true);
            LightCageLightSound->Play();
    }

    if (OrangeGoal && OrangeGoalRim && BlueGoal && BlueGoalRim && BlueBubble && OrangeBubble && BlueBubbleInterior && OrangeBubbleInterior)
    {
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
    }

    if (ScoreManager)
    {
       ScoreManager->OScoringAllowed = false;
           ScoreManager->BScoringAllowed = false; 
    }
    

    for (APointLight* Light : ArenaLightArray)
    {
        Light->SetEnabled(false);
    }

    if (ArenaEmissions && ArenaEmissions2 && PowerDown)
    {
            ArenaEmissions->SetEnabled(false);
            ArenaEmissions2->SetEnabled(false);
            PowerDown->Play();
    }

    SlowDownMusic = true;
    PowerIsOn = false;
}

void APowerManager::TurnPowerOff2()
{
    if (LightCage && LightCageSphere && LightCageLight && LightCageLightSound)
    {
        LightCage->GetStaticMeshComponent()->SetVisibility(true);
        LightCageSphere->GetStaticMeshComponent()->SetVisibility(true);
        LightCageLight->SetEnabled(true);
        LightCageLightSound->Play();
    }

    if (ArenaEmissions && ArenaEmissions2 && PowerDown)
    {
        ArenaEmissions->SetEnabled(false);
        ArenaEmissions2->SetEnabled(false);
        PowerDown->Play();
    }
    TArray<AActor*> OverlappingActors;
    if (WhatToTeleportTrigger)
    {
            WhatToTeleportTrigger->GetOverlappingActors(OverlappingActors);
    }
    FVector GeneratorPosition;
    FVector NewGenPos;
    if (GeneratorActor && NewGeneratorActor)
    {
        GeneratorPosition = GeneratorActor->GetActorLocation();
        NewGenPos = NewGeneratorActor->GetActorLocation();
    }
    FVector currentDifference;
    for (AActor* ActorToTeleport : OverlappingActors)
    {
        if (ActorToTeleport->ActorHasTag("ToTeleport"))
        {
            currentDifference = ActorToTeleport->GetActorLocation() - GeneratorPosition;
            ActorToTeleport->SetActorLocation(NewGenPos + currentDifference, false, nullptr, ETeleportType::TeleportPhysics);
        }
    }
    if (GeneratorActor)
    {
            GeneratorActor->SetActorTransform(NewGeneratorActor->GetActorTransform(), false, nullptr, ETeleportType::TeleportPhysics);
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

    if (HallwayShrinkTrigger)
    {
        if (!HallwayShrunkAdded)
        {
            HallwayShrinkTrigger->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnOverlapBeginHallShrink);
            HallwayShrunkAdded = true;
        }

    }
    if (MusicTrigger)
    {
        if (!MusicTriggerAdded)
        {
            MusicTrigger->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnOverlapBeginMusic);
            MusicTriggerAdded = true;
        }
    }
    if (CloseArenaOpenElevatorTrigger)
    {
        if (!CloseArenaOpenElevatorAdded)
        {
            CloseArenaOpenElevatorTrigger->OnActorBeginOverlap.AddDynamic(this, &APowerManager::OnOverlapBeginCloseArenaOpenElevator);
            CloseArenaOpenElevatorAdded = true;
        }
    }
    
    for (AStaticMeshActor* Actor : SecondPhaseHallwayAdditions)
    {
        Actor->SetActorHiddenInGame(false);
    }

    PowerIsOn = false;
    
}

void APowerManager::TurnOffPowerSection1()
{
    if (PowerSectionOff)
    {
        PowerSectionOff->Play();
    }
    
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
    if (PowerSectionOff)
    {
        PowerSectionOff->Play();
    }
    if (OrangeGoalRim && OrangeGoal && BlueBubble && BlueBubbleInterior)
    {
        OrangeGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(true);
        OrangeGoalRim->GetStaticMeshComponent()->SetEnableGravity(true);
        OrangeGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        OrangeGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        BlueBubble->SetActorHiddenInGame(true);
        BlueBubbleInterior->SetActorHiddenInGame(true); 
    }
    if (ScoreManager)
    {
        ScoreManager->OScoringAllowed = false;
    }
    
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
    if (PowerSectionOff)
    {
        PowerSectionOff->Play();
    }
    if (BlueGoalRim && BlueGoal && OrangeBubble && OrangeBubbleInterior)
    {
           BlueGoalRim->GetStaticMeshComponent()->SetSimulatePhysics(true);
           BlueGoalRim->GetStaticMeshComponent()->SetEnableGravity(true);
           BlueGoalRim->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
           BlueGoal->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
           OrangeBubble->SetActorHiddenInGame(true);
           OrangeBubbleInterior->SetActorHiddenInGame(true); 
    }
    if (ScoreManager)
    {
       ScoreManager->BScoringAllowed = false; 
    }
    SlowDownMusic = true;
    if (ScaryLaugh)
    {
       ScaryLaugh->Play(); 
    }
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
    if (PowerSectionOff)
    {
       PowerSectionOff->Play(); 
    }
    
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
    if (Solution)
    {
       Solution->Play(); 
    }
    if (LightCage && LightCageSphere && LightCageLight && LightCageLightSound)
    {
        LightCage->GetStaticMeshComponent()->SetVisibility(false);
        LightCageSphere->GetStaticMeshComponent()->SetVisibility(false);
        LightCageLight->SetEnabled(false);
        LightCageLightSound->Stop();
    }

    if (OrangeGoal && OrangeGoalRim && BlueGoal && BlueGoalRim && BlueBubble && OrangeBubble && BlueBubbleInterior && OrangeBubbleInterior)
    {
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
    }

    if (ScoreManager)
    {
        ScoreManager->BScoringAllowed = true;
            ScoreManager->OScoringAllowed = true;
    }

    

    for (APointLight* Light : ArenaLightArray)
    {
        Light->SetEnabled(true);
    }

    if (ArenaEmissions && ArenaEmissions2)
    {
            ArenaEmissions->SetEnabled(true);
            ArenaEmissions2->SetEnabled(true);
    }


    SpeedMusicUp1 = true;

    if (BlueGoalRim && BlueLight && OrangeGoalRim && OrangeLight)
    {
          BlueGoalRim->SetActorTransform(InitBGoalPos);
          BlueLight->SetActorTransform(InitBLightPos);
          OrangeGoalRim->SetActorTransform(InitOGoalPos);
          OrangeLight->SetActorTransform(InitOLightPos);  
    }

    if (ScoreManager)
    {
           ScoreManager->ResetScore(); 
    }

    PowerIsOn = true;
}

void APowerManager::TurnPowerBackOn2()
{
    if (LightCage && LightCageSphere && LightCageLight && LightCageLightSound)
    {
            LightCage->GetStaticMeshComponent()->SetVisibility(false);
            LightCageSphere->GetStaticMeshComponent()->SetVisibility(false);
            LightCageLight->SetEnabled(false);
            LightCageLightSound->Stop();
    }
    if (OrangeGoal && OrangeGoalRim && BlueGoal && BlueGoalRim && BlueBubble && OrangeBubble && BlueBubbleInterior && OrangeBubbleInterior)
    {
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
    }
    if (ScoreManager)
    {
           ScoreManager->OScoringAllowed = true;
           ScoreManager->BScoringAllowed = true; 
    }


    for (APointLight* Light : ArenaLightArray)
    {
        Light->SetEnabled(true);
    }
    if (ArenaEmissions && ArenaEmissions2)
    {
            ArenaEmissions->SetEnabled(true);
            ArenaEmissions2->SetEnabled(true);
    }
    if (Music2)
    {
        Music2->Play(50.f);
    }
    UE_LOG(LogTemp, Warning, TEXT("SHOULD START MUSIC HERE"));

    if (BlueGoalRim && BlueLight && OrangeGoalRim && OrangeLight)
    {
            BlueGoalRim->SetActorTransform(InitOGoalPos);
            BlueLight->SetActorTransform(InitOLightPos);
            OrangeGoalRim->SetActorTransform(InitBGoalPos);
            OrangeLight->SetActorTransform(InitBLightPos);
    }

if (ScoreManager)
{
        ScoreManager->ResetScore();

}
    
    PowerIsOn = true;
}

void APowerManager::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!GoalsSwapped && CanSwapGoals)
    {
        if (BlueGoalRim && BlueLight && OrangeGoalRim && OrangeLight)
        {
                    BlueGoalRim->SetActorTransform(InitOGoalPos);
                    BlueLight->SetActorTransform(InitOLightPos);
                    OrangeGoalRim->SetActorTransform(InitBGoalPos);
                    OrangeLight->SetActorTransform(InitBLightPos);
        }
        if (Ball)
        {
                    Ball->GetStaticMeshComponent()->SetPhysicsLinearVelocity(FVector(0.f,0.f,0.f));
                    Ball->SetActorTransform(InitBallPos);
        }
        InitialButtonPositions.Empty();
        GoalsSwapped = true;
        CanSwapGoals = false;
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
        if (ElevatorOpenWall)
        {
                    ElevatorOpenWall->SetActorHiddenInGame(true);
                    ElevatorOpenWall->SetActorEnableCollision(false);
        }
        if (ArenaCloseWall)
        {
                    ArenaCloseWall->SetActorHiddenInGame(false);
                    ArenaCloseWall->SetActorEnableCollision(true);
        }


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
    if (OtherActor->GetName().Contains("Pawn") && CanResetGame)
    {
        GoEnableGravity = false;
        GravityEnabled = false;
        GoDisableGravity = true;
        GravityDisabled = true;
        if (VRPawn)
        {
                    VRPawn->InitiateFall = false;
                    VRPawn->ResetGame = true;
        }

    }
}

void APowerManager::OnOverlapBeginSplat(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!Splatted && OtherActor->GetName().Contains("Pawn"))
    {
        if (WindFall)
        {
                   WindFall->Stop(); 
        }

        if (Splat)
        {
            Splat->Play();
        }
        Splatted = true;
        if (VRPawn)
        {
                   VRPawn->CanPlayDragSound = true; 
        }
        if (BloodSplatter)
        {
        BloodSplatter->SetActorHiddenInGame(false);
        }
        if (Moonlight)
        {
            Moonlight->SetEnabled(true);
        }
    }
    if (!FlashBroke && OtherActor->ActorHasTag("Flashlight"))
    {
        if (FlashBreak)
        {
           FlashBreak->Play(); 
        }
        if (FlashlightLight && FlashlightLightMesh && Flashlight)
        {
            FlashlightLight->SetEnabled(false);
            FlashlightLightMesh->SetActorHiddenInGame(true);
            Flashlight->Tags.Add("BrokenCollect");
        }
        
        FlashBroke = true;
    }
}

void APowerManager::OnButtonOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
    UE_LOG(LogTemp, Warning, TEXT("%s"), *OverlappedActor->GetName());
    UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherActor->GetName());
    if (!PowerIsOn)
    {
        if (!overlappedButtons.IsEmpty())
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
}

