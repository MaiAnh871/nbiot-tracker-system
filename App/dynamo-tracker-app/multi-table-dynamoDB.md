# Create 3 table
- Table name
- Partion key: 
    - Partion key of `PostsTable` is `siteId`
    - Partion key of `CommentsTable` is `postId`
- Sort key
- Table setting: Customize Settings
- Read/write capacity settings: On-demand

# Create new API on AppSync
## Step 1: Select API type
- API option: GraphQL APIs
- GraphQL API Data Source: Design from scratch

## Step 2: Specify API details
- API name: `MultiTableBlog`

## Step 3: Specify GraphQL resources
- Create GraphQL resource later

# Modify Schema
```graphql
schema {
  query: Query
  mutation: Mutation
}

type Query {
  getSite(domain: String!): Site
  getPostsForSite(siteId: String!, num: Int, after: String): PostConnection
}

type Mutation {
  createSite(input: CreateSiteInput): Site
  createPost(input: CreatePostInput): Post
  createComment(input: CreateCommentInput): Comment
}

type Site {
  id: ID!
  name: String!
  domain: String!
  posts(num: Int, after: String): PostConnection!
}

type PostConnection {
  cursor: String
  posts: [Post]
}

type Post {
  id: ID!
  siteId: String!
  title: String!
  publishDate: AWSDateTime!
  content: String!
  comments(num: Int, after: String): CommentConnection!
}

type CommentConnection {
  cursor: String
  comments: [Comment]
}

type Comment {
  id: ID!
  username: String!
  content: String!
  publishDate: AWSDateTime!
}

input CreateSiteInput {
  name: String
  domain: String!
}

input CreatePostInput {
  siteId: String!
  title: String!
  publishDate: AWSDateTime
  content: String!
}

input CreateCommentInput {
  postId: String!
  username: String!
  publishDate: AWSDateTime
  content: String!
}
```

# Modify Data Sources



