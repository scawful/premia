# AccountAPI

All URIs are relative to *https://api.premia.local*

Method | HTTP request | Description
------------- | ------------- | -------------
[**getAccountScreen**](AccountAPI.md#getaccountscreen) | **GET** /v1/screens/account | Load the account screen payload.


# **getAccountScreen**
```swift
    open class func getAccountScreen(completion: @escaping (_ data: AccountScreenResponse?, _ error: Error?) -> Void)
```

Load the account screen payload.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated


// Load the account screen payload.
AccountAPI.getAccountScreen() { (response, error) in
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

[**AccountScreenResponse**](AccountScreenResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

