// Portions Copyright Epic Games, Inc. All Rights Reserved.
// Portions Copyright Greg Corson, All Rights Reserved
// This LiveLink code is based on the original Epic FreeD plugin, this was necessary because Epic does not supply documentation on how to use LiveLink
// I have added comments to make the code easier to understand.

#include "LiveLinkBlissSourceFactory.h"
#include "LiveLinkBlissSource.h"
#include "SLiveLinkBlissSourceFactory.h"
#include "LiveLinkBlissSourceSettings.h"

#include "ILiveLinkClient.h"
#include "Features/IModularFeatures.h"

#define LOCTEXT_NAMESPACE "LiveLinkBlissSourceFactory"

// Get the source name
FText ULiveLinkBlissSourceFactory::GetSourceDisplayName() const
{
	return LOCTEXT("SourceDisplayName", "LiveLinkBliss Source");	
}

// Get the tooltip that appears next to the source
FText ULiveLinkBlissSourceFactory::GetSourceTooltip() const
{
	return LOCTEXT("SourceTooltip", "Allows creation of multiple LiveLink sources using the Bliss tracking system");
}

TSharedPtr<SWidget> ULiveLinkBlissSourceFactory::BuildCreationPanel(FOnLiveLinkSourceCreated InOnLiveLinkSourceCreated) const
{
	return SNew(SLiveLinkBlissSourceFactory)
		.OnConnectionSettingsAccepted(FOnLiveLinkBlissConnectionSettingsAccepted::CreateUObject(this, &ULiveLinkBlissSourceFactory::CreateSourceFromSettings, InOnLiveLinkSourceCreated));
}

TSharedPtr<ILiveLinkSource> ULiveLinkBlissSourceFactory::CreateSource(const FString& ConnectionString) const
{
	FLiveLinkBlissConnectionSettings ConnectionSettings;
	if (!ConnectionString.IsEmpty())
	{
		FLiveLinkBlissConnectionSettings::StaticStruct()->ImportText(*ConnectionString, &ConnectionSettings, nullptr, PPF_None, GLog, TEXT("ULiveLinkBlissSourceFactory"));
	}
	return MakeShared<FLiveLinkBlissSource>(ConnectionSettings);
}

void ULiveLinkBlissSourceFactory::CreateSourceFromSettings(FLiveLinkBlissConnectionSettings InConnectionSettings, FOnLiveLinkSourceCreated OnSourceCreated) const
{
	FString ConnectionString;
	FLiveLinkBlissConnectionSettings::StaticStruct()->ExportText(ConnectionString, &InConnectionSettings, nullptr, nullptr, PPF_None, nullptr);

	TSharedPtr<FLiveLinkBlissSource> SharedPtr = MakeShared<FLiveLinkBlissSource>(InConnectionSettings);
	OnSourceCreated.ExecuteIfBound(SharedPtr, MoveTemp(ConnectionString));
}

#undef LOCTEXT_NAMESPACE
