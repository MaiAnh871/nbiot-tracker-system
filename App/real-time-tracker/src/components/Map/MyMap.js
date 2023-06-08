import { StatusBar } from 'expo-status-bar';
import { StyleSheet, Text, View } from 'react-native';
import { Amplify, PubSub } from 'aws-amplify';
import { AWSIoTProvider } from '@aws-amplify/pubsub';
import awsmobile from '../../aws-exports';
import { useEffect, useState } from 'react';

Amplify.configure(awsmobile);

// Apply plugin with configuration
Amplify.addPluggable(
  new AWSIoTProvider({
    aws_pubsub_region: 'ap-northeast-2',
    aws_pubsub_endpoint:
      'wss://a2ht7rbdkt6040-ats.iot.ap-northeast-2.amazonaws.com/mqtt'
  })
);

function MyMap() {
  const [message, setMessage] = useState('');

  useEffect(() => {
    let subscription;
    Amplify.PubSub.subscribe('tracker/data').subscribe({    
      next: data => {        
        console.log('Message received:', data.value.message);
        setMessage(data.value.message);
      },
      error: error => console.error(error),    
      close: () => console.log('Done'),  
    });
    return () => {
      if (subscription) {
        subscription.unsubscribe();
      }
    };
  }, []);

  return (
    <View style={styles.container}>
      {
        message && (
          <View>
            <Text>Hi </Text>
            <Text>Latitude: {message.lat}</Text>
            <Text>Longitude: {message.long}</Text>
          </View>
        )
      }
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

export default MyMap;
