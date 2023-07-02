/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const getUser = /* GraphQL */ `
  query GetUser($id: ID!) {
    getUser(id: $id) {
      id
      username
      email
      name
      devices {
        items {
          id
          createdAt
          updatedAt
          userDevicesId
          username
        }
        nextToken
      }
      createdAt
      updatedAt
    }
  }
`;
export const listUsers = /* GraphQL */ `
  query ListUsers(
    $filter: ModelUserFilterInput
    $limit: Int
    $nextToken: String
  ) {
    listUsers(filter: $filter, limit: $limit, nextToken: $nextToken) {
      items {
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
      nextToken
    }
  }
`;
export const getDevice = /* GraphQL */ `
  query GetDevice($id: ID!) {
    getDevice(id: $id) {
      id
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
      username
    }
  }
`;
export const listDevices = /* GraphQL */ `
  query ListDevices(
    $filter: ModelDeviceFilterInput
    $limit: Int
    $nextToken: String
  ) {
    listDevices(filter: $filter, limit: $limit, nextToken: $nextToken) {
      items {
        id
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
        username
      }
      nextToken
    }
  }
`;
export const getMessage = /* GraphQL */ `
  query GetMessage($id: ID!) {
    getMessage(id: $id) {
      id
      device {
        id
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
        username
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
      overspeed_alert
      createdAt
      updatedAt
      deviceMessagesId
    }
  }
`;
export const listMessages = /* GraphQL */ `
  query ListMessages(
    $filter: ModelMessageFilterInput
    $limit: Int
    $nextToken: String
  ) {
    listMessages(filter: $filter, limit: $limit, nextToken: $nextToken) {
      items {
        id
        device {
          id
          createdAt
          updatedAt
          userDevicesId
          username
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
        overspeed_alert
        createdAt
        updatedAt
        deviceMessagesId
      }
      nextToken
    }
  }
`;
export const userByUserName = /* GraphQL */ `
  query UserByUserName(
    $username: String!
    $id: ModelIDKeyConditionInput
    $sortDirection: ModelSortDirection
    $filter: ModelUserFilterInput
    $limit: Int
    $nextToken: String
  ) {
    userByUserName(
      username: $username
      id: $id
      sortDirection: $sortDirection
      filter: $filter
      limit: $limit
      nextToken: $nextToken
    ) {
      items {
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
      nextToken
    }
  }
`;
