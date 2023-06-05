import { Pressable, StyleSheet, Text, View } from "react-native";
import { useNavigation } from '@react-navigation/native';

function Device({ id, deviceId }) {
    const navigation = useNavigation();

    function devicePressHandler() {
        navigation.navigate
    }
}