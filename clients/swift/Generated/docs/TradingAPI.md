# TradingAPI

All URIs are relative to *https://api.premia.local*

Method | HTTP request | Description
------------- | ------------- | -------------
[**previewOrder**](TradingAPI.md#previeworder) | **POST** /v1/orders/preview | Preview an equity or option order.
[**submitOrder**](TradingAPI.md#submitorder) | **POST** /v1/orders/submit | Submit an equity or option order.


# **previewOrder**
```swift
    open class func previewOrder(orderIntentRequest: OrderIntentRequest, completion: @escaping (_ data: OrderPreviewResponse?, _ error: Error?) -> Void)
```

Preview an equity or option order.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let orderIntentRequest = OrderIntentRequest(accountId: "accountId_example", symbol: "symbol_example", assetType: "assetType_example", instruction: "instruction_example", quantity: "quantity_example", orderType: "orderType_example", limitPrice: "limitPrice_example", duration: "duration_example", session: "session_example", confirmLive: false) // OrderIntentRequest | 

// Preview an equity or option order.
TradingAPI.previewOrder(orderIntentRequest: orderIntentRequest) { (response, error) in
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
 **orderIntentRequest** | [**OrderIntentRequest**](OrderIntentRequest.md) |  | 

### Return type

[**OrderPreviewResponse**](OrderPreviewResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **submitOrder**
```swift
    open class func submitOrder(orderIntentRequest: OrderIntentRequest, completion: @escaping (_ data: OrderSubmissionResponse?, _ error: Error?) -> Void)
```

Submit an equity or option order.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let orderIntentRequest = OrderIntentRequest(accountId: "accountId_example", symbol: "symbol_example", assetType: "assetType_example", instruction: "instruction_example", quantity: "quantity_example", orderType: "orderType_example", limitPrice: "limitPrice_example", duration: "duration_example", session: "session_example", confirmLive: false) // OrderIntentRequest | 

// Submit an equity or option order.
TradingAPI.submitOrder(orderIntentRequest: orderIntentRequest) { (response, error) in
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
 **orderIntentRequest** | [**OrderIntentRequest**](OrderIntentRequest.md) |  | 

### Return type

[**OrderSubmissionResponse**](OrderSubmissionResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

