// Copyright 2022 by Greg Corson and Kangaroo Koncepts
// Software offered as is, no warranties
// You may use and redistribute this software as long as my copyright and "free" notice is retained.
// All redistributions and derivative works based on this software must be offered free
//

#pragma once

#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Widgets/SCompoundWidget.h"
#include "Input/Reply.h"
#include "Types/SlateEnums.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SEditableTextBox;

class SBlissTrackerSourceFactory : public SCompoundWidget
{
public:
    DECLARE_DELEGATE_OneParam(FOnOkClicked, FIPv4Endpoint);

    SLATE_BEGIN_ARGS(SBlissTrackerSourceFactory) {}
    SLATE_EVENT(FOnOkClicked, OnOkClicked)
        SLATE_END_ARGS()

        void Construct(const FArguments& Args);

private:

    void OnEndpointChanged(const FText& NewValue, ETextCommit::Type);

    FReply OnOkClicked();

    TWeakPtr<SEditableTextBox> EditabledText;
    FOnOkClicked OkClicked;
};
