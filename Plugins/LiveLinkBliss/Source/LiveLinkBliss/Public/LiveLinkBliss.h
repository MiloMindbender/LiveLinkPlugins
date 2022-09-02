// Copyright Epic Games, Inc. All Rights Reserved.

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
