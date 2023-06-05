import React from 'react';
import {View, Text} from 'react-native';
import {Auth} from 'aws-amplify';
import DevicesOutput from '../../components/DeviceOutput/DevicesOutput';

const HomeScreen = () => {

  return (
    <DevicesOutput />
  );
};

export default HomeScreen;