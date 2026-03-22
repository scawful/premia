# BootstrapAPI

All URIs are relative to *https://api.premia.local*

Method | HTTP request | Description
------------- | ------------- | -------------
[**getBootstrap**](BootstrapAPI.md#getbootstrap) | **GET** /v1/bootstrap | Load app bootstrap state.


# **getBootstrap**
```swift
    open class func getBootstrap(completion: @escaping (_ data: BootstrapResponse?, _ error: Error?) -> Void)
```

Load app bootstrap state.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated


// Load app bootstrap state.
BootstrapAPI.getBootstrap() { (response, error) in
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

[**BootstrapResponse**](BootstrapResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

