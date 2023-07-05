const ADD_USER = "ADD_USER";
const ADD_DEVICE_ID = "ADD_DEVICE_ID"

export const addUser = (username) => ({
    type: ADD_USER,
    payload: username
})

export const addDeviceID = (deviceID) => ({
    type: ADD_DEVICE_ID,
    payload: deviceID
})

// Initial state
const initialState = {
    messages: [""]
}

const rootReducer = (state=initialState, action) => {
    switch (action.type) {
        case ADD_USER:
            return {
                ...state,
                username: action.payload
            };

        case ADD_DEVICE_ID:
            return {
                ...state,
                deviceID: action.payload
            }

        default: 
            return state
    }
}

export default rootReducer;
