import { View, StyleSheet } from "react-native";

import DevicesList from "./DevicesList";
import { GlobalStyles } from "../../constants/styles";

function DevicesOutput({ devices }) {
    return (
        <View style={styles.container}>
            {/*<DevicesList devices={DUMMY_DEVICES} />*/}
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