import React from 'react';
import {View, Text, StyleSheet } from 'react-native';
import {Auth} from 'aws-amplify';
import { useRoute, useNavigation } from '@react-navigation/native';

const HomeScreen = () => {
  const route = useRoute();
  const { scannedData } = route.params || {};
  const navigation = useNavigation();

  const handleDataClick = (deviceId) => {
    navigation.navigate(`device_${deviceId}`);
  };

  return (
    <View style={styles.container}>
      {/* Render the scannedData if available */}
      {scannedData && (
        <View>
          <Text>Scanned Data:</Text>
          <Text>{JSON.stringify(scannedData)}</Text>
        </View>
      )}
      {/* ... rest of the HomeScreen component */}
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
      flex: 1,
      flexDirection: 'column',
      justifyContent: 'center'
  }
})

export default HomeScreen;