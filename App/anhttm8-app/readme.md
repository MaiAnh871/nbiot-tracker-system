# Step-by-step 
## Init my project
```bash
npx create-expo-app anhttm8-app
amplify configure
amplify init
npm install aws-amplify amazon-cognito-identity-js @react-native-community/netinfo @react-native-async-storage/async-storage
```

## Create IAM policies for AWS IoT 
`real-time-tracker-iot-policy`
```json
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": "*",
      "Resource": "*"
    }
  ]
}
```

## Create an AWS Cognito User Pool.
    - Configure sign-in experience
        - Cognito user pool sign-in options: User name
    - Configure security requirements
        - MFA enforcement: No MFA
    - Configure sign-up experience
    - Configure message delivery
        - Email provider: Send email with Cognito
    - Integrate your app
        - User pool name: `anhttm8-app-user-pool`
        - App client name: `anhttm8-app-client`
```
User pool ID: ap-northeast-2_gYCGsRuof
Client ID: 5ed3h3ht13utpu8ek60ttcd43q
```

## Create Cognito Identity Pool.
    - Create identity pool
        - Identity pool name: `anhttm8-app-identity-pool`
        - Enable access to unauthenticated identities: `true`
        - Authentication providers - Cognito: 
            - User Pool ID: `ap-northeast-2_gYCGsRuof`
            - App client id: `5ed3h3ht13utpu8ek60ttcd43q`
    - Set up permission
        - Role Name: `Cognito_anhttm8appidentitypoolAuth_Role` and `Cognito_anhttm8appidentitypoolUnauth_Role`
        - Modify policy document:
        ```
        {
            "Version": "2012-10-17",
            "Statement": [
                {
                    "Effect": "Allow",
                    "Action": [
                        "mobileanalytics:PutEvents",
                        "cognito-sync:*",
                        "cognito-identity:*"
                    ],
                    "Resource": [
                        "*"
                    ]
                },
                {
                    "Effect": "Allow",
                    "Action": [
                        "iot:*"
                    ],
                    "Resource": [
                        "*"
                    ]
                }
            ]
        }
        ```
    - My Identity pool ID: `ap-northeast-2:26b69494-e2fd-4660-a6ab-54318ee698e9`

## Attach your policy to your Amazon Cognito Identity
You can retrieve the Cognito Identity Id of a logged in user with Auth Module:
```js
Auth.currentCredentials().then((info) => {
  const cognitoIdentityId = info.identityId;
});
```
Then, you need to send your Cognito Identity Id to the AWS backend and attach policy:
```bash
aws iot attach-policy --policy-name 'real-time-tracker-iot-policy' --target 'ap-northeast-2:26b69494-e2fd-4660-a6ab-54318ee698e9'
```

## Allow the Amazon Cognito Authenticated Role to access IoT Services
**Cloudformation**. Stack name `amplify-anhttm8app-dev-233255`. **Resources**. `amplify-anhttm8app-dev-233255-authRole`. **Attach Policies**. Search `AWSIoTDataAccess` and `AWSIoTConfigAccess`, select them and tap on **Attach policy**.

## Setup Amplify Credentials
In src\aws-exports.js:
```js
const awsmobile = {
    "aws_project_region": "ap-northeast-2",
    "aws_cognito_identity_pool_id": "ap-northeast-2:26b69494-e2fd-4660-a6ab-54318ee698e9",
    "aws_cognito_region": "ap-northeast-2",
    "aws_user_pools_id": "ap-northeast-2_gYCGsRuof",
    "aws_user_pools_web_client_id": "5ed3h3ht13utpu8ek60ttcd43q",
};

export default awsmobile;
```

## Setup Amplify Authentication
**Amplify**. `anhttm8app`. **Set up Amplify Studio**. **Enable Amplify Studio.**
After enable successfully, **Launch Studio**. **Enable Authentication**. **Reuse existing Amazon Cognito resources**.
- Select a Cognito User Pool: `anhttm8-app-user-pool	ap-northeast-2_gYCGsRuof`
Select an app client: `anhttm8-app-client	5ed3h3ht13utpu8ek60ttcd43q`
- Select an Identity Pool: `anhttm8-app-identity-pool	ap-northeast-2:26b69494-e2fd-4660-a6ab-54318ee698e9`
**Import**.

## Config done. Run app.
```bash
npx expo start
```






