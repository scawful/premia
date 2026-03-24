# WatchlistsAPI

All URIs are relative to *https://api.premia.local*

Method | HTTP request | Description
------------- | ------------- | -------------
[**addWatchlistSymbol**](WatchlistsAPI.md#addwatchlistsymbol) | **POST** /v1/watchlists/{watchlistId}/symbols | Add a symbol to a watchlist.
[**archiveWatchlist**](WatchlistsAPI.md#archivewatchlist) | **PATCH** /v1/watchlists/{watchlistId}/archive | Archive or restore a watchlist.
[**createWatchlist**](WatchlistsAPI.md#createwatchlist) | **POST** /v1/watchlists | Create a watchlist.
[**deleteWatchlist**](WatchlistsAPI.md#deletewatchlist) | **DELETE** /v1/watchlists/{watchlistId} | Delete a watchlist.
[**deleteWatchlistSymbol**](WatchlistsAPI.md#deletewatchlistsymbol) | **DELETE** /v1/watchlists/{watchlistId}/symbols/{symbol} | Remove a symbol from a watchlist.
[**getWatchlistScreen**](WatchlistsAPI.md#getwatchlistscreen) | **GET** /v1/screens/watchlists/{watchlistId} | Load a watchlist screen.
[**listWatchlists**](WatchlistsAPI.md#listwatchlists) | **GET** /v1/watchlists | List watchlists.
[**moveWatchlistSymbol**](WatchlistsAPI.md#movewatchlistsymbol) | **POST** /v1/watchlists/{watchlistId}/move | Reorder a symbol within a watchlist.
[**pinWatchlistSymbol**](WatchlistsAPI.md#pinwatchlistsymbol) | **PATCH** /v1/watchlists/{watchlistId}/symbols/{symbol} | Pin or unpin a symbol in a watchlist.
[**transferWatchlistSymbol**](WatchlistsAPI.md#transferwatchlistsymbol) | **POST** /v1/watchlists/{watchlistId}/transfer | Move a symbol into another watchlist.
[**updateWatchlist**](WatchlistsAPI.md#updatewatchlist) | **PATCH** /v1/watchlists/{watchlistId} | Update a watchlist.


# **addWatchlistSymbol**
```swift
    open class func addWatchlistSymbol(watchlistId: String, addWatchlistSymbolRequest: AddWatchlistSymbolRequest, completion: @escaping (_ data: WatchlistResponse?, _ error: Error?) -> Void)
```

Add a symbol to a watchlist.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let watchlistId = "watchlistId_example" // String | 
let addWatchlistSymbolRequest = AddWatchlistSymbolRequest(symbol: "symbol_example") // AddWatchlistSymbolRequest | 

// Add a symbol to a watchlist.
WatchlistsAPI.addWatchlistSymbol(watchlistId: watchlistId, addWatchlistSymbolRequest: addWatchlistSymbolRequest) { (response, error) in
    guard error == nil else {
        print(error)
        return
    }

    if (response) {
        dump(response)
    }
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **watchlistId** | **String** |  | 
 **addWatchlistSymbolRequest** | [**AddWatchlistSymbolRequest**](AddWatchlistSymbolRequest.md) |  | 

### Return type

[**WatchlistResponse**](WatchlistResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **archiveWatchlist**
```swift
    open class func archiveWatchlist(watchlistId: String, archiveWatchlistRequest: ArchiveWatchlistRequest, completion: @escaping (_ data: WatchlistResponse?, _ error: Error?) -> Void)
```

Archive or restore a watchlist.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let watchlistId = "watchlistId_example" // String | 
let archiveWatchlistRequest = ArchiveWatchlistRequest(archived: false) // ArchiveWatchlistRequest | 

// Archive or restore a watchlist.
WatchlistsAPI.archiveWatchlist(watchlistId: watchlistId, archiveWatchlistRequest: archiveWatchlistRequest) { (response, error) in
    guard error == nil else {
        print(error)
        return
    }

    if (response) {
        dump(response)
    }
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **watchlistId** | **String** |  | 
 **archiveWatchlistRequest** | [**ArchiveWatchlistRequest**](ArchiveWatchlistRequest.md) |  | 

### Return type

[**WatchlistResponse**](WatchlistResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **createWatchlist**
```swift
    open class func createWatchlist(createWatchlistRequest: CreateWatchlistRequest, completion: @escaping (_ data: WatchlistResponse?, _ error: Error?) -> Void)
```

Create a watchlist.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let createWatchlistRequest = CreateWatchlistRequest(name: "name_example") // CreateWatchlistRequest | 

// Create a watchlist.
WatchlistsAPI.createWatchlist(createWatchlistRequest: createWatchlistRequest) { (response, error) in
    guard error == nil else {
        print(error)
        return
    }

    if (response) {
        dump(response)
    }
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **createWatchlistRequest** | [**CreateWatchlistRequest**](CreateWatchlistRequest.md) |  | 

### Return type

[**WatchlistResponse**](WatchlistResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **deleteWatchlist**
```swift
    open class func deleteWatchlist(watchlistId: String, completion: @escaping (_ data: WatchlistResponse?, _ error: Error?) -> Void)
```

Delete a watchlist.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let watchlistId = "watchlistId_example" // String | 

// Delete a watchlist.
WatchlistsAPI.deleteWatchlist(watchlistId: watchlistId) { (response, error) in
    guard error == nil else {
        print(error)
        return
    }

    if (response) {
        dump(response)
    }
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **watchlistId** | **String** |  | 

### Return type

[**WatchlistResponse**](WatchlistResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **deleteWatchlistSymbol**
```swift
    open class func deleteWatchlistSymbol(watchlistId: String, symbol: String, completion: @escaping (_ data: WatchlistResponse?, _ error: Error?) -> Void)
```

Remove a symbol from a watchlist.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let watchlistId = "watchlistId_example" // String | 
let symbol = "symbol_example" // String | 

// Remove a symbol from a watchlist.
WatchlistsAPI.deleteWatchlistSymbol(watchlistId: watchlistId, symbol: symbol) { (response, error) in
    guard error == nil else {
        print(error)
        return
    }

    if (response) {
        dump(response)
    }
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **watchlistId** | **String** |  | 
 **symbol** | **String** |  | 

### Return type

[**WatchlistResponse**](WatchlistResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **getWatchlistScreen**
```swift
    open class func getWatchlistScreen(watchlistId: String, completion: @escaping (_ data: WatchlistScreenResponse?, _ error: Error?) -> Void)
```

Load a watchlist screen.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let watchlistId = "watchlistId_example" // String | 

// Load a watchlist screen.
WatchlistsAPI.getWatchlistScreen(watchlistId: watchlistId) { (response, error) in
    guard error == nil else {
        print(error)
        return
    }

    if (response) {
        dump(response)
    }
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **watchlistId** | **String** |  | 

### Return type

[**WatchlistScreenResponse**](WatchlistScreenResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **listWatchlists**
```swift
    open class func listWatchlists(completion: @escaping (_ data: WatchlistsResponse?, _ error: Error?) -> Void)
```

List watchlists.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated


// List watchlists.
WatchlistsAPI.listWatchlists() { (response, error) in
    guard error == nil else {
        print(error)
        return
    }

    if (response) {
        dump(response)
    }
}
```

### Parameters
This endpoint does not need any parameter.

### Return type

[**WatchlistsResponse**](WatchlistsResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **moveWatchlistSymbol**
```swift
    open class func moveWatchlistSymbol(watchlistId: String, moveWatchlistSymbolRequest: MoveWatchlistSymbolRequest, completion: @escaping (_ data: WatchlistResponse?, _ error: Error?) -> Void)
```

Reorder a symbol within a watchlist.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let watchlistId = "watchlistId_example" // String | 
let moveWatchlistSymbolRequest = MoveWatchlistSymbolRequest(symbol: "symbol_example", beforeSymbol: "beforeSymbol_example") // MoveWatchlistSymbolRequest | 

// Reorder a symbol within a watchlist.
WatchlistsAPI.moveWatchlistSymbol(watchlistId: watchlistId, moveWatchlistSymbolRequest: moveWatchlistSymbolRequest) { (response, error) in
    guard error == nil else {
        print(error)
        return
    }

    if (response) {
        dump(response)
    }
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **watchlistId** | **String** |  | 
 **moveWatchlistSymbolRequest** | [**MoveWatchlistSymbolRequest**](MoveWatchlistSymbolRequest.md) |  | 

### Return type

[**WatchlistResponse**](WatchlistResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **pinWatchlistSymbol**
```swift
    open class func pinWatchlistSymbol(watchlistId: String, symbol: String, pinWatchlistSymbolRequest: PinWatchlistSymbolRequest, completion: @escaping (_ data: WatchlistResponse?, _ error: Error?) -> Void)
```

Pin or unpin a symbol in a watchlist.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let watchlistId = "watchlistId_example" // String | 
let symbol = "symbol_example" // String | 
let pinWatchlistSymbolRequest = PinWatchlistSymbolRequest(pinned: false) // PinWatchlistSymbolRequest | 

// Pin or unpin a symbol in a watchlist.
WatchlistsAPI.pinWatchlistSymbol(watchlistId: watchlistId, symbol: symbol, pinWatchlistSymbolRequest: pinWatchlistSymbolRequest) { (response, error) in
    guard error == nil else {
        print(error)
        return
    }

    if (response) {
        dump(response)
    }
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **watchlistId** | **String** |  | 
 **symbol** | **String** |  | 
 **pinWatchlistSymbolRequest** | [**PinWatchlistSymbolRequest**](PinWatchlistSymbolRequest.md) |  | 

### Return type

[**WatchlistResponse**](WatchlistResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **transferWatchlistSymbol**
```swift
    open class func transferWatchlistSymbol(watchlistId: String, transferWatchlistSymbolRequest: TransferWatchlistSymbolRequest, completion: @escaping (_ data: WatchlistResponse?, _ error: Error?) -> Void)
```

Move a symbol into another watchlist.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let watchlistId = "watchlistId_example" // String | 
let transferWatchlistSymbolRequest = TransferWatchlistSymbolRequest(symbol: "symbol_example", destinationWatchlistId: "destinationWatchlistId_example") // TransferWatchlistSymbolRequest | 

// Move a symbol into another watchlist.
WatchlistsAPI.transferWatchlistSymbol(watchlistId: watchlistId, transferWatchlistSymbolRequest: transferWatchlistSymbolRequest) { (response, error) in
    guard error == nil else {
        print(error)
        return
    }

    if (response) {
        dump(response)
    }
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **watchlistId** | **String** |  | 
 **transferWatchlistSymbolRequest** | [**TransferWatchlistSymbolRequest**](TransferWatchlistSymbolRequest.md) |  | 

### Return type

[**WatchlistResponse**](WatchlistResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **updateWatchlist**
```swift
    open class func updateWatchlist(watchlistId: String, updateWatchlistRequest: UpdateWatchlistRequest, completion: @escaping (_ data: WatchlistResponse?, _ error: Error?) -> Void)
```

Update a watchlist.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let watchlistId = "watchlistId_example" // String | 
let updateWatchlistRequest = UpdateWatchlistRequest(name: "name_example") // UpdateWatchlistRequest | 

// Update a watchlist.
WatchlistsAPI.updateWatchlist(watchlistId: watchlistId, updateWatchlistRequest: updateWatchlistRequest) { (response, error) in
    guard error == nil else {
        print(error)
        return
    }

    if (response) {
        dump(response)
    }
}
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **watchlistId** | **String** |  | 
 **updateWatchlistRequest** | [**UpdateWatchlistRequest**](UpdateWatchlistRequest.md) |  | 

### Return type

[**WatchlistResponse**](WatchlistResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

