import { useContext, useLayoutEffect } from 'react';
import { StyleSheet, View, Text } from 'react-native';

import { DevicesContext } from '../../store/device-context';

function ManageDeviceScreen({ route }) {
    const manageDeviceId = route.params?.trackerId;

    return (
        <View>
            <Text>ManageDevice Screen</Text>
            <Text>{manageDeviceId}</Text>
        </View>
    );
  };
  
export default ManageDeviceScreen;