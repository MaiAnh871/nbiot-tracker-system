var aws = require('aws-sdk');
var ddb = new aws.DynamoDB();

async function handler(event, context) {
  let date = new Date();

  if (event.request.userAttributes.sub) {
    let params = {
      Item: {
        'id': { S: event.request.userAttributes.sub },
        '__typename': { S: 'User' },
        'username': { S: event.userName },
        'email': { S: event.request.userAttributes.email },
        'name': { S: event.request.userAttributes.name },        
        'createdAt': { S: date.toISOString() },
        'updatedAt': { S: date.toISOString() },
      },
      TableName: process.env.USERTABLE
    }

    try {
      await ddb.putItem(params).promise()
      console.log("Put Item Successfully");
    }
    catch (err) {
      console.log("Put Item Error", err);
    }

    console.log("Success: Everything executed correctly");
    context.done(null, event)
  }
  else {
    console.log("Error: Nothing was written to DynamoDB")
    context.done(null, event)
  }
};

exports.handler = handler