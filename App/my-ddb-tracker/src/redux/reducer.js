const ADD_USER = "ADD_USER";

export const addUser = (username) => ({
    type: ADD_USER,
    payload: username
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

        default: 
            return state
    }
}

export default rootReducer;
