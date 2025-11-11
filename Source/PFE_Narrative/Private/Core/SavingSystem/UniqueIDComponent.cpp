// copyright2025 Allen Broquet Lamouche Tamen. All rights reserved.


#include "Core/SavingSystem/UniqueIDComponent.h"


UUniqueIDComponent::UUniqueIDComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

#if WITH_EDITOR
void UUniqueIDComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (ActorID.IsEmpty())
	{
		ActorID = FGuid::NewGuid().ToString();
		Modify();
	}
}

void UUniqueIDComponent::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);
	if (ActorID.IsEmpty())
	{
		ActorID = FGuid::NewGuid().ToString();
		Modify();
	}
}

void UUniqueIDComponent::PostEditImport()
{
	Super::PostEditImport();
	if (ActorID.IsEmpty())
	{
		ActorID = FGuid::NewGuid().ToString();
		Modify();
	}
}

void UUniqueIDComponent::RegenerateID()
{
	ActorID = FGuid::NewGuid().ToString();
	Modify();
}
#endif

