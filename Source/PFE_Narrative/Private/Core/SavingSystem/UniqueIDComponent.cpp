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
#endif

