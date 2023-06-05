import { Pressable, StyleSheet, Text, View } from "react-native";
import { useNavigation } from '@react-navigation/native';

function DeviceItem({ id, deviceId }) {
    const navigation = useNavigation();

    function devicePressHandler() {
        navigation.navigate
    }
}