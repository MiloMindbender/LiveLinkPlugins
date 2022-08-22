// Copyright 2022 by Greg Corson and Kangaroo Koncepts
// Software offered as is, no warranties
// You may use and redistribute this software as long as my copyright and "free" notice is retained.
// All redistributions and derivative works based on this software must be offered free
//

#pragma once

#include "CoreMinimal.h"
#include "LiveLinkSourceFactory.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "BlissTrackerSourceFactory.generated.h"

/**
 * 
 */
UCLASS()
class UBlissTrackerSourceFactory : public ULiveLinkSourceFactory
{
public:
	GENERATED_BODY()
    virtual FText GetSourceDisplayName() const override;
    virtual FText GetSourceTooltip() const override;
    virtual EMenuType GetMenuType() const override { return EMenuType::SubPanel; }
    virtual TSharedPtr<SWidget> BuildCreationPanel(FOnLiveLinkSourceCreated OnLiveLinkSourceCreated) const override;
    TSharedPtr<ILiveLinkSource> CreateSource(const FString& ConnectionString) const override;

private:
    void OnOkClicked(FIPv4Endpoint Endpoint, FOnLiveLinkSourceCreated OnLiveLinkSourceCreated) const;

};
