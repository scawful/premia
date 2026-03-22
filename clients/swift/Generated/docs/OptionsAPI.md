# OptionsAPI

All URIs are relative to *https://api.premia.local*

Method | HTTP request | Description
------------- | ------------- | -------------
[**getOptionChainScreen**](OptionsAPI.md#getoptionchainscreen) | **GET** /v1/screens/options/{symbol} | Load option chain data for a symbol.


# **getOptionChainScreen**
```swift
    open class func getOptionChainScreen(symbol: String, strikeCount: String? = nil, strategy: String? = nil, range: String? = nil, expMonth: String? = nil, optionType: String? = nil, completion: @escaping (_ data: OptionChainResponse?, _ error: Error?) -> Void)
```

Load option chain data for a symbol.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let symbol = "symbol_example" // String | 
let strikeCount = "strikeCount_example" // String |  (optional)
let strategy = "strategy_example" // String |  (optional)
let range = "range_example" // String |  (optional)
let expMonth = "expMonth_example" // String |  (optional)
let optionType = "optionType_example" // String |  (optional)

// Load option chain data for a symbol.
OptionsAPI.getOptionChainScreen(symbol: symbol, strikeCount: strikeCount, strategy: strategy, range: range, expMonth: expMonth, optionType: optionType) { (response, error) in
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
 **symbol** | **String** |  | 
 **strikeCount** | **String** |  | [optional] 
 **strategy** | **String** |  | [optional] 
 **range** | **String** |  | [optional] 
 **expMonth** | **String** |  | [optional] 
 **optionType** | **String** |  | [optional] 

### Return type

[**OptionChainResponse**](OptionChainResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

