/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const onCreateUser = /* GraphQL */ `
  subscription OnCreateUser($filter: ModelSubscriptionUserFilterInput) {
    onCreateUser(filter: $filter) {
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
        }
        nextToken
      }
      createdAt
      updatedAt
    }
  }
`;
export const onUpdateUser = /* GraphQL */ `
  subscription OnUpdateUser($filter: ModelSubscriptionUserFilterInput) {
    onUpdateUser(filter: $filter) {
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
        }
        nextToken
      }
      createdAt
      updatedAt
    }
  }
`;
export const onDeleteUser = /* GraphQL */ `
  subscription OnDeleteUser($filter: ModelSubscriptionUserFilterInput) {
    onDeleteUser(filter: $filter) {
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
        }
        nextToken
      }
      createdAt
      updatedAt
    }
  }
`;
export const onCreateDevice = /* GraphQL */ `
  subscription OnCreateDevice($filter: ModelSubscriptionDeviceFilterInput) {
    onCreateDevice(filter: $filter) {
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
    }
  }
`;
export const onUpdateDevice = /* GraphQL */ `
  subscription OnUpdateDevice($filter: ModelSubscriptionDeviceFilterInput) {
    onUpdateDevice(filter: $filter) {
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
    }
  }
`;
export const onDeleteDevice = /* GraphQL */ `
  subscription OnDeleteDevice($filter: ModelSubscriptionDeviceFilterInput) {
    onDeleteDevice(filter: $filter) {
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
    }
  }
`;
export const onCreateMessage = /* GraphQL */ `
  subscription OnCreateMessage($filter: ModelSubscriptionMessageFilterInput) {
    onCreateMessage(filter: $filter) {
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
        }
        messages {
          nextToken
        }
        createdAt
        updatedAt
        userDevicesId
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
export const onUpdateMessage = /* GraphQL */ `
  subscription OnUpdateMessage($filter: ModelSubscriptionMessageFilterInput) {
    onUpdateMessage(filter: $filter) {
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
        }
        messages {
          nextToken
        }
        createdAt
        updatedAt
        userDevicesId
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
export const onDeleteMessage = /* GraphQL */ `
  subscription OnDeleteMessage($filter: ModelSubscriptionMessageFilterInput) {
    onDeleteMessage(filter: $filter) {
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
        }
        messages {
          nextToken
        }
        createdAt
        updatedAt
        userDevicesId
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
