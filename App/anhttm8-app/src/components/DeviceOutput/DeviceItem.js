import { Pressable, StyleSheet, Text, View } from "react-native";
import { useNavigation } from '@react-navigation/native';
import { GlobalStyles } from "../../constants/styles";

function DeviceItem({ id, deviceId }) {
    const navigation = useNavigation();

    function devicePressHandler() {
        navigation.navigate('ManageDeviceScreen', {
            routeDeviceId: deviceId,
            routeId: id
        });
    }
    
    return (
        <Pressable
            onPress={devicePressHandler}
            style={({ pressed }) => pressed && styles.pressed}
        >
            <View style={styles.deviceItem}>
                <View>
                    <Text style={[styles.textBase, styles.description]}>
                        SEE MAP OF 
                    </Text>
                </View>
                <View style={styles.idContainer}>
                    <Text style={styles.id}>
                        {deviceId}
                    </Text>
                </View>
            </View>
        </Pressable>
    );
}

export default DeviceItem;

const styles = StyleSheet.create({
    deviceItem: {
        padding: 12,
        marginVertical: 8,
        backgroundColor: GlobalStyles.colors.primary500,
        flexDirection: 'row',
        justifyContent: 'space-between',
        borderRadius: 6,
        elevation: 3,
        shadowColor: GlobalStyles.colors.gray500,
        shadowRadius: 4,
        shadowOffset: {
            width: 1,
            height: 1
        },
        shadowOpacity: 0.4
    },
    textBase: {
        color: GlobalStyles.colors.primary50,
    },
    description: {
        fontSize: 16,
        marginBottom: 4,
        fontWeight: 'bold'
    },
    idContainer: {
        paddingHorizontal: 12,
        paddingVertical: 4,
        backgroundColor: 'white',
        justifyContent: 'center',
        alignItems: 'center',
        borderRadius: 4,
        minWidth: 80,
    },
    id: {
        color: GlobalStyles.colors.primary500,
        fontWeight: 'bold'
    },
    pressed: {
        opacity: 0.75
    }
});