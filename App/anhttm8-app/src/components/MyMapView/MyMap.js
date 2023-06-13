import { StatusBar } from 'expo-status-bar';
import { StyleSheet, Text, View, AppState } from 'react-native';
import { Amplify, PubSub, Hub } from 'aws-amplify';
import { AWSIoTProvider, CONNECTION_STATE_CHANGE, ConnectionState } from '@aws-amplify/pubsub';
import awsmobile from '../../aws-exports';
import { useEffect } from 'react';

// Amplify.Logger.LOG_LEVEL = 'DEBUG';
Amplify.configure(awsmobile);

// Apply plugin with configuration
Amplify.addPluggable(
  new AWSIoTProvider({
    aws_pubsub_region: 'ap-northeast-2',
    aws_pubsub_endpoint:
      'wss://a2ht7rbdkt6040-ats.iot.ap-northeast-2.amazonaws.com/mqtt'
  })
);

PubSub.subscribe('tracker/data').subscribe({
  next: data => console.log('Message received', data),
  error: error => console.error(error),
  complete: () => console.log('Done'),
});

Hub.listen('pubsub', (data) => {
  const { payload } = data;
  if (payload.event === CONNECTION_STATE_CHANGE) {
      const connectionState = payload.data.connectionState;
      console.log(connectionState);
  }
});


export default function MyMap() {
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
