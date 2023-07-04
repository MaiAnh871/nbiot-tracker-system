import React from 'react';
import { useContext } from 'react';
import DevicesOutput from '../../components/DeviceOutput/DevicesOutput';
import { DevicesContext } from '../../store/devices-context';

function HomeScreen() {
  const devicesCtx = useContext(DevicesContext);

  return (
    <DevicesOutput devices={ devicesCtx.devices }/>
  ) 
}

export default HomeScreen;