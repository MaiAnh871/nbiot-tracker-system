/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const createUser = /* GraphQL */ `
  mutation CreateUser(
    $input: CreateUserInput!
    $condition: ModelUserConditionInput
  ) {
    createUser(input: $input, condition: $condition) {
      id
      username
      email
      name
      devices {
        items {
          id
          deviceIMEI
          createdAt
          updatedAt
          userDevicesId
          owner
        }
        nextToken
      }
      createdAt
      updatedAt
      owner
    }
  }
`;
export const updateUser = /* GraphQL */ `
  mutation UpdateUser(
    $input: UpdateUserInput!
    $condition: ModelUserConditionInput
  ) {
    updateUser(input: $input, condition: $condition) {
      id
      username
      email
      name
      devices {
        items {
          id
          deviceIMEI
          createdAt
          updatedAt
          userDevicesId
          owner
        }
        nextToken
      }
      createdAt
      updatedAt
      owner
    }
  }
`;
export const deleteUser = /* GraphQL */ `
  mutation DeleteUser(
    $input: DeleteUserInput!
    $condition: ModelUserConditionInput
  ) {
    deleteUser(input: $input, condition: $condition) {
      id
      username
      email
      name
      devices {
        items {
          id
          deviceIMEI
          createdAt
          updatedAt
          userDevicesId
          owner
        }
        nextToken
      }
      createdAt
      updatedAt
      owner
    }
  }
`;
export const createDevice = /* GraphQL */ `
  mutation CreateDevice(
    $input: CreateDeviceInput!
    $condition: ModelDeviceConditionInput
  ) {
    createDevice(input: $input, condition: $condition) {
      id
      deviceIMEI
      user {
        id
        username
        email
        name
        devices {
          nextToken
        }
        createdAt
        updatedAt
        owner
      }
      messages {
        items {
          id
          imei
          timestamp
          latitude
          longitude
          speed
          acceX
          acceY
          acceZ
          cellID
          rsrp
          tilt_alert
          wheelie_alert
          overspeed_alert
          createdAt
          updatedAt
          deviceMessagesId
        }
        nextToken
      }
      createdAt
      updatedAt
      userDevicesId
      owner
    }
  }
`;
export const updateDevice = /* GraphQL */ `
  mutation UpdateDevice(
    $input: UpdateDeviceInput!
    $condition: ModelDeviceConditionInput
  ) {
    updateDevice(input: $input, condition: $condition) {
      id
      deviceIMEI
      user {
        id
        username
        email
        name
        devices {
          nextToken
        }
        createdAt
        updatedAt
        owner
      }
      messages {
        items {
          id
          imei
          timestamp
          latitude
          longitude
          speed
          acceX
          acceY
          acceZ
          cellID
          rsrp
          tilt_alert
          wheelie_alert
          overspeed_alert
          createdAt
          updatedAt
          deviceMessagesId
        }
        nextToken
      }
      createdAt
      updatedAt
      userDevicesId
      owner
    }
  }
`;
export const deleteDevice = /* GraphQL */ `
  mutation DeleteDevice(
    $input: DeleteDeviceInput!
    $condition: ModelDeviceConditionInput
  ) {
    deleteDevice(input: $input, condition: $condition) {
      id
      deviceIMEI
      user {
        id
        username
        email
        name
        devices {
          nextToken
        }
        createdAt
        updatedAt
        owner
      }
      messages {
        items {
          id
          imei
          timestamp
          latitude
          longitude
          speed
          acceX
          acceY
          acceZ
          cellID
          rsrp
          tilt_alert
          wheelie_alert
          overspeed_alert
          createdAt
          updatedAt
          deviceMessagesId
        }
        nextToken
      }
      createdAt
      updatedAt
      userDevicesId
      owner
    }
  }
`;
export const createMessage = /* GraphQL */ `
  mutation CreateMessage(
    $input: CreateMessageInput!
    $condition: ModelMessageConditionInput
  ) {
    createMessage(input: $input, condition: $condition) {
      id
      device {
        id
        deviceIMEI
        user {
          id
          username
          email
          name
          createdAt
          updatedAt
          owner
        }
        messages {
          nextToken
        }
        createdAt
        updatedAt
        userDevicesId
        owner
      }
      imei
      timestamp
      latitude
      longitude
      speed
      acceX
      acceY
      acceZ
      cellID
      rsrp
      tilt_alert
      wheelie_alert
      overspeed_alert
      createdAt
      updatedAt
      deviceMessagesId
    }
  }
`;
export const updateMessage = /* GraphQL */ `
  mutation UpdateMessage(
    $input: UpdateMessageInput!
    $condition: ModelMessageConditionInput
  ) {
    updateMessage(input: $input, condition: $condition) {
      id
      device {
        id
        deviceIMEI
        user {
          id
          username
          email
          name
          createdAt
          updatedAt
          owner
        }
        messages {
          nextToken
        }
        createdAt
        updatedAt
        userDevicesId
        owner
      }
      imei
      timestamp
      latitude
      longitude
      speed
      acceX
      acceY
      acceZ
      cellID
      rsrp
      tilt_alert
      wheelie_alert
      overspeed_alert
      createdAt
      updatedAt
      deviceMessagesId
    }
  }
`;
export const deleteMessage = /* GraphQL */ `
  mutation DeleteMessage(
    $input: DeleteMessageInput!
    $condition: ModelMessageConditionInput
  ) {
    deleteMessage(input: $input, condition: $condition) {
      id
      device {
        id
        deviceIMEI
        user {
          id
          username
          email
          name
          createdAt
          updatedAt
          owner
        }
        messages {
          nextToken
        }
        createdAt
        updatedAt
        userDevicesId
        owner
      }
      imei
      timestamp
      latitude
      longitude
      speed
      acceX
      acceY
      acceZ
      cellID
      rsrp
      tilt_alert
      wheelie_alert
      overspeed_alert
      createdAt
      updatedAt
      deviceMessagesId
    }
  }
`;
