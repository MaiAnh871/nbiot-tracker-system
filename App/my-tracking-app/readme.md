Follow this blog https://medium.com/serverlessguru/serverless-real-time-reactjs-app-aws-iot-mqtt-17d023954045 for PubSub first, then follow this video https://www.youtube.com/watch?v=EvzLRCDe6VI&ab_channel=notJust%E2%80%A4dev for Authentication.

Notice some important things:
1. In Creating User Pool: 
- Step 1: Configure sign-in experience:
    - `Cognito user pool sign-in options`: User name
    DON'T TICK ANYTHING IN `User name requirements`.
- Step 2: Configure security requirements
    - `Multi-factor authentication`: No MFA
- Step 3: Configure sign-up experience
    - In `Require attributes`: Don't tickk anything.
- Step 4: Configure message delivery
    - `Email provider`: Send email with Cognito
- Step 5: Integrate your app: Enter your `User pool name` and `App client name`
**Things I didn't mention leave it as default.**
2. After run successfully PubSub, navigate to `AWS Amplify` and launch `Amplify Studio`.
- Add Authentication using existing User Pool, App Client and Identity Pool.

