// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "LiveLinkBlissConnectionSettings.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

#if WITH_EDITOR
#include "IStructureDetailsView.h"
#endif //WITH_EDITOR

#include "Input/Reply.h"

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