
#include "LimitAreaCubeHelper.h"

#include <Components/SplineComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/SplineMeshComponent.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <Kismet/GameplayStatics.h>
#include <Components/BoxComponent.h>

ALimitAreaCubeHelper::ALimitAreaCubeHelper(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SceneComponentPtr = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	RootComponent = SceneComponentPtr;

	TopBoxComponentPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent1"));
	TopBoxComponentPtr->SetupAttachment(SceneComponentPtr);
	BottomBoxComponentPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent2"));
	BottomBoxComponentPtr->SetupAttachment(SceneComponentPtr);
	LeftBoxComponentPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent3"));
	LeftBoxComponentPtr->SetupAttachment(SceneComponentPtr);
	RightBoxComponentPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent4"));
	RightBoxComponentPtr->SetupAttachment(SceneComponentPtr);
	ForwardBoxComponentPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent5"));
	ForwardBoxComponentPtr->SetupAttachment(SceneComponentPtr);
	BackwardBoxComponentPtr = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent6"));
	BackwardBoxComponentPtr->SetupAttachment(SceneComponentPtr);

	PrimaryActorTick.bCanEverTick = true;
}

void ALimitAreaCubeHelper::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	TopBoxComponentPtr->SetBoxExtent(FVector(Size.X, Size.Y, 1));
	TopBoxComponentPtr->SetRelativeLocation(FVector(0, 0, Size.Z));
	TopBoxComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TopBoxComponentPtr->SetCollisionResponseToAllChannels(ECR_Ignore);
	TopBoxComponentPtr->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Block);

	BottomBoxComponentPtr->SetBoxExtent(FVector(Size.X, Size.Y, 1));
	BottomBoxComponentPtr->SetRelativeLocation(FVector(0, 0, -Size.Z));
	BottomBoxComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BottomBoxComponentPtr->SetCollisionResponseToAllChannels(ECR_Ignore);
	BottomBoxComponentPtr->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Block);

	LeftBoxComponentPtr->SetBoxExtent(FVector(Size.X, 1, Size.Z));
	LeftBoxComponentPtr->SetRelativeLocation(FVector(0, -Size.Y, 0));
	LeftBoxComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	LeftBoxComponentPtr->SetCollisionResponseToAllChannels(ECR_Ignore);
	LeftBoxComponentPtr->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Block);

	RightBoxComponentPtr->SetBoxExtent(FVector(Size.X, 1, Size.Z));
	RightBoxComponentPtr->SetRelativeLocation(FVector(0, Size.Y, 0));
	RightBoxComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RightBoxComponentPtr->SetCollisionResponseToAllChannels(ECR_Ignore);
	RightBoxComponentPtr->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Block);

	ForwardBoxComponentPtr->SetBoxExtent(FVector(1, Size.Y, Size.Z));
	ForwardBoxComponentPtr->SetRelativeLocation(FVector(Size.X, 0, 0));
	ForwardBoxComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ForwardBoxComponentPtr->SetCollisionResponseToAllChannels(ECR_Ignore);
	ForwardBoxComponentPtr->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Block);

	BackwardBoxComponentPtr->SetBoxExtent(FVector(1, Size.Y, Size.Z));
	BackwardBoxComponentPtr->SetRelativeLocation(FVector(-Size.X, 0, 0));
	BackwardBoxComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BackwardBoxComponentPtr->SetCollisionResponseToAllChannels(ECR_Ignore);
	BackwardBoxComponentPtr->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Block);
}

void ALimitAreaCubeHelper::BeginPlay()
{
	Super::BeginPlay();
}

void ALimitAreaCubeHelper::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
