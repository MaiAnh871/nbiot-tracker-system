import { useContext, useLayoutEffect } from 'react';
import { StyleSheet, View, Text } from 'react-native';

import { DevicesContext } from '../../store/device-context';
import IconButton from '../../components/IconButton/IconButton';
import { GlobalStyles } from '../../constants/styles';

function ManageDeviceScreen({ route }) {
    const manageDeviceId = route.params?.trackerId;

    function deleteDeviceHandler() {

    }

    return (
        <View style={styles.container}>
            <Text>Manage Device Screen</Text>
            <Text>{manageDeviceId}</Text>
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