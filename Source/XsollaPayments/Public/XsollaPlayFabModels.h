// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "XsollaPaymentsSettings.h"


#include "XsollaPlayFabModels.generated.h"


// -----------------------Requests--------------------------

/**
* Equal of FClientLoginWithPlayFabRequest.
* 
* Username and password lengths are provided for information purposes. The server will validate that data passed in
* conforms to the field definition and report errors appropriately. It is recommended that developers not perform this
* validation locally, so that future updates to the username or password do not require client updates.
*/
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaClientLoginRequest
{
	GENERATED_BODY()
public:

	/** Flags for which pieces of info to return for the user. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayFab | Client | Authentication Models")
	UObject* InfoRequestParameters = nullptr;

	/** PlayFab username for the account. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayFab | Client | Authentication Models")
	FString Username;

	/** Password for the PlayFab account (6-100 characters) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayFab | Client | Authentication Models")
	FString Password;

	/** PlayFab TitleID */
	UPROPERTY()
	FString TitleID;
};

/** Combined entity type and ID structure which uniquely identifies a single entity. */
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaEntityKey
{
	GENERATED_BODY()

	/** Unique ID of the entity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	FString Id;

	/** Entity type.
	 *  See https://docs.microsoft.com/gaming/playfab/features/data/entities/available-built-in-entity-types
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	FString Type;
};

USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaEntityTokenResponse
{
	GENERATED_BODY()

	/** The entity id and type. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	FXsollaEntityKey EntityKey;

	/** The token used to set X-EntityToken for all entity based API calls */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	FString EntityToken;

	/** The time the token will expire, if it is an expiring token, in UTC. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	FString TokenExpiration;
};

USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaPlayerCombinedInfoResultPayload
{
	GENERATED_BODY()

	// /** Account information for the user. This is always retrieved. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	// FXsollaUserAccountInfo AccountInfo;
	//
	// /** Inventories for each character for the user */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	// TArray<FXsollaCharacterInventory> CharacterInventories;
	//
	// /** List of characters for the user */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	// TArray<FXsollaCharacterResult> CharacterList;
	//
	// /** The profile of the players. This profile is not guaranteed to be up-to-date.
	//  *  For a new player, this profile will not exist.
	//  */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	// FXsollaPlayerProfileModel PlayerProfile;
	//
	// /** List of statistics for this player. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	// TArray<FXsollaStatisticValue> PlayerStatistics;
	//
	// /** Title data for this title. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	// UObject* TitleData;
	//
	// /** User specific custom data. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	// FXsollaUserDataRecord UserData;
	//
	// /** The version of the UserData that was returned. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	// int32 UserDataVersion;
	//
	// /** Array of inventory items in the user's current inventory. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	// TArray<FXsollaItemInstance> UserInventory;
	//
	// /** User specific read-only data. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	// FXsollaUserDataRecord UserReadOnlyData;
	//
	// /** The version of the Read-Only UserData that was returned. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	// int32 UserReadOnlyDataVersion;
	//
	// /** Dictionary of virtual currency balance(s) belonging to the user. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	// UObject* UserVirtualCurrency;
	//
	// /** Dictionary of remaining times and timestamps for virtual currencies. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	// FXsollaVirtualCurrencyRechargeTime UserVirtualCurrencyRechargeTimes;
};

USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaClientLoginResult
{
	GENERATED_BODY()
public:

	/**
	* If LoginTitlePlayerAccountEntity flag is set on the login request the title_player_account will also be logged in and
	* returned.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	FXsollaEntityTokenResponse EntityToken;

	/** Results for requested info. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	FXsollaPlayerCombinedInfoResultPayload InfoResultPayload;

	/** The time of this user's previous login. If there was no previous login, then it's DateTime.MinValue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	FString LastLoginTime;

	/** True if the account was newly created on this login. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	bool NewlyCreated = false;

	/** Player's unique PlayFabId. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	FString PlayFabId;

	/** Unique token authorizing the user and game at the server level, for the current session. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	FString SessionTicket;

	/** Settings specific to this user. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	TMap<FString, FString> SettingsForUser;

	/** The experimentation treatments for this user at the time of login. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	TMap<FString, FString> TreatmentAssignment;
};

USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaCatalogItemBundleInfo
{
	GENERATED_BODY()

	/** Unique ItemId values for all items which will be added to the player inventory when the bundle is added */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	TArray<FString> BundledItems;

	/** Unique TableId values for all RandomResultTable objects which are part of the bundle
	 *  (random tables will be resolved and add the relevant items to the player inventory when the bundle is added) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	TArray<FString> BundledResultTables;

	/** Virtual currency types and balances which will be added to the player inventory when the bundle is added */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	TMap<FString, float> BundledVirtualCurrencies;
};

USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaCatalogItemConsumableInfo
{
	GENERATED_BODY()

	/** Number of times this object can be used, after which it will be removed from the player inventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	int32 UsageCount;

	/** Duration in seconds for how long the item will remain in the player inventory - once elapsed,
	 *  the item will be removed (recommended minimum value is 5 seconds, as lower values can cause
	 *  the item to expire before operations depending on this item's details have completed)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	int32 UsagePeriod;

	/** All inventory item instances in the player inventory sharing a non-null UsagePeriodGroup have their UsagePeriod values added together,
	 *  and share the result - when that period has elapsed, all the items in the group will be removed
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	FString UsagePeriodGroup;
};

/**
 *  Containers are inventory items that can hold other items defined in the catalog, as well as virtual currency,
 *  which is added to the player inventory when the container is unlocked, using the UnlockContainerItem API.
 *  The items can be anything defined in the catalog, as well as RandomResultTable objects which will be resolved when the container is unlocked.
 *  Containers and their keys should be defined as Consumable (having a limited number of uses) in their catalog definitions,
 *  unless the intent is for the player to be able to re-use them infinitely.
 */
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaCatalogItemContainerInfo
{
	GENERATED_BODY()

	/** Unique ItemId values for all items which will be added to the player inventory, once the container has been unlocked */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	TArray<FString> ItemContents;

	/** ItemId for the catalog item used to unlock the container,
	 *  if any (if not specified, a call to UnlockContainerItem will open the container,
	 *  adding the contents to the player inventory and currency balances)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	FString KeyItemId;

	/** Unique TableId values for all RandomResultTable objects which are part of the container
	 *  (once unlocked, random tables will be resolved and add the relevant items to the player inventory) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	TArray<FString> ResultTableContents;

	/** Virtual currency types and balances which will be added to the player inventory when the container is unlocked */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Login")
	TMap<FString, float> VirtualCurrencyContents;
};


/** A purchasable item from the item catalog */
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaPlayFabCatalogItem
{
	GENERATED_BODY()

	/** Defines the bundle properties for the item - bundles are items which contain other items, including random drop tables and virtual currencies */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	FXsollaCatalogItemBundleInfo Bundle;

	/** If true, then an item instance of this type can be used to grant a character to a user. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	bool CanBecomeCharacter;

	/** Catalog version for this item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	FString CatalogVersion;

	/** Defines the consumable properties (number of uses, timeout) for the item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	FXsollaCatalogItemConsumableInfo Consumable;

	/** Defines the container properties for the item - what items it contains, including random drop tables and virtual currencies, and what item (if any) is required to open it via the UnlockContainerItem API */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	FXsollaCatalogItemContainerInfo Container;

	/** Game specific custom data */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	FString CustomData;

	/** Text description of item, to show in-game */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	FString Description;

	/** Text name for the item, to show in-game */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	FString DisplayName;

	/** If the item has IsLimitedEdition set to true, and this is the first time this ItemId has been defined as a limited edition item,
	 *  this value determines the total number of instances to allocate for the title. Once this limit has been reached,
	 *  no more instances of this ItemId can be created, and attempts to purchase or grant it will return a Result of false for that ItemId.
	 *  If the item has already been defined to have a limited edition count, or if this value is less than zero, it will be ignored.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	int32 InitialLimitedEditionCount;

	/** BETA: If true, then only a fixed number can ever be granted. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	bool IsLimitedEdition;

	/** If true, then only one item instance of this type will exist and its remaining uses will be incremented instead.
	 *  RemainingUses will cap out at Int32.Max (2,147,483,647). All subsequent increases will be discarded */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	bool IsStackable;

	/** If true, then an item instance of this type can be traded between players using the trading APIs */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	bool IsTradable;

	/** Class to which the item belongs */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	FString ItemClass;

	/** Unique identifier for this item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	FString ItemId;

	/** URL to the item image. For Facebook purchase to display the image on the item purchase page, this must be set to an HTTP URL. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	FString ItemImageUrl;

	/** Override prices for this item for specific currencies */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	TMap<FString, float> RealCurrencyPrices;

	/** List of item tags */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	TArray<FString> Tags;

	/** Price of this item in virtual currencies and "RM" (the base Real Money purchase price, in USD pennies) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	TMap<FString, float> VirtualCurrencyPrices;
};

/** If CatalogVersion is not specified, only inventory items associated with the most recent version of the catalog will be returned. */
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaCatalogItemsResult
{
	GENERATED_BODY()

	/** Array of items which can be purchased. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Items")
	TArray<FXsollaPlayFabCatalogItem> Catalog;
};

/** The basic wrapper around every failed API response */
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaApiErrorWrapper
{
	GENERATED_BODY()

	/** Numerical HTTP code */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Error")
	int32 Code;

	/** PlayFab error code */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Error")
	FString Error;

	/** Numerical PlayFab error code */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Error")
	int32 ErrorCode;

	/** Detailed description of individual issues with request object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Error")
	TMap<FString, FString> ErrorDetails;

	/** Description for the PlayFab errorCode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Error")
	FString ErrorMessage;

	/** String HTTP code */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Error")
	FString Status;
};

USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaRegisterRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Register")
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Register")
	FString Email;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Register")
	FString Password;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Register")
	FString Username;
};

USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaPlayerCombinedInfoRequestParams
{
	GENERATED_BODY()
};

USTRUCT()
struct XSOLLAPAYMENTS_API FXsollaClientRegisterUserRequest
{
	GENERATED_BODY()

	/** The optional custom tags associated with the request (e.g. build number, external trace identifiers, etc.). */
	UPROPERTY()
	TMap<FString, FString> CustomTags;

	/** An optional parameter for setting the display name for this title (3-25 characters). */
	UPROPERTY()
	FString DisplayName;

	/** User email address attached to their account */
	UPROPERTY()
	FString Email;

	/** Base64 encoded body that is encrypted with the Title's public RSA key (Enterprise Only). */
	UPROPERTY()
	FString EncryptedRequest;

	/** Flags for which pieces of info to return for the user. */
	UPROPERTY()
	FXsollaPlayerCombinedInfoRequestParams InfoRequestParameters;

	/** Password for the PlayFab account (6-100 characters) */
	UPROPERTY()
	FString Password;

	/** Player secret that is used to verify API request signatures (Enterprise Only). */
	UPROPERTY()
	FString PlayerSecret;

	/** An optional parameter that specifies whether both the username and email parameters are required.
	 *  If true, both parameters are required; if false, the user must supply either the username or email parameter.
	 *  The default value is true.
	 */
	UPROPERTY()
	bool RequireBothUsernameAndEmail;

	/** Unique identifier for the title, found in the Settings > Game Properties section of the PlayFab developer site when a title has been selected. */
	UPROPERTY()
	FString TitleId;

	/** PlayFab username for the account (3-20 characters) */
	UPROPERTY()
	FString Username;
};

USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaUserSettings
{
	GENERATED_BODY()

	/** Boolean for whether this player is eligible for gathering device info. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Register")
	bool GatherDeviceInfo;

	/** Boolean for whether this player should report OnFocus play-time tracking. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Register")
	bool GatherFocusInfo;

	/** Boolean for whether this player is eligible for ad tracking. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Register")
	bool NeedsAttribution;
};

/** Each account must have a unique email address in the PlayFab service. Once created,
 *  the account may be associated with additional accounts (Steam, Facebook, Game Center, etc.),
 *  allowing for added social network lists and achievements systems.
 */
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaRegisterUserResult
{
	GENERATED_BODY()

	/** If LoginTitlePlayerAccountEntity flag is set on the login request the title_player_account will also be logged in and returned. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Register")
	FXsollaEntityTokenResponse EntityToken;

	/** PlayFab unique identifier for this newly created account. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Register")
	FString PlayFabId;

	/** Unique token identifying the user and game at the server level, for the current session. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Register")
	FString SessionTicket;

	/** Settings specific to this user. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Register")
	FXsollaUserSettings UserSettings;

	/** PlayFab unique user name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Register")
	FString Username;
};

/** If the account in question is a "temporary" account (for example,
 *  one that was created via a call to LoginFromIOSDeviceID), this function will have no effect.
 *  Only PlayFab accounts which have valid email addresses will be able to receive a password reset email using this API.
 */
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaRecoveryEmailRequest
{
	GENERATED_BODY()

	/** User email address attached to their account */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Recovery")
	FString Email;
};

/** Structure for response from request */
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaRecoveryEmailResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Recovery")
	FString Code;
};

/** A unique instance of an item in a user's inventory.
 *  Note, to retrieve additional information for an item such as Tags,
 *  Description that are the same across all instances of the item, a call to GetCatalogItems is required.
 *  The ItemID of can be matched to a catalog entry, which contains the additional information.
 *  Also note that Custom Data is only set when the User's specific instance has updated the CustomData
 *  via a call to UpdateUserInventoryItemCustomData. Other fields such as UnitPrice and UnitCurrency are only set when
 *  the item was granted via a purchase.
 */
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaItemInstance
{
	GENERATED_BODY()

	/** Game specific comment associated with this instance when it was added to the user inventory. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString Annotation;

	/** Array of unique items that were awarded when this catalog item was purchased. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	TArray<FString> BundleContents;

	/** Unique identifier for the parent inventory item, as defined in the catalog,
	 *  for object which were added from a bundle or container.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString BundleParent;

	/** Catalog version for the inventory item, when this instance was created. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString CatalogVersion;

	/** A set of custom key-value pairs on the instance of the inventory item,
	 *  which is not to be confused with the catalog item's custom data.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	TMap<FString, FString> CustomData;

	/** CatalogItem.DisplayName at the time this item was purchased. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString DisplayName;

	/** Timestamp for when this instance will expire. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString Expiration;

	/** Class name for the inventory item, as defined in the catalog. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString ItemClass;

	/** Unique identifier for the inventory item, as defined in the catalog. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString ItemId;

	/** Unique item identifier for this specific instance of the item. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString ItemInstanceId;

	/** Timestamp for when this instance was purchased. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString PurchaseDate;

	/** Total number of remaining uses, if this is a consumable item. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	int32 RemainingUses;

	/** Currency type for the cost of the catalog item. Not available when granting items. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString UnitCurrency;

	/** Cost of the catalog item in the given currency. Not available when granting items. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	int32 UnitPrice;

	/** The number of uses that were added or removed to this item in this call. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	int32 UsesIncrementedBy;
};

USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaVirtualCurrencyRechargeTime
{
	GENERATED_BODY()

	/** Maximum value to which the regenerating currency will automatically increment.
	 *  Note that it can exceed this value through use of the AddUserVirtualCurrency API call.
	 *  However, it will not regenerate automatically until it has fallen below this value.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	int32 RechargeMax;

	/** Server timestamp in UTC indicating the next time the virtual currency will be incremented. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString RechargeTime;

	/** Time remaining (in seconds) before the next recharge increment of the virtual currency. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	int32 SecondsToRecharge;
};

/** All items currently in the user inventory will be returned, irrespective of how they were acquired (via purchasing, grants, coupons, etc.).
 *  Items that are expired, fully consumed, or are no longer valid are not considered to be in the user's current inventory,
 *  and so will not be not included.
 */
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaUserInventoryResult
{
	GENERATED_BODY()

	/** Array of inventory items belonging to the user. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	TArray<FXsollaItemInstance> Inventory;

	/** Array of virtual currency balance(s) belonging to the user. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	TMap<FString, float> VirtualCurrency;

	/** Array of remaining times and timestamps for virtual currencies. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Recovery")
	FXsollaVirtualCurrencyRechargeTime VirtualCurrencyRechargeTimes;
};

/** Struct for PurchaseItem API function */
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaPurchaseItemRequest
{
	GENERATED_BODY()

	/** Catalog version for the items to be purchased (defaults to most recent version. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString CatalogVersion;

	/** Unique PlayFab assigned ID for a specific character owned by a user */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString CharacterId;

	/** The optional custom tags associated with the request (e.g. build number, external trace identifiers, etc.). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	TMap<FString, FString> CustomTags;

	/** Unique identifier of the item to purchase. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString ItemId;

	/** Price the client expects to pay for the item (in case a new catalog or store was uploaded, with new prices). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	int32 Price;

	/** Store to buy this item through. If not set, prices default to those in the catalog. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString StoreId;

	/** Virtual currency to use to purchase the item. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString VirtualCurrency;
};

USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaPurchaseItemResult
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	TArray<FXsollaItemInstance> Items;
};

/** Struct for PlayFab API functions */
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaItemPurchaseRequest
{
	GENERATED_BODY()

	/** Title-specific text concerning this purchase. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString Annotation;

	/** Unique ItemId of the item to purchase. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString ItemId;

	/** How many of this item to purchase. Min 1, maximum 25. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	int32 Quantity;

	/** Items to be upgraded as a result of this purchase (upgraded items are hidden, as they are "replaced" by the new items). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	TArray<FString> UpgradeFromItems;
};

/** This is the first step in the purchasing process. For security purposes, once the order (or "cart") has been
 *  created, additional inventory objects may no longer be added. In addition, inventory objects will be locked
 *  to the current prices, regardless of any subsequent changes at the catalog level which may occur during the next
 *  two steps.
 */
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaStartPurchaseRequest
{
	GENERATED_BODY()

	/** Catalog version for the items to be purchased. Defaults to most recent catalog. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString CatalogVersion;

	/** Array of items to purchase. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	TArray<FXsollaItemPurchaseRequest> Items;

	/** Store through which to purchase items. If not set, prices will be pulled from the catalog itself. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString StoreId;
};

USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaCartItem
{
	GENERATED_BODY()

	/** Description of the catalog item. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString Description;

	/** Display name for the catalog item. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString DisplayName;

	/** Class name to which catalog item belongs. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString ItemClass;

	/** Unique identifier for the catalog item. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString ItemId;

	/** Unique instance identifier for this catalog item. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString ItemInstanceId;

	/** Cost of the catalog item for each applicable real world currency. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	TMap<FString, float> RealCurrencyPrices;

	/** Amount of each applicable virtual currency which will be received as a result of purchasing this catalog item. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	TMap<FString, float> VCAmount;

	/** Cost of the catalog item for each applicable virtual currency. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	TMap<FString, float> VirtualCurrencyPrices;
};

USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaPaymentOption
{
	GENERATED_BODY()

	/** Specific currency to use to fund the purchase. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString Currency;

	/** Amount of the specified currency needed for the purchase. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	int32 Price;

	/** Name of the purchase provider for this option. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString ProviderName;

	/** Amount of existing credit the user has with the provider. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	int32 StoreCredit;
};

/** Struct for PlayFab API functions */
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaStartPurchaseResult
{
	GENERATED_BODY()

	/** Cart items to be purchased. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	TArray<FXsollaCartItem> Contents;

	/** Purchase order identifier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString OrderId;

	/** Available methods by which the user can pay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	TArray<FXsollaPaymentOption> PaymentOptions;

	/** Current virtual currency totals for the user. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	TMap<FString, float> VirtualCurrencyBalances;
};


/** Struct for PlayFab API functions */
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaGetPurchaseRequest
{
	GENERATED_BODY()

	/** Purchase order identifier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString OrderId;
};

/** Struct for PlayFab API functions */
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaGetPurchaseResult
{
	GENERATED_BODY()

	/** Purchase order identifier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString OrderId;

	/** Payment provider used for transaction (If not VC) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString PaymentProvider;

	/** Date and time of the purchase */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString PurchaseDate;

	/** Provider transaction ID (If not VC) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString TransactionId;

	/** PlayFab transaction status */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|Shop")
	FString TransactionStatus;
};

USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaCloudScriptRevisionOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	FString Latest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	FString Live;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	FString Specific;
};

/** Struct for PlayFab ABI functions */
USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaExecuteCloudScriptRequest
{
	GENERATED_BODY()

	/** The name of the CloudScript function to execute */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	FString FunctionName;

	/** Object that is passed in to the function as the first argument */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	UObject* FunctionParameter;

	/** Generate a 'player_executed_cloudscript' PlayStream event containing the results of the function execution and
	 *  other contextual information. This event will show up in the PlayStream debugger console for the player
	 *  in Game Manager.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	bool GeneratePlayStreamEvent;

	/** Option for which revision of the CloudScript to execute. 'Latest' executes the most recently created revision,
	 *  'Live' executes the current live, published revision, and 'Specific' executes the specified revision.
	 *  The default value is 'Specific', if the SpecificRevision parameter is specified, otherwise it is 'Live'.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	FXsollaCloudScriptRevisionOption RevisionSelection;

	/** The specific revision to execute, when RevisionSelection is set to 'Specific' */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	int32 SpecificRevision;
};

USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaScriptExecutionError
{
	GENERATED_BODY()

	/** Error code, such as CloudScriptNotFound, JavascriptException, CloudScriptFunctionArgumentSizeExceeded,
	 *  CloudScriptAPIRequestCountExceeded, CloudScriptAPIRequestError, or CloudScriptHTTPRequestError  
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	FString Error;

	/** Details about the error */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	FString Message;

	/** Point during the execution of the script at which the error occured, if any */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	FString StackTrace;
};

USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaLogStatement
{
	GENERATED_BODY()

	/** Optional object accompanying the message as contextual information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	UObject* Data;

	/** 'Debug', 'Info', or 'Error' */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	FString Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	FString Message;
};

USTRUCT(BlueprintType)
struct XSOLLAPAYMENTS_API FXsollaExecuteCloudScriptResult
{
	GENERATED_BODY()

	/** Number of PlayFab API requests issued by the CloudScript function */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	int32 APIRequestsIssued;

	/** Information about the error, if any, that occurred during execution */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	FXsollaScriptExecutionError Error;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	int32 ExecutionTimeSeconds;

	/** The name of the function that executed */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	FString FunctionName;

	/** The object returned from the CloudScript function, if any */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	UObject* FunctionResult;

	/** Flag indicating if the FunctionResult was too large and was subsequently dropped from this event.
	 *  This only occurs if the total event size is larger than 350KB.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	bool FunctionResultTooLarge;

	/** Number of external HTTP requests issued by the CloudScript function */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	int32 HttpRequestsIssued;

	/** Entries logged during the function execution. These include both entries logged in the function code using
	 *  log.info() and log.error() and error entries for API and HTTP request failures.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	TArray<FXsollaLogStatement> Logs;

	/** Flag indicating if the logs were too large and were subsequently dropped from this event.
	 *  This only occurs if the total event size is larger than 350KB after the FunctionResult was removed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	bool LogsTooLarge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	int32 MemoryConsumedBytes;

	/** Processor time consumed while executing the function. This does not include time spent waiting on
	 *  API calls or HTTP requests.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	int32 ProcessorTimeSeconds;

	/** The revision of the CloudScript that executed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Xsolla|CloudScript")
	int32 Revision;
};