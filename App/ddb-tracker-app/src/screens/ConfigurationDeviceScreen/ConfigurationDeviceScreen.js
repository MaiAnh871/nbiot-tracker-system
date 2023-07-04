import React, { useState } from 'react';
import { SafeAreaView, StyleSheet, TextInput, Button } from 'react-native';

const ConfigurationDeviceScreen = () => {
  const [ inputValue, setInputValue ] = useState('');

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
    <SafeAreaView>
      <TextInput
        style={ styles.input }
        value={ inputValue }
        onChangeText={ handleInputChange }
        placeholder="Enter a value"
      />
      <Button title="OK" onPress={ handleOKPress } />
    </SafeAreaView>
  );
};

const styles = StyleSheet.create({
  input: {
    height: 40,
    margin: 12,
    borderWidth: 1,
    padding: 10,
  },
});

export default ConfigurationDeviceScreen;