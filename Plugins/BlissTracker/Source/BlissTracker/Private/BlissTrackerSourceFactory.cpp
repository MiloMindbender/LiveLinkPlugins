// Copyright 2022 by Greg Corson and Kangaroo Koncepts
// Software offered as is, no warranties
// You may use and redistribute this software as long as my copyright and "free" notice is retained.
// All redistributions and derivative works based on this software must be offered free
//

#pragma once

#include "BlissTrackerSourceFactory.h"
#include "BlissTrackerSource.h"
#include "SBlissTrackerSourceFactory.h"

#define LOCTEXT_NAMESPACE "BlissTrackerSourceFactory"

FText UBlissTrackerSourceFactory::GetSourceDisplayName() const
{
    return LOCTEXT("SourceDisplayName", "Bliss Tracker");
}


FText UBlissTrackerSourceFactory::GetSourceTooltip() const
{
    return LOCTEXT("SourceTooltip", "Creates a connection to a Bliss UDP Stream");
}

TSharedPtr<SWidget> UBlissTrackerSourceFactory::BuildCreationPanel(FOnLiveLinkSourceCreated InOnLiveLinkSourceCreated) const
{
    return SNew(SBlissTrackerSourceFactory)
        .OnOkClicked(SBlissTrackerSourceFactory::FOnOkClicked::CreateUObject(this, &UBlissTrackerSourceFactory::OnOkClicked, InOnLiveLinkSourceCreated));
}

TSharedPtr<ILiveLinkSource> UBlissTrackerSourceFactory::CreateSource(const FString& InConnectionString) const
{
    FIPv4Endpoint DeviceEndPoint;
    if (!FIPv4Endpoint::Parse(InConnectionString, DeviceEndPoint))
    {
        return TSharedPtr<ILiveLinkSource>();
    }

    return MakeShared<FBlissTrackerSource>(DeviceEndPoint);
}

void UBlissTrackerSourceFactory::OnOkClicked(FIPv4Endpoint InEndpoint, FOnLiveLinkSourceCreated InOnLiveLinkSourceCreated) const
{
    InOnLiveLinkSourceCreated.ExecuteIfBound(MakeShared<FBlissTrackerSource>(InEndpoint), InEndpoint.ToString());
}

#undef LOCTEXT_NAMESPACE