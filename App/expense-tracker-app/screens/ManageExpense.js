import { useLayoutEffect } from "react";
import { Text } from "react-native";

function ManageExpense({ route, navigation }) {
    const editedExpenseId = route.param?.expenseId;
    const isEditing = !!editedExpenseId;

    useLayoutEffect(() => {
        navigation.setOption({
            title: isEditing ? 'Edit Expense' : 'Add Expense'
        });
    }, [navigation, isEditing]);
    

    return <Text>ManageExpense Screen</Text>
}

export default ManageExpense;