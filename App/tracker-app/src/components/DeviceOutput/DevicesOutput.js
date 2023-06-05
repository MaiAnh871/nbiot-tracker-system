import { View, StyleSheet } from "react-native";

import DevicesList from "./DevicesList";
import { GlobalStyles } from "../../constants/styles";

const DUMMY_DEVICES = [
    {
        id: 'd1',
        deviceId: '861536030196001'
    },
    {
        id: 'd2',
        deviceId: '861536030196019'
    }, 
    {
        id: 'd3',
        deviceId: '868754048214137'
    }, 
    {
        id: 'd4',
        deviceId: '868754048214129'
    }, 
    {
        id: 'd5',
        deviceId: '868754048214265'
    }, 
    {
        id: 'd6',
        deviceId: '868754048214365'
    }, 
    {
        id: 'd7',
        deviceId: '868754048212565'
    }, 
    {
        id: 'd8',
        deviceId: '868754048214365'
    }, 
    {
        id: 'd9',
        deviceId: '868754048212565'
    }, 
];

function DevicesOutput({ devices }) {
    return (
        <View style={styles.container}>
            <DevicesList devices={DUMMY_DEVICES} />
        </View>
    );
}

export default DevicesOutput;

const styles = StyleSheet.create({
    container: {
        paddingHorizontal: 24,
        paddingTop: 24,
        paddingBottom: 0,
        backgroundColor: GlobalStyles.colors.tomato,
        flex: 1,
    }
})