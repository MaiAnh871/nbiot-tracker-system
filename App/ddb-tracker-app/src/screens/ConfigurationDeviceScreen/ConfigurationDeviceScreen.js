import React, { useState } from 'react';
import { View, StyleSheet, TextInput, TouchableOpacity, Text } from 'react-native';
import { GlobalStyles } from '../../constants/styles';

const ConfigurationDeviceScreen = () => {
  const [inputValue, setInputValue] = useState('');

  const handleInputChange = (text) => {
    setInputValue(text);
  };

  const handleOKPress = () => {
    valueHandler(inputValue);
    setInputValue('');
  };

  const valueHandler = (inputValue) => {
    console.log(inputValue);
  } 

  return (
    <View style={styles.container}>
      <Text style={styles.title}>Enter the Desired Message Period Value</Text>
      <TextInput
        style={styles.input}
        value={inputValue}
        onChangeText={handleInputChange}
        placeholder="e.g. 1 hour"
      />
      <TouchableOpacity style={styles.button} onPress={handleOKPress}>
        <Text style={styles.buttonText}>OK</Text>
      </TouchableOpacity>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'flex-start',
    paddingTop: 50,
    backgroundColor: '#F5FCFF',
  },
  title: {
    fontSize: 20,
    fontWeight: 'bold',
    textAlign: 'center',
    marginBottom: 20,
  },
  input: {
    height: 40,
    width: '80%',
    borderWidth: 1,
    borderRadius: 5,
    paddingHorizontal: 10,
    marginBottom: 20,
  },
  button: {
    backgroundColor: GlobalStyles.colors.tomato,
    borderRadius: 5,
    paddingVertical: 10,
    paddingHorizontal: 20,
  },
  buttonText: {
    color: '#FFFFFF',
    fontSize: 16,
    fontWeight: 'bold',
  },
});

export default ConfigurationDeviceScreen;