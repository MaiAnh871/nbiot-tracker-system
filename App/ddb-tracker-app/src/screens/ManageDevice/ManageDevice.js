import { useContext } from 'react';
import { StyleSheet, View, Text } from 'react-native';

import { DevicesContext } from '../../store/devices-context';
import IconButton from '../../components/UI/IconButton';
import { GlobalStyles } from '../../constants/styles';
import MyMapView from '../../components/MyMapView/MyMapView';
import MyMap from '../../components/MyMapView/MyMap';

function ManageDeviceScreen({ route, navigation }) {
    const devicesCtx = useContext(DevicesContext);
    const manageDeviceId = route.params?.routeDeviceId;
    const manageId = route.params?.routeId;

    function configurePressHandler() {
        navigation.navigate('ConfigurationScreen');
    }

    function deleteDeviceHandler() {
        devicesCtx.deleteDevice(manageId);
        navigation.goBack();
    }

    return (
        <View style={ styles.container }>
            {/* <Text>Device ID: {manageDeviceId}</Text> */}
            <IconButton
                color={ GlobalStyles.colors.tomato }
                size={ 24 }
                onPress={ configurePressHandler }
            />
            <MyMapView manageDeviceId={manageDeviceId}/>
            {/* <View style={styles.deleteContainer}>
                <IconButton
                    icon="trash"
                    color="white"
                    size = {36}
                    onPress={deleteDeviceHandler}
                />
            </View> */}
        </View>
    );
};
  
export default ManageDeviceScreen;

const styles = StyleSheet.create({
    deleteContainer: {
        marginTop: 60,
        paddingTop: 8,
        borderTopWidth: 2,
        borderTopColor: "white",
        alignItems: 'center'
    },
    container: {
        flex: 1,
        padding: 0,
        backgroundColor: GlobalStyles.colors.tomato
    }
})