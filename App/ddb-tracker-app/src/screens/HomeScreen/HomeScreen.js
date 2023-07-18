import React from 'react';
import { useContext } from 'react';
import DevicesOutput from '../../components/DeviceOutput/DevicesOutput';
import { DevicesContext } from '../../store/devices-context';
import { API, graphqlOperation } from 'aws-amplify';
import * as queries from '../../graphql/queries';
import * as mutations from '../../graphql/mutations';

import { listDevices } from '../../graphql/queries';

function HomeScreen() {
  const devicesCtx = useContext(DevicesContext);

  return (
    <DevicesOutput devices={ devicesCtx.devices }/>
  ) 
}

export default HomeScreen;