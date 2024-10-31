#!/bin/bash
set -e
set -x

# List out the remotes, since that's what we're about to start fetching from
git remote --verbose

# Get the data from the server, and toss any stale remote branches that aren't
#  on the server anymore
git fetch --prune --verbose

# List all the branches, including the remote ones
git branch --list --all

# Rebase all the remote commits onto the local copy
git rebase remotes/origin/master master --verbose

# Show the recent commits
git log --pretty=oneline --max-count=10

# Show the current status
git status
