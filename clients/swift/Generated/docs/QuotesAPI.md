# QuotesAPI

All URIs are relative to *https://api.premia.local*

Method | HTTP request | Description
------------- | ------------- | -------------
[**getQuoteScreen**](QuotesAPI.md#getquotescreen) | **GET** /v1/screens/quotes/{symbol} | Load quote detail for a symbol.


# **getQuoteScreen**
```swift
    open class func getQuoteScreen(symbol: String, completion: @escaping (_ data: QuoteScreenResponse?, _ error: Error?) -> Void)
```

Load quote detail for a symbol.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let symbol = "symbol_example" // String | 

// Load quote detail for a symbol.
QuotesAPI.getQuoteScreen(symbol: symbol) { (response, error) in
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

### Return type

[**QuoteScreenResponse**](QuoteScreenResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

