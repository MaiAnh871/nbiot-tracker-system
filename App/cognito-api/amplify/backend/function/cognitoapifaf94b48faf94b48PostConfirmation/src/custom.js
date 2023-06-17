import { DynamoDB } from 'aws-sdk';
var ddb = new DynamoDB();

/**
 * @type {import('@types/aws-lambda').APIGatewayProxyHandler}
 */
export async function handler(event, context) {
  // insert code to be executed by your lambda trigger
  let date = new Date()

  if (event.request.userAttributes.sub) {
    let params = {
      Item: {
          'id': {S: event.request.userAttributes.sub},
          '__typename': {S: 'User'},
          'name': {S: event.request.userAttributes.name},
          'email': {S: event.request.userAttributes.email},
          'createdAt': {S: date.toISOString()},
          'updatedAt': {S: date.toISOString()},
      },
      TableName: process.env.USERTABLE
    };

    // Call DynamoDB
    try {
        await ddb.putItem(params).promise()
        console.log("Success");
    } catch (err) {
        console.log("Error", err);
    }

    console.log("Success: Everything executed correctly");
    context.done(null, event);   
  }
  else {
    // Nothing to do, the user's email ID is unknown
    console.log("Error: Nothing was written to DynamoDB");
    context.done(null, event);
  }
  return event;
}
