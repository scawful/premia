# TradingAPI

All URIs are relative to *https://api.premia.local*

Method | HTTP request | Description
------------- | ------------- | -------------
[**cancelOrder**](TradingAPI.md#cancelorder) | **POST** /v1/orders/{orderId}/cancel | Cancel an existing order.
[**getOpenOrders**](TradingAPI.md#getopenorders) | **GET** /v1/orders/open | List currently open orders.
[**getOrderHistory**](TradingAPI.md#getorderhistory) | **GET** /v1/orders/history | List historical orders.
[**previewOrder**](TradingAPI.md#previeworder) | **POST** /v1/orders/preview | Preview an equity or option order.
[**replaceOrder**](TradingAPI.md#replaceorder) | **POST** /v1/orders/{orderId}/replace | Replace an existing order.
[**submitOrder**](TradingAPI.md#submitorder) | **POST** /v1/orders/submit | Submit an equity or option order.


# **cancelOrder**
```swift
    open class func cancelOrder(orderId: String, orderCancelRequest: OrderCancelRequest? = nil, completion: @escaping (_ data: OrderCancellationResponse?, _ error: Error?) -> Void)
```

Cancel an existing order.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let orderId = "orderId_example" // String | 
let orderCancelRequest = OrderCancelRequest(accountId: "accountId_example", confirmLive: false) // OrderCancelRequest |  (optional)

// Cancel an existing order.
TradingAPI.cancelOrder(orderId: orderId, orderCancelRequest: orderCancelRequest) { (response, error) in
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
 **orderId** | **String** |  | 
 **orderCancelRequest** | [**OrderCancelRequest**](OrderCancelRequest.md) |  | [optional] 

### Return type

[**OrderCancellationResponse**](OrderCancellationResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **getOpenOrders**
```swift
    open class func getOpenOrders(accountId: String? = nil, completion: @escaping (_ data: OrderRecordsResponse?, _ error: Error?) -> Void)
```

List currently open orders.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let accountId = "accountId_example" // String |  (optional)

// List currently open orders.
TradingAPI.getOpenOrders(accountId: accountId) { (response, error) in
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
 **accountId** | **String** |  | [optional] 

### Return type

[**OrderRecordsResponse**](OrderRecordsResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **getOrderHistory**
```swift
    open class func getOrderHistory(accountId: String? = nil, completion: @escaping (_ data: OrderRecordsResponse?, _ error: Error?) -> Void)
```

List historical orders.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let accountId = "accountId_example" // String |  (optional)

// List historical orders.
TradingAPI.getOrderHistory(accountId: accountId) { (response, error) in
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
 **accountId** | **String** |  | [optional] 

### Return type

[**OrderRecordsResponse**](OrderRecordsResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

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

# **replaceOrder**
```swift
    open class func replaceOrder(orderId: String, orderReplaceRequest: OrderReplaceRequest, completion: @escaping (_ data: OrderReplacementResponse?, _ error: Error?) -> Void)
```

Replace an existing order.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let orderId = "orderId_example" // String | 
let orderReplaceRequest = OrderReplaceRequest(accountId: "accountId_example", symbol: "symbol_example", assetType: "assetType_example", instruction: "instruction_example", quantity: "quantity_example", orderType: "orderType_example", limitPrice: "limitPrice_example", duration: "duration_example", session: "session_example", confirmLive: false) // OrderReplaceRequest | 

// Replace an existing order.
TradingAPI.replaceOrder(orderId: orderId, orderReplaceRequest: orderReplaceRequest) { (response, error) in
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
 **orderId** | **String** |  | 
 **orderReplaceRequest** | [**OrderReplaceRequest**](OrderReplaceRequest.md) |  | 

### Return type

[**OrderReplacementResponse**](OrderReplacementResponse.md)

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

