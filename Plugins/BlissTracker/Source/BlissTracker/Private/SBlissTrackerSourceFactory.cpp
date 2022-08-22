// Copyright 2022 by Greg Corson and Kangaroo Koncepts
// Software offered as is, no warranties
// You may use and redistribute this software as long as my copyright and "free" notice is retained.
// All redistributions and derivative works based on this software must be offered free
//

// Standard includes and namespace declaration

#include "SBlissTrackerSourceFactory.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "BlissTrackerSourceEditor"



void SBlissTrackerSourceFactory::Construct(const FArguments& Args)
{
    OkClicked = Args._OnOkClicked;

    FIPv4Endpoint Endpoint;
    Endpoint.Address = FIPv4Address::Any;
    Endpoint.Port = 54321;

    ChildSlot
        [
            SNew(SBox)
            .WidthOverride(250)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
        .HAlign(HAlign_Left)
        .FillWidth(0.5f)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("JSONPortNumber", "Port Number"))
        ]
    + SHorizontalBox::Slot()
        .HAlign(HAlign_Fill)
        .FillWidth(0.5f)
        [
            SAssignNew(EditabledText, SEditableTextBox)
            .Text(FText::FromString(Endpoint.ToString()))
        .OnTextCommitted(this, &SBlissTrackerSourceFactory::OnEndpointChanged)
        ]
        ]
    + SVerticalBox::Slot()
        .HAlign(HAlign_Right)
        .AutoHeight()
        [
            SNew(SButton)
            .OnClicked(this, &SBlissTrackerSourceFactory::OnOkClicked)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("Ok", "Ok"))
        ]
        ]
        ]
        ];
}

void SBlissTrackerSourceFactory::OnEndpointChanged(const FText& NewValue, ETextCommit::Type)
{
    TSharedPtr<SEditableTextBox> EditabledTextPin = EditabledText.Pin();
    if (EditabledTextPin.IsValid())
    {
        FIPv4Endpoint Endpoint;
        if (!FIPv4Endpoint::Parse(NewValue.ToString(), Endpoint))
        {
            Endpoint.Address = FIPv4Address::Any;
            Endpoint.Port = 54321;
            EditabledTextPin->SetText(FText::FromString(Endpoint.ToString()));
        }
    }
}

FReply SBlissTrackerSourceFactory::OnOkClicked()
{
    TSharedPtr<SEditableTextBox> EditabledTextPin = EditabledText.Pin();
    if (EditabledTextPin.IsValid())
    {
        FIPv4Endpoint Endpoint;
        if (FIPv4Endpoint::Parse(EditabledTextPin->GetText().ToString(), Endpoint))
        {
            OkClicked.ExecuteIfBound(Endpoint);
        }
    }
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
