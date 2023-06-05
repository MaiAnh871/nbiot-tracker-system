import { FlatList, Text } from "react-native";

function renderDeviceItem(itemData) {
    return (
        <Text>
            {itemData.item.deviceId}
        </Text>
    );
}

function DevicesList({ devices }) {
    return (
        <FlatList
            data={devices}
            renderItem={renderDeviceItem}
            keyExtractor={(item) => item.id}
        />
    )
}

export default DevicesList;