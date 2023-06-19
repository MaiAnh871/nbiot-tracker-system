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


