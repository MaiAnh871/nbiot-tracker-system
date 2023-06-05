import { View } from "react-native";

import DevicesList from "./DeviceList";

function DevicesOutput({ devices }) {
    return (
        <View>
            <DevicesList />
        </View>
    );
}

export default DevicesOutput;