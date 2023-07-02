const ADD_USER = "ADD_USER";

export const addUser = () => ({
    type: ADD_USER
})

// Initial state
const initialState = {
    messages: ["Test"]
}

const rootReducer = (state=initialState, action) => {
    switch (action.type) {
        case ADD_USER:
            return {
                ...state,
                messages: [...state.messages, "new"]
            }

        default: 
            return state
    }
}

export default rootReducer;
