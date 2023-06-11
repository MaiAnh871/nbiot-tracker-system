import { createContext, useReducer } from 'react';


const DUMMY_DEVICES = [
    {
        id: 'd1',
        deviceId: '861536030196001'
    },
    {
        id: 'd2',
        deviceId: '861536030196019'
    }, 
    {
        id: 'd3',
        deviceId: '868754048214137'
    }, 
    {
        id: 'd4',
        deviceId: '868754048214129'
    }, 
    {
        id: 'd5',
        deviceId: '868754048214265'
    }, 
    {
        id: 'd6',
        deviceId: '868754048214365'
    }, 
    {
        id: 'd7',
        deviceId: '868754048212565'
    }, 
    {
        id: 'd8',
        deviceId: '868754048214365'
    }, 
    {
        id: 'd9',
        deviceId: '868754048212565'
    }, 
];

export const DevicesContext = createContext({
    devices: [],
    addDevice: ({ deviceId }) => {},
    deleteDevice: ( id ) => {},
});

function devicesReducer(state, action) {
    switch (action.kind) {
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
    const [devicesState, dispatch] = useReducer(devicesReducer, DUMMY_DEVICES);

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
        <DevicesContext.Provider value={value}>
            {children}
        </DevicesContext.Provider>
    )
}

export default DevicesContextProvider;
