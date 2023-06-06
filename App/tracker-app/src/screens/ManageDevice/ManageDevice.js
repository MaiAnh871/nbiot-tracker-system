import { useContext } from 'react';
import { StyleSheet, View, Text } from 'react-native';

import { DevicesContext } from '../../store/devices-context';
import IconButton from '../../components/UI/IconButton';
import { GlobalStyles } from '../../constants/styles';

function ManageDeviceScreen({ route, navigation }) {
    const devicesCtx = useContext(DevicesContext);
    const manageDeviceId = route.params?.routeDeviceId;
    const manageId = route.params?.routeId;

    function deleteDeviceHandler() {
        devicesCtx.deleteDevice(manageId);
        navigation.goBack();
    }

    return (
        <View style={styles.container}>
            <Text>Manage Device Screen</Text>
            <Text>{manageDeviceId}</Text>
            <Text>{manageId}</Text>
            <View style={styles.deleteContainer}>
                <IconButton
                    icon="trash"
                    color="white"
                    size = {36}
                    onPress={deleteDeviceHandler}
                />
            </View>
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
        padding: 24,
        backgroundColor: GlobalStyles.colors.tomato
    }
})