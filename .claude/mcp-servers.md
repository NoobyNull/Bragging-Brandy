# MCP Servers Configuration for Agentic Vibe Coding

## Overview
This document outlines the MCP (Model Context Protocol) servers configured for enhanced agentic coding capabilities in the Bragging-Brandy project.

## Configured Servers

### Core Development Tools
- **Filesystem Server**: Provides access to project files and directories
- **Git Server**: Enables version control operations and repository management
- **SQLite Server**: Database interaction for local data storage

### Advanced Capabilities
- **Puppeteer Server**: Web scraping and browser automation
- **Memory Server**: Persistent memory across sessions
- **Everything Server**: Universal search and indexing
- **Python Server**: Python code execution and environment management

### External Services (Disabled by default)
- **GitHub Server**: GitHub API integration (requires PAT)
- **Brave Search Server**: Web search capabilities (requires API key)
- **Google Drive Server**: Cloud storage integration (requires API key)

## Usage

### Enabling Disabled Servers
1. Set the required environment variables:
   - `GITHUB_PERSONAL_ACCESS_TOKEN` for GitHub server
   - `BRAVE_API_KEY` for Brave Search server
   - `GDRIVE_API_KEY` for Google Drive server

2. Change `"disabled": true` to `"disabled": false` in the configuration

### Server-Specific Configuration
- **Filesystem**: Automatically configured for project directory
- **Git**: Points to current repository
- **SQLite**: Creates database at project root if not exists

## Installation
All servers are configured to use `npx -y` for automatic installation when first accessed.

## Troubleshooting
- Ensure Node.js is installed for npm-based servers
- Check firewall settings for external service connections
- Verify API tokens have proper permissions