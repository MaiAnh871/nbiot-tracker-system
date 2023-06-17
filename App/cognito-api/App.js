import { StatusBar } from 'expo-status-bar';
import { StyleSheet, Text, View, Button } from 'react-native';

import { Amplify, Auth } from 'aws-amplify';
import awsmobile from './src/aws-exports';
import { withAuthenticator } from 'aws-amplify-react-native'

Amplify.configure({
  ...awsmobile,
  Analytics: {
    disabled: true,
  },
}
  
);

function App() {

  async function signOut() {
    try {
      await Auth.signOut();
    }
    catch (error) {
      console.log(error)
    }
  }
  return (
    <View style={styles.container}>
      <Text>Hi Everyone</Text>
      <Button title= 'Sign Out' onPress={signOut} />
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

export default withAuthenticator(App);
