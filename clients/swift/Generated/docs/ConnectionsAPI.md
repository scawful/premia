# ConnectionsAPI

All URIs are relative to *https://api.premia.local*

Method | HTTP request | Description
------------- | ------------- | -------------
[**completePlaidLink**](ConnectionsAPI.md#completeplaidlink) | **POST** /v1/connections/plaid/link-complete | Complete the Plaid link flow.
[**completeSchwabOAuth**](ConnectionsAPI.md#completeschwaboauth) | **POST** /v1/connections/schwab/oauth/complete | Complete the Schwab OAuth flow.
[**createPlaidLinkToken**](ConnectionsAPI.md#createplaidlinktoken) | **POST** /v1/connections/plaid/link-token | Create a Plaid link token.
[**startSchwabOAuth**](ConnectionsAPI.md#startschwaboauth) | **POST** /v1/connections/schwab/oauth/start | Start the Schwab OAuth flow.


# **completePlaidLink**
```swift
    open class func completePlaidLink(completePlaidLinkRequest: CompletePlaidLinkRequest, completion: @escaping (_ data: ConnectionSummaryResponse?, _ error: Error?) -> Void)
```

Complete the Plaid link flow.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let completePlaidLinkRequest = CompletePlaidLinkRequest(publicToken: "publicToken_example", institutionId: "institutionId_example") // CompletePlaidLinkRequest | 

// Complete the Plaid link flow.
ConnectionsAPI.completePlaidLink(completePlaidLinkRequest: completePlaidLinkRequest) { (response, error) in
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
 **completePlaidLinkRequest** | [**CompletePlaidLinkRequest**](CompletePlaidLinkRequest.md) |  | 

### Return type

[**ConnectionSummaryResponse**](ConnectionSummaryResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **completeSchwabOAuth**
```swift
    open class func completeSchwabOAuth(completeSchwabOAuthRequest: CompleteSchwabOAuthRequest, completion: @escaping (_ data: ConnectionSummaryResponse?, _ error: Error?) -> Void)
```

Complete the Schwab OAuth flow.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let completeSchwabOAuthRequest = CompleteSchwabOAuthRequest(callback: "callback_example", state: "state_example") // CompleteSchwabOAuthRequest | 

// Complete the Schwab OAuth flow.
ConnectionsAPI.completeSchwabOAuth(completeSchwabOAuthRequest: completeSchwabOAuthRequest) { (response, error) in
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
 **completeSchwabOAuthRequest** | [**CompleteSchwabOAuthRequest**](CompleteSchwabOAuthRequest.md) |  | 

### Return type

[**ConnectionSummaryResponse**](ConnectionSummaryResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **createPlaidLinkToken**
```swift
    open class func createPlaidLinkToken(createPlaidLinkTokenRequest: CreatePlaidLinkTokenRequest? = nil, completion: @escaping (_ data: PlaidLinkTokenResponse?, _ error: Error?) -> Void)
```

Create a Plaid link token.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let createPlaidLinkTokenRequest = CreatePlaidLinkTokenRequest(userId: "userId_example", redirectUri: "redirectUri_example") // CreatePlaidLinkTokenRequest |  (optional)

// Create a Plaid link token.
ConnectionsAPI.createPlaidLinkToken(createPlaidLinkTokenRequest: createPlaidLinkTokenRequest) { (response, error) in
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
 **createPlaidLinkTokenRequest** | [**CreatePlaidLinkTokenRequest**](CreatePlaidLinkTokenRequest.md) |  | [optional] 

### Return type

[**PlaidLinkTokenResponse**](PlaidLinkTokenResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **startSchwabOAuth**
```swift
    open class func startSchwabOAuth(startSchwabOAuthRequest: StartSchwabOAuthRequest? = nil, completion: @escaping (_ data: SchwabOAuthStartResponse?, _ error: Error?) -> Void)
```

Start the Schwab OAuth flow.

### Example
```swift
// The following code samples are still beta. For any issue, please report via http://github.com/OpenAPITools/openapi-generator/issues/new
import PremiaAPIClientGenerated

let startSchwabOAuthRequest = StartSchwabOAuthRequest(redirectUri: "redirectUri_example", clientPlatform: "clientPlatform_example") // StartSchwabOAuthRequest |  (optional)

// Start the Schwab OAuth flow.
ConnectionsAPI.startSchwabOAuth(startSchwabOAuthRequest: startSchwabOAuthRequest) { (response, error) in
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
 **startSchwabOAuthRequest** | [**StartSchwabOAuthRequest**](StartSchwabOAuthRequest.md) |  | [optional] 

### Return type

[**SchwabOAuthStartResponse**](SchwabOAuthStartResponse.md)

### Authorization

[bearerAuth](../README.md#bearerAuth)

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

