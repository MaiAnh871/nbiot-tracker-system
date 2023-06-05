import { FlatList, Text } from "react-native";
import DeviceItem from "./DeviceItem";

function renderDeviceItem(itemData) {
    return (
        <DeviceItem {...itemData.item} />
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