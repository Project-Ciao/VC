// Copyright Epic Games, Inc. All Rights Reserved.

#include "TileAdjacencyMatchCustomization.h"
#include "GameplayTagContainer.h"
#include "DetailWidgetRow.h"
#include "IPropertyTypeCustomization.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"

#include "Tile.h"

class IPropertyHandle;
class SMenuAnchor;
class ITableRow;
class STableViewBase;
class SGameplayTagPicker;
class SComboButton;

static ECheckBoxState GetCheckboxState(TSharedPtr<IPropertyHandle> PropertyHandle, uint8 BoxBits)
{
	if (PropertyHandle)
	{
		uint8 AdjacencyNum;
		PropertyHandle->GetValue(AdjacencyNum);

		return (AdjacencyNum & BoxBits) == BoxBits ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}

	return ECheckBoxState::Undetermined;
}

class STileAdjacencyButton : public SCheckBox
{
	SLATE_BEGIN_ARGS(STileAdjacencyButton)
		: _PropertyHandle(nullptr)
		, _AdjacencyBits(1)
		{}
	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle)
	SLATE_ARGUMENT(uint8, AdjacencyBits)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		TSharedPtr<IPropertyHandle> PropertyHandle = InArgs._PropertyHandle;
		uint8 AdjacencyBits = InArgs._AdjacencyBits;

		SCheckBox::FArguments ParentArgs;
		ParentArgs
			.IsChecked_Lambda([PropertyHandle, AdjacencyBits]() -> ECheckBoxState {
				return GetCheckboxState(PropertyHandle, AdjacencyBits);
			})
			.OnCheckStateChanged(FOnCheckStateChanged::CreateLambda([PropertyHandle, AdjacencyBits](ECheckBoxState NewState)
			{
				if (PropertyHandle)
				{
					if (NewState == ECheckBoxState::Checked)
					{
						uint8 Value;
						PropertyHandle->GetValue(Value);
						Value |= AdjacencyBits;
						PropertyHandle->SetValue(Value);
					}
					else if (NewState == ECheckBoxState::Unchecked)
					{
						uint8 Value;
						PropertyHandle->GetValue(Value);
						Value &= ~AdjacencyBits;
						PropertyHandle->SetValue(Value);
					}
				}
			}));
		SCheckBox::Construct(ParentArgs);
	}
};

/**
 * Widget for editing a Tile Adjacency Match
 * This widget has 8 toggle boxes representing the match
 */
class STileAdjacencyMatchWidget : public SCompoundWidget
{
	SLATE_DECLARE_WIDGET(STileAdjacencyMatchWidget, SCompoundWidget)

public:

	SLATE_BEGIN_ARGS(STileAdjacencyMatchWidget)
		: _PropertyHandle(nullptr)
		{}

	SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle)

	SLATE_END_ARGS();

	STileAdjacencyMatchWidget()
	{

	}

	void Construct(const FArguments& InArgs)
	{
		TSharedPtr<IPropertyHandle> PropertyHandle = InArgs._PropertyHandle;

		//PropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &STileAdjacencyMatchWidget::CacheQueryList));

		ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Left)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				[
					SNew(STileAdjacencyButton) // NW
					.PropertyHandle(PropertyHandle)
					.AdjacencyBits(1 << 7)
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				[
					SNew(STileAdjacencyButton) // N
					.PropertyHandle(PropertyHandle)
					.AdjacencyBits(1 << 0)
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				[
					SNew(STileAdjacencyButton) // NE
					.PropertyHandle(PropertyHandle)
					.AdjacencyBits(1 << 1)
				]
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Left)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				[
					SNew(STileAdjacencyButton) // W
					.PropertyHandle(PropertyHandle)
					.AdjacencyBits(1 << 6)
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				[
					SNew(SCheckBox) // Middle
					.IsEnabled(false)
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				[
					SNew(STileAdjacencyButton) // E
					.PropertyHandle(PropertyHandle)
					.AdjacencyBits(1 << 2)
				]
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Left)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				[
					SNew(STileAdjacencyButton) // SW
					.PropertyHandle(PropertyHandle)
					.AdjacencyBits(1 << 5)
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				[
					SNew(STileAdjacencyButton) // S
					.PropertyHandle(PropertyHandle)
					.AdjacencyBits(1 << 4)
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				[
					SNew(STileAdjacencyButton) // SE
					.PropertyHandle(PropertyHandle)
					.AdjacencyBits(1 << 3)
				]
			]
		];
	}
};

SLATE_IMPLEMENT_WIDGET(STileAdjacencyMatchWidget)
void STileAdjacencyMatchWidget::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
	//SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "TagContainer", TagContainerAttribute, EInvalidateWidgetReason::Layout)
	//.OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda([](SWidget& Widget)
	//	{
	//		static_cast<STileAdjacencyMatchWidget&>(Widget).RefreshTagContainers();
	//	}));
}

void FTileAdjacencyMatchCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	HeaderRow
	.NameContent()
	[
		InStructPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		InStructPropertyHandle->CreatePropertyValueWidget()
	];

	AdjacencyMatches = InStructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FTileAdjacencyMatch, AdjacencyMatches));
}

void FTileAdjacencyMatchCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	if (InStructPropertyHandle->IsValidHandle())
	{
		ChildBuilder.AddCustomRow(INVTEXT("Adjacency Match Bits"))
			.NameContent()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Adjacency Match Bits")))
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
			.ValueContent()
			[
				SNew(STileAdjacencyMatchWidget)
				.PropertyHandle(AdjacencyMatches)
			];

		uint32 NumChildren = 0;
		InStructPropertyHandle->GetNumChildren(NumChildren);

		for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ChildIndex++)
		{
			ChildBuilder.AddProperty(InStructPropertyHandle->GetChildHandle(ChildIndex).ToSharedRef());
		}
	}
}
