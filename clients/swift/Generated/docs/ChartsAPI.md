# ChartsAPI

All URIs are relative to *https://api.premia.local*

Method | HTTP request | Description
------------- | ------------- | -------------
[**getChartScreen**](ChartsAPI.md#getchartscreen) | **GET** /v1/screens/charts/{symbol} | Load chart data for a symbol.


# **getChartScreen**
```swift
    open class func getChartScreen(symbol: String, range: ModelRange_getChartScreen? = nil, interval: Interval_getChartScreen? = nil, extendedHours: Bool? = nil, completion: @escaping (_ data: ChartScreenResponse?, _ error: Error?) -> Void)
```

Load chart data for a symbol.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let symbol = "symbol_example" // String | 
let range = "range_example" // String |  (optional)
let interval = "interval_example" // String |  (optional)
let extendedHours = true // Bool |  (optional)

// Load chart data for a symbol.
ChartsAPI.getChartScreen(symbol: symbol, range: range, interval: interval, extendedHours: extendedHours) { (response, error) in
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
 **range** | **String** |  | [optional] 
 **interval** | **String** |  | [optional] 
 **extendedHours** | **Bool** |  | [optional] 

### Return type

[**ChartScreenResponse**](ChartScreenResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

