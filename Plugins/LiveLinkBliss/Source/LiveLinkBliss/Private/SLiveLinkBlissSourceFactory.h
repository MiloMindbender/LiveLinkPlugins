// Portions Copyright Epic Games, Inc. All Rights Reserved.
// Portions Copyright Greg Corson, All Rights Reserved
// This LiveLink code is based on the original Epic FreeD plugin, this was necessary because Epic does not supply documentation on how to use LiveLink
// I have added comments to make the code easier to understand.
#pragma once

#include "LiveLinkBlissConnectionSettings.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

#if WITH_EDITOR
#include "IStructureDetailsView.h"
#endif //WITH_EDITOR

#include "Input/Reply.h"

// This is boiler plate code that all LiveLink plugins need to have.

struct FLiveLinkBlissConnectionSettings;

DECLARE_DELEGATE_OneParam(FOnLiveLinkBlissConnectionSettingsAccepted, FLiveLinkBlissConnectionSettings);

class SLiveLinkBlissSourceFactory : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SLiveLinkBlissSourceFactory)
	{}
		SLATE_EVENT(FOnLiveLinkBlissConnectionSettingsAccepted, OnConnectionSettingsAccepted)
	SLATE_END_ARGS()

	void Construct(const FArguments& Args);


private:
	FLiveLinkBlissConnectionSettings ConnectionSettings;

#if WITH_EDITOR
	TSharedPtr<FStructOnScope> StructOnScope;
	TSharedPtr<IStructureDetailsView> StructureDetailsView;
#endif //WITH_EDITOR

	FReply OnSettingsAccepted();
	FOnLiveLinkBlissConnectionSettingsAccepted OnConnectionSettingsAccepted;
};