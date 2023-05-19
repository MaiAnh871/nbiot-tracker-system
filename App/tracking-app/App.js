import { StatusBar } from 'expo-status-bar';
import { StyleSheet, Text, View } from 'react-native';
import { Amplify, Auth } from 'aws-amplify';
import awsconfig from './src/aws-exports';

Amplify.configure(awsconfig);

/* Sign Up */
async function signUp() {
  try {
    const { user } = await Auth.signUp({
      username,
      password,
      attributes: {
        email,          // optional
        phone_number,   // optional - E.164 number convention
        // other custom attributes 
      },
      autoSignIn: { // optional - enables auto sign in after user is confirmed
        enabled: true,
      }
    });
    console.log(user);
  } catch (error) {
    console.log('Error signing up:', error);
  }
}

/* Re-send sign up confirmation code */
async function resendConfirmationCode() {
  try {
    await Auth.resendSignUp(username);
    console.log('Code resent successfully');
  } catch (err) {
    console.log('Error resending code: ', err);
  }
}

/* Confirm sign up */
async function confirmSignUp() {
  try {
    await Auth.confirmSignUp(username, code);
  } catch (error) {
    console.log('error confirming sign up', error);
  }
}

/* Auto sign in after sign up */
function listenToAutoSignInEvent() {
  Hub.listen('auth', ({ payload }) => {
    const { event } = payload;
    if (event === 'autoSignIn') {
      const user = payload.data;
      // assign user
    } else if (event === 'autoSignIn_failure') {
      // redirect to sign in page
    }
  })
}

/* Sign in */
async function signIn() {
  try {
    const user = await Auth.signIn(username, password);
  } catch (error) {
    console.log('Error signing in', error);
  }
}


export default function App() {
  return (
    <View style={styles.container}>
      <Text>Open up App.js to start working on your app!</Text>
      <StatusBar style="auto" />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#fff',
    alignItems: 'center',
    justifyContent: 'center',
  },
});
