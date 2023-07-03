import { useState, useEffect, useContext } from 'react';
import { View, Text, StyleSheet, Button } from 'react-native';
import { BarCodeScanner } from 'expo-barcode-scanner';
import { DevicesContext } from '../../store/devices-context';
import { useSelector } from 'react-redux';
import { API, graphqlOperation } from 'aws-amplify';
import * as mutations from '../../graphql/mutations';
import * as queries from '../../graphql/queries';
import { listDevices } from '../../graphql/queries';

export default function RegisterDeviceScreen() {
  const devicesCtx = useContext(DevicesContext);
  const [ hasPermission, setHasPermission ] = useState(null);
  const [ scanned, setScanned ] = useState(false);
  const reduxUsername = useSelector(state => state.username);
  console.log(reduxUsername);

  // const outputDevice = API.graphql({
  //   query: mutations.createDevice,
  //   variables: { input: deviceDetails }
  // })
  // console.log(outputDevice);

  // const outputListDevice = API.graphql({
  //   query: queries.listDevices
  // })

  // console.log(outputListDevice);
  
  const outputDeviceList = API.graphql({
    query: queries.listDevices
  }).then((result) => {
    console.log(result);
  }).catch((error) => {
    console.log(error);
  });
  //console.log(outputDeviceList);

  // useEffect(() => {
  //   fetchDevices();
  // }, []);

  // async function fetchDevices() {
  //   try {
  //     const deviceData = await API.graphql(graphqlOperation(listDevices));
  //     const device = deviceData.data.listDevices.items;
  //   } catch (error) {
  //     console.log(error);
  //   }
  // }

  useEffect(() => {
    const getBarCodeScannerPermissions = async () => {
      const { status } = await BarCodeScanner.requestPermissionsAsync();
      setHasPermission(status === 'granted');
    };

    getBarCodeScannerPermissions();
  }, []);

  const handleBarCodeScanned = ({ type, data }) => {
    setScanned(true);
    devicesCtx.addDevice({deviceId: `${ data }`});
    const outputDevice = API.graphql({
      query: mutations.createDevice,
      variables: { input: {
        deviceIMEI: `${ data }`
      }}
    }).then((result) => {
      console.log(result);
    })
    .catch((error) => {
      console.error(error);
    });
    alert(`Bar code with type ${ type } and data ${ data } has been scanned!`);
  };

  if (hasPermission === null) {
    return <Text>Requesting for camera permission</Text>;
  }
  if (hasPermission === false) {
    return <Text>No access to camera</Text>;
  }

  return (
    <View style={ styles.container }>
        <BarCodeScanner
            onBarCodeScanned={ scanned ? undefined : handleBarCodeScanned }
            style={ StyleSheet.absoluteFillObject }
        />
        { scanned && <Button title={'Tap to Scan Again'} onPress={() => setScanned(false)} />}
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
      flex: 1,
      flexDirection: 'column',
      justifyContent: 'center'
  }
})