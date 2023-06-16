var aws = require('aws-sdk');
var ddb = new aws.DynamoDB();

exports.handler = async (event, context) => {
  let date = new Date();

  if (event.request.userAttributes.sub) {

  }
  else {
    console.log("Error: Nothing was written to DynamoDB")
  }
}