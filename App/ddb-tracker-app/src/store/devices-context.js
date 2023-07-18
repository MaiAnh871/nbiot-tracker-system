import { createContext, useEffect, useReducer } from 'react';
import { API, graphqlOperation } from 'aws-amplify';
import { listDevices } from '../graphql/queries';
import * as queries from '../graphql/queries';

export const DevicesContext = createContext({
    devices: [],
    addDevice: ({ deviceId }) => {},
    deleteDevice: ( id ) => {},
});

function devicesReducer(state, action) {
    switch (action.kind) {
        case 'INIT':
            return [...action.payload];

        case 'ADD':
            const id = new Date().toString() + Math.random().toString();
            return [{ ...action.payload, id: id }, ...state]

        case 'DELETE':
            return state.filter((device) => device.id !== action.payload);

        default:
            return state;
    }
}

function DevicesContextProvider({ children }) {
    const [ devicesState, dispatch ] = useReducer(devicesReducer, []);

    useEffect(() => {
        API.graphql({ query: queries.listDevices })
            .then((result) => {
                const devices = result.data.listDevices.items;
                console.log(devices); // This will log an array of device objects)
                dispatch({ kind: 'INIT', payload: result.data?.listDevices?.items });
                if (devices && devices.length > 0) {
                    const deviceIMEIs = devices.map((device) => device.deviceIMEI);
                    console.log(deviceIMEIs); // This will log an array of deviceIMEI values
                } else {
                    console.log('No devices found.');
                }
            })
            .catch((error) => {
                console.log(error);
            });
    }, []);

    function addDevice(deviceData) {
        dispatch({ kind: 'ADD', payload: deviceData });
    }

    function deleteDevice(id) {
        dispatch({ kind: 'DELETE', payload: id });
    }

    const value = {
        devices: devicesState,
        addDevice: addDevice,
        deleteDevice: deleteDevice,
    };

    return (
        <DevicesContext.Provider value={ value }>
            { children }
        </DevicesContext.Provider>
    )
}

export default DevicesContextProvider;