// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "LiveLinkSourceFactory.h"
#include "LiveLinkBlissSource.h"
#include "LiveLinkBlissSourceFactory.generated.h"

UCLASS()
class ULiveLinkBlissSourceFactory : public ULiveLinkSourceFactory
{
public:
	GENERATED_BODY()

	virtual FText GetSourceDisplayName() const override;
	virtual FText GetSourceTooltip() const override;

	virtual EMenuType GetMenuType() const override { return EMenuType::SubPanel; }
	virtual TSharedPtr<SWidget> BuildCreationPanel(FOnLiveLinkSourceCreated OnLiveLinkSourceCreated) const override;
	virtual TSharedPtr<ILiveLinkSource> CreateSource(const FString& ConnectionString) const override;

private:
	void CreateSourceFromSettings(FLiveLinkBlissConnectionSettings ConnectionSettings, FOnLiveLinkSourceCreated OnSourceCreated) const;
};