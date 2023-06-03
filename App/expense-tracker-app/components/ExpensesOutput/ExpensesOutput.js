import { View } from "react-native";

import ExpensesSummary from "./ExpensesSummary";
import ExpensesList from "./ExpensesList";

const DUMMY_EXPENSES = [
    {
        id: 'e1',
        description: 'A pair of shoes',
        amount: 59.99,
        date: new Date('2023-05-31')
    },
    {
        id: 'e2',
        description: 'A pair of trousers',
        amount: 89.29,
        date: new Date('2023-06-01')
    },
    {
        id: 'e3',
        description: 'Some bananas',
        amount: 5.69,
        date: new Date('2023-06-02')
    },
    {
        id: 'e4',
        description: 'A book',
        amount: 14.49,
        date: new Date('2023-06-03')
    },
    {
        id: 'e5',
        description: 'A dress',
        amount: 34.89,
        date: new Date('2023-06-03')
    },
]

function ExpensesOutput({ expenses, expensesPeriod }) {
    return (
        <View>
            <ExpensesSummary expenses={DUMMY_EXPENSES} periodName={expensesPeriod} />
            <ExpensesList />
        </View>
    );
}

export default ExpensesOutput;