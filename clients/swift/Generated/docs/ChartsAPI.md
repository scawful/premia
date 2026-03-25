# ChartsAPI

All URIs are relative to *https://api.premia.local*

Method | HTTP request | Description
------------- | ------------- | -------------
[**getChartScreen**](ChartsAPI.md#getchartscreen) | **GET** /v1/screens/charts/{symbol} | Load chart data for a symbol.
[**replaceChartAnnotations**](ChartsAPI.md#replacechartannotations) | **PUT** /v1/screens/charts/{symbol}/annotations | Replace editable chart annotations for a symbol.


# **getChartScreen**
```swift
    open class func getChartScreen(symbol: String, accountId: String? = nil, range: ModelRange_getChartScreen? = nil, interval: Interval_getChartScreen? = nil, extendedHours: Bool? = nil, completion: @escaping (_ data: ChartScreenResponse?, _ error: Error?) -> Void)
```

Load chart data for a symbol.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let symbol = "symbol_example" // String | 
let accountId = "accountId_example" // String |  (optional)
let range = "range_example" // String |  (optional)
let interval = "interval_example" // String |  (optional)
let extendedHours = true // Bool |  (optional)

// Load chart data for a symbol.
ChartsAPI.getChartScreen(symbol: symbol, accountId: accountId, range: range, interval: interval, extendedHours: extendedHours) { (response, error) in
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
 **accountId** | **String** |  | [optional] 
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

# **replaceChartAnnotations**
```swift
    open class func replaceChartAnnotations(symbol: String, replaceChartAnnotationsRequest: ReplaceChartAnnotationsRequest, completion: @escaping (_ data: ChartScreenResponse?, _ error: Error?) -> Void)
```

Replace editable chart annotations for a symbol.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let symbol = "symbol_example" // String | 
let replaceChartAnnotationsRequest = ReplaceChartAnnotationsRequest(accountId: "accountId_example", annotations: [ChartAnnotation(id: "id_example", label: "label_example", price: "price_example", kind: "kind_example")]) // ReplaceChartAnnotationsRequest | 

// Replace editable chart annotations for a symbol.
ChartsAPI.replaceChartAnnotations(symbol: symbol, replaceChartAnnotationsRequest: replaceChartAnnotationsRequest) { (response, error) in
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
 **replaceChartAnnotationsRequest** | [**ReplaceChartAnnotationsRequest**](ReplaceChartAnnotationsRequest.md) |  | 

### Return type

[**ChartScreenResponse**](ChartScreenResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

