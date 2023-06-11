import { StatusBar } from 'expo-status-bar';
import { SafeAreaView, StyleSheet, Text, View } from 'react-native';
import Navigation from './src/navigation';
import { Amplify } from 'aws-amplify';
import awsconfig from './src/aws-exports';
import MyMap from './src/components/MyMap';
import Map from './src/components/Map';

Amplify.configure(awsconfig);

const App = () => {
  return (
    // <SafeAreaView style={styles.root}>
    //   <Navigation />
    // </SafeAreaView>
    // <MyMap />
    <MyMap />
  );
};

const styles = StyleSheet.create({
  root: {
    flex: 1,
    backgroundColor: '#F9FBFC',
  },
});

export default App;