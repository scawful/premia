# StreamingAPI

All URIs are relative to *https://api.premia.local*

Method | HTTP request | Description
------------- | ------------- | -------------
[**streamEvents**](StreamingAPI.md#streamevents) | **GET** /v1/stream/events | Subscribe to server-sent stream events.


# **streamEvents**
```swift
    open class func streamEvents(completion: @escaping (_ data: String?, _ error: Error?) -> Void)
```

Subscribe to server-sent stream events.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated


// Subscribe to server-sent stream events.
StreamingAPI.streamEvents() { (response, error) in
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

**String**

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: text/event-stream, application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

