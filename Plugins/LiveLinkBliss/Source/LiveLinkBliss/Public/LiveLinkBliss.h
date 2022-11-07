// Portions Copyright Epic Games, Inc. All Rights Reserved.
// Portions Copyright Greg Corson, All Rights Reserved
// This LiveLink code is based on the original Epic FreeD plugin, this was necessary because Epic does not supply documentation on how to use LiveLink
// I have added comments to make the code easier to understand.
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLiveLinkBliss, Log, All);

class FLiveLinkBlissModule : public IModuleInterface
{
public:

	static FLiveLinkBlissModule& Get()
	{
		return FModuleManager::Get().LoadModuleChecked<FLiveLinkBlissModule>(TEXT("LiveLinkBliss"));
	}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
