import { View } from "react-native";

import DevicesList from "./DevicesList";

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
];

function DevicesOutput({ devices }) {
    return (
        <View>
            <DevicesList />
        </View>
    );
}

export default DevicesOutput;