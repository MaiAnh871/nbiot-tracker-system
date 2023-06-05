import { useContext, useLayoutEffect } from 'react';
import { StyleSheet, View } from 'react-native';

import { DevicesContext } from '../../store/device-context';

const ManageDevice = () => {

    const devicesCtx = useContext(DevicesContext);
  
    return (
      <View style={{flex: 1}}>
        <Text style={{fontSize: 24, alignSelf: 'center'}}>Manage Device</Text>
        <Text
          onPress={signOut}
          style={{
            width: '100%',
            textAlign: 'center',
            color: 'red',
            marginTop: 'auto',
            marginVertical: 20,
            fontSize: 20,
          }}>
          Sign out
        </Text>
      </View>
    );
  };
  
  export default ManageDevice;