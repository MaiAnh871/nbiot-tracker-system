import React from 'react';
import { TouchableOpacity, Text, StyleSheet } from 'react-native';

const HeaderButton = ({ title, onPress }) => {
  return (
    <TouchableOpacity onPress={ onPress } style={styles.button}>
      <Text style={ styles.text }>{ title }</Text>
    </TouchableOpacity>
  );
};

const styles = StyleSheet.create({
  button: {
    paddingHorizontal: 15,
    paddingVertical: 5,
  },
  text: {
    fontSize: 16,
    color: '#007AFF',
  },
});

export default HeaderButton;