# HomeAPI

All URIs are relative to *https://api.premia.local*

Method | HTTP request | Description
------------- | ------------- | -------------
[**getHomeScreen**](HomeAPI.md#gethomescreen) | **GET** /v1/screens/home | Load the home screen payload.


# **getHomeScreen**
```swift
    open class func getHomeScreen(accountId: String? = nil, completion: @escaping (_ data: HomeScreenResponse?, _ error: Error?) -> Void)
```

Load the home screen payload.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let accountId = "accountId_example" // String |  (optional)

// Load the home screen payload.
HomeAPI.getHomeScreen(accountId: accountId) { (response, error) in
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

[**HomeScreenResponse**](HomeScreenResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

